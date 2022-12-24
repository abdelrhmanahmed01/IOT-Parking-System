

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Servo.h>
Servo myservo;
int carEnter_IR = D7;        
int carExited_IR = D6;  

int slot_1=D3;
int slot_2=D2;
int led_slot1 = D0;
int led_slot2= D1;
int num_of_cars=0;
// Replace with your network credentials
const char* ssid     = "";
const char* password = "";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "closed";


// Assign output variables to GPIO pins
const int output5 = D5;
int pos = 0;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(carExited_IR, INPUT);    // ir as input
  pinMode(carEnter_IR, INPUT);
  pinMode(slot_1,INPUT);
  pinMode(slot_2,INPUT); 
   
   pinMode(led_slot1,OUTPUT);
   pinMode(led_slot2,OUTPUT); // Led pin OUTPUT

  // Set outputs to LOW
 // digitalWrite(output5, LOW);
 myservo.attach(output5); 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}





void loop(){
   oped_close_gate_automatically();
 int slot_1_status=digitalRead(slot_1);
 int slot_2_status=digitalRead(slot_2);
  String myString;
 myString= car_parking(slot_1_status,slot_2_status,myString);
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "opend";
              open_gate();
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "closed";
              close_gate();
  
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv='refresh' content='3'><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body style='background-color:powderblue;'><h1>parking car system </h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5
            if ( slot_1_status==1){
            client.println("<p> state of the slot 1 now : Available""</p>");
            }
            else{
              client.println("<p> state of the slot 1 now : Not Available""</p>");
            }
             if ( slot_2_status==1){
            client.println("<p> state of the slot 2 now : Available""</p>");
            }
            else{
              client.println("<p> state of the slot 2 now : Not Available""</p>");
            }
           // client.println("<p> state of the slot 2 now : " + myString2 + "</p>");
            client.println("<p> The number of parked cars now : " + myString + "</p>");  
            client.println(" <p>the state of the gate now : " + output5State + "</p>");

            // If the output5State is off, it displays the ON button       
            if (output5State=="closed") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">Open</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">Close</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void oped_close_gate_automatically(){

if ( digitalRead(carEnter_IR)==0&&num_of_cars<2){
  open_gate();
  delay(5000);
  close_gate();
  delay(3000);
    }   
     if(digitalRead (carExited_IR)==0){
  open_gate();
  delay(5000);
  close_gate();
  delay(3000);
}
}
String car_parking(int slot1,int slot2,String string1){
 
if (slot1==0){
  digitalWrite(led_slot1, LOW);
  num_of_cars++;
}
else {
  num_of_cars=0;
  digitalWrite(led_slot1, HIGH);
}
if (slot2==0){
  digitalWrite(led_slot2, LOW);
  num_of_cars++;
}
else {
  num_of_cars=0;
  digitalWrite(led_slot2, HIGH);
}
num_of_cars=(!slot1)+(!slot2);
string1 = String(num_of_cars);
return string1;
}
void open_gate(){
 for (pos = 0; pos <= 180; pos += 1) { 
             myservo.write(pos);              
          } 
}
void close_gate(){
      for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);                                  
  }
}