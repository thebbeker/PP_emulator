#include <dummy.h>

//
//  >>>  esp8266_wlan_parallelport.ino <<<      T.Hebbeker  
//  
//  = software to emulate parallel port (centronics port, D-SUB 25 connector)
//  NodeMCU ESP8266 12F board must be complemented by electronics and female D-SUB connector
//
//  code inspired by
//  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiManualWebServer/
//   
//  esp8266 serves as a wlan server.
//  PC/notebook sends requests - via WLAN Router - to esp8266 board 
//    which in turn addresses parallel port
//   
//  adapted to bbox3 WLAN router in Schievenhoevel 9, but can be changed easily 
//   by redefining STASSID and STAPSK 
//
//  code based on example WiFiManualWebServer.in and WiFiManualWebServer_test.ino
//  compilation and uploading to ESP8266 via Arduino IDE
// 
//  Arduino IDE:  board = generic ESP8266 module
//   port = /dev/ttyUSB0  or /dev/ttyUSB1  or ...  (can be used also in cpp programs)
//
//  sets eight output "data bits" and reads five "status bits" of ESP8266, 
//    connected to those bits of parallel port
//
//  on PC/notebook a c++ program esp8266_wifi_parallelport.cpp with subroutine 
//     void parallelport_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate wlan commands to set/unset the appropriate pins of the esp8266 board
//  similarly 
//     int parallelport_in() {  }   //   = 0 .. 31
//  should receive status bits when appropriate wlan command is sent 
//
//  parallel port input lines:
//  ---------------------------
//  NodeMCU esp8266 board pins => parallel port 
//    D0 = GPIO16              => Data out bit 0   Pin 2
//    TX = GPIO1               => Data out bit 1   Pin 3
//    RX = GPIO3               => Data out bit 2   Pin 4
//    D3 = GPIO0               => Data out bit 3   Pin 5
//    D4 = GPIO2               => Data out bit 4   Pin 6
//    D5 = GPIO14              => Data out bit 5   Pin 7
//    D6 = GPIO12              => Data out bit 6   Pin 8
//    D7 = GPIO13              => Data out bit 7   Pin 9
//   
// parallel port status lines:                                    
//  ---------------------------
//  NodeMCU esp8266 board pins <= parallel port = PP
//    S0 = D8   = GPIO15         <= Data in bit 6   Pin 10  ACK             
//    S1 = D2   = GPIO4          <= Data in bit 7   Pin 11  BUSY inverted        
//    S2 = D1   = GPIO5          <= Data in bit 5   Pin 12  PAPER (_END)         
//    S3 = SD3  = GPI10          <= Data in bit 4   Pin 13  SELECT      
//    S4  ! use ADC0 !           <= Data in bit 3   Pin 15  ERROR  
//
// the following hardware connections, voltage dividers and pull up resistors are needed:
//
//    GND(ESP8288) = GND(PP) (lines 18-25) 
//
//    VIN = 5V (ESP8266) = pin 1 (PP) = Strobe (inverted)
// 
//    all incoming status lines from parallel port might have + 5V, too much for EPS8266,
//      thus use the followint voltage dividers for ACK, BUSY, PAPER, SELECT  
//      GND - 470 Ohm --- 220 Ohm - PP      this scales down 5V input to 3.4 V at ESP8266
//                     |
//                    ESP
//
//    ADC input for ERROR statut bit:
//       to avoid too high voltage (max 3.3 V permitted for NodeMCU ESP8266 use voltage divider, 
//       but "inverted" with respect to normal GPIO inputs: 
//       GND - 220 Ohm --- 470 Ohm - PP    this scales down 5V input to 1.6 V (ADC = 124) 
//                      |                     at ESP8266 - enough to recognize it
//                     ESP
//  
//      These resistor values are not yet optimized !!!
//
//    Note: during boot of the ESP8266 the output data lines are NOT all at "LOW" !
//    Note: the use of RX and TX as output lines prevents Serial.print to serial monitor!
//
//    code can be tested with shell commands like   curl http://192.168.1.199/D255
//   
//  2024-01-30   v1.0   as esp8266_wlan_parallel-port.ino (hyphen!)
//  2024-04-16   v2.0   cleanup 
//
//

#include <ESP8266WiFi.h>

// Router bbox3 Schievenhoevel 9: network name and password
#ifndef STASSID
#define STASSID "WiFi-2.4-081B"     // to be adapted
#define STAPSK  "xxxxxxxxxxx"        // to be adapted
#endif

#define MONITOR 1       // 0 = dont use, 1 = only in setup, 
                        // 2 = also in loop (here not working due to RX/TX use as data lines)

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const int high = 1;
const int low = 0; 

String inputString;
String outputString;  

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {16,1,3,0,2,14,12,13};
const int GPIO_status_line[5] = {15,4,5,10,99};   // last value not used - instead ADC0

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// for on-board LED (inverted! lit when low)
const int builtin_led = 2;   

int adcvalue;
int adcvalue_threshold = 64;   //  corresponding to about 1.6V/2  see above


void setup() {

  // for printing into serial monitor window, not necessary but good for debugging
  if(MONITOR>0) {
    Serial.begin(115200);
    delay(1000);   //  wait 1 ms
    Serial.println();
    Serial.print(F(" >>> esp8266_wlan_parallelport <<<   MONITOR = "));
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

 // prepare LED 
  pinMode(builtin_led, OUTPUT);

  // prepare GPIOs for data lines
  //  and set everything to low
  for(int i=0; i<8; i++) {
    pinMode(GPIO_data_line[i], OUTPUT);
    digitalWrite(GPIO_data_line[i], low);
  }

  // prepare GPIOs for status lines
  for(int i=0; i<4; i++) {     // only 4 of 5 status lines
    pinMode(GPIO_status_line[i], INPUT);
  }
 
  // say hello with built-in LED
  delay(2000);
  for(int m=1; m<=6; m++) {
    digitalWrite(builtin_led, high); 
    delay(250);
    digitalWrite(builtin_led, low);
    delay(250);
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
    for(int i=0; i<4; i++) {      // read only 4 status lines, 5th via adc
      bit[i] = digitalRead(GPIO_status_line[i]);
      statusbyte = statusbyte + bit[i]*factor; 
      factor = factor*2; 
    }
    
     // special treatment for A4 = ERROR bit: use ADC
	   bit[4] = 0; 
     adcvalue = analogRead(A0);
   	 if (adcvalue > adcvalue_threshold) bit[4] = 1; 
	   statusbyte = statusbyte + bit[4]*factor; 
    
    statusbyte = statusbyte + 10;  // guarantees 2 characters
    outputString = String(statusbyte);
    Serial.print(outputString);
    
  }
  
  // allow access to test buittin led   eg via curl command from shell 
  if (inputString.indexOf(F("/dt/1")) != -1) {
    digitalWrite(builtin_led, high);
  }
  if (inputString.indexOf(F("/dt/0")) != -1) {
    digitalWrite(builtin_led, low);
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
  
  
