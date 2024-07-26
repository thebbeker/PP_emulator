//
//  >>>  esp32_wlan_parallelport.ino <<<      T.Hebbeker  
//  
//  = software to emulate parallel port (centronics port, D-SUB 25 connector) via WLAN
//  NodeMCU esp32 12F board must be complemented by electronics and female D-SUB connector
//   
//  esp32 serves as a wlan server.
//  PC/notebook sends requests - via WLAN Router - to esp32 board 
//    which in turn addresses parallel port
//   
//  adapted to bbox3 WLAN router in Schievenhoevel 9, but can be changed easily 
//   by redefining STASSID and STAPSK 
//
//  code based on example WiFiManualWebServer.in and WiFiManualWebServer_test.ino
//  compilation and uploading to esp32 via Arduino IDE
// 
//  Arduino IDE:  board = esp32 dev module
//   port = /dev/ttyUSB0  or /dev/ttyUSB1  or ...  (can be used also in cpp programs)
//
//  sets eight output "data bits" and reads five "status bits" of esp32, 
//    connected to those bits of parallel port
//
//  on PC/notebook a c++ program esp32_wifi_parallelport.cpp with subroutine 
//     void parallelport_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate wlan commands to set/unset the appropriate pins of the esp32 board
//  similarly 
//     int parallelport_in() {  }   //   = 0 .. 31
//  should receive status bits when appropriate wlan command is sent 
//
//  parallel port input lines and status lines: see esp32_usb_parallelport.ino
//
//  can be tested from shell with commands like   curl http://192.168.1.199/D123
//
//
//  2024-03-31   v1.0 
//  2024-04-09   v2.0   clean up code 
//
//

#include <WiFi.h>

// Router bbox3 Schievenhoevel 9: network name and password
#ifndef STASSID
  #define STASSID "WiFi-2.4-081B"     // to be adapted
  #define STAPSK  "xxxxxxxxxxx"        // to be adapted
#endif

#define MONITOR 1       // for serial monitor: 0 = dont use, 1 = use in setup

const char* ssid = STASSID;
const char* password = STAPSK;

String inputString = "";      // a String to hold incoming data for serial communication
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data for serial communication

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const int high = 1;
const int low = 0; 

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {2,4,5,14,27,26,25,33};
const int GPIO_status_line[5] = {13,35,34,39,36};

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// prepare test LED 
const int test_led = 23;

void setup() {
  
  // for printing into serial monitor window, not necessary but good for debugging
  if(MONITOR>0) {
    Serial.begin(115200);
    delay(1000);   //  wait 1 ms
    Serial.println();
    Serial.print(F(" >>> esp32_wlan_parallelport <<<   MONITOR = "));
    Serial.println(MONITOR);
  }
  
  // Connect to WiFi network
  if(MONITOR>0) {
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(MONITOR>0) Serial.print(F("."));
  }
  delay(500);
  if(MONITOR>0) {
    Serial.println();
    Serial.println(F("WiFi connected"));
  }
  // Print the IP address
  if(MONITOR>0) {
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  }
  // Start the server
  server.begin();
  if(MONITOR>0) Serial.println(F("Server started"));
  delay(500);
  
  // prepare test LED 
  pinMode(test_led, OUTPUT);
  
  // prepare GPIOs for data lines
  //  and set everything to low
  for(int i=0; i<8; i++) {
    pinMode(GPIO_data_line[i], OUTPUT);
    digitalWrite(GPIO_data_line[i], low);
  }
  
  // prepare GPIOs for status lines
  for(int i=0; i<5; i++) {
    pinMode(GPIO_status_line[i], INPUT);
  }
  
  // say hello, use GPIO23 with external yellow LED, replacing missing builtin led
  delay(1000);
  for(int m=1; m<=6; m++) {
    digitalWrite(test_led, high); 
    delay(300);
    digitalWrite(test_led, low);
    delay(300);
  }
  
  
}

void loop() {
  
  // Check if a client = notebook or ....  has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  client.setTimeout(5000); // in milliseconds, default is 1000 
  
  // Read the first line of the inputStringuest
  
  String inputString = client.readStringUntil('\r');
  
  // Match the request
  
  // get data from PC
  if(inputString.substring(5,6) == "D") {   // substring syntax:  from, to   inclusive, exclusive !
    
    databyte = inputString.substring(6,9).toInt(); // 3 digits + LF
    
    for(int i=0; i<8; i++) {
      bit[i] = ((databyte >> i) & 0x01);
    }
    
    for(int i=0; i<8; i++) {
      if(bit[i]!=oldbit[i]) {
        oldbit[i]=bit[i]; 
        digitalWrite(GPIO_data_line[i],bit[i]);
      }
    }
    
  }
  
  // send status data to PC
  if(inputString.substring(5,6) == "S") {
    
    factor = 1; 
    statusbyte = 0;
    for(int i=0; i<5; i++) {
      bit[i] = digitalRead(GPIO_status_line[i]);
      statusbyte = statusbyte + bit[i]*factor; 
      factor = factor*2; 
    }
    
    statusbyte = statusbyte + 10;  // guarantees 2 characters
    outputString = String(statusbyte);
    Serial.print(outputString);
    
  }
  
  // allow access to test led   eg via curl command from shell 
  if (inputString.indexOf(F("/dt/1")) != -1) {
    digitalWrite(test_led, high);
  }
  if (inputString.indexOf(F("/dt/0")) != -1) {
    digitalWrite(test_led, low);
  }
  
  while (client.available()) {
    client.read();
  }
  
  // Send the response to the client
  client.print(F("HTTP/1.1 200 OK\r\nConnt-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n"));  // needed !
  
  client.print(outputString); 
  client.print("\n");
  
  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  
}
