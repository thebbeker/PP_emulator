
//
//  >>>  arduino_ethernet_parallelport.ino <<<      T.Hebbeker  
//  
//  = Software to emulate parallel port (centronics port, D-SUB connector) via ethernet interface
//  Arduino Uno board must be complemented by ethernet shield and further electronics and female D-SUB connector
//  arduino is connected via standard RJ45 ethernet cable to router, notebook via wifi to same router
//   
//  Arduino Uno translates between notebook/PC and parallel port.
//  Notebook sends requests - via ethernet cable - to arduino board which in turn addresses parallel port.
//
//  Compilation and uploading to arduino via Arduino IDE
//  use board = "Arduino Uno"  
//  and chose appropriate usb port to upload the code to the arduino 
//
//  sets 8 output "data line bits" and reads 5 "status bits" of Arduino uno, 
//    connected to those bits of parallel port
//
//  On the PC/notebook a c++ program parallelport_test.cpp with subroutine 
//     void parallelport_ethernet_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate commands to set/unset the appropriate pins of the arduino board
//  similarly 
//     int parallelport_ethernet_in() {  }   //   = 0 .. 31 
//  should receive control bits when appropriate  command is sent 
//
//  parallel port data lines:
//  ---------------------------
//  arduino board pins         => parallel port = PP
//    D2                       => Data out bit 0   Pin 2
//    D3                       => Data out bit 1   Pin 3
//    D4                       => Data out bit 2   Pin 4  => Pin 19   (4 used for SDcard)
//    D5                       => Data out bit 3   Pin 5
//    D6                       => Data out bit 4   Pin 6
//    D7                       => Data out bit 5   Pin 7
//    D8                       => Data out bit 6   Pin 8
//    D9                       => Data out bit 7   Pin 9
//
//  note: cant use D0 and D1 (RX and TX) since are used by UART - ethernet communication    
//
// parallel port status lines:                                    
//  ---------------------------
//  arduino board pins           <= parallel port = PP
//    A0   = 14                  <= Data in Pin 10  ACK                    
//    A1   = 15                  <= Data in Pin 11  BUSY inverted         
//    A2   = 16                  <= Data in Pin 12  PAPER (_END)         
//    A3   = 17                  <= Data in Pin 13  SELECT                  
//    A4   = 18                  <= Data in Pin 15  ERROR            
//
// the following hardware connections are needed:
//
//    GND(Arduino) = GND(PP) (lines 18-25) 
//
//    5V (arduino) = pin 1 (PP) = Strobe (inverted)
// 
//    all incoming status lines from parallel port might have up to + 5V.
//      This is ok for Arduino uno which runs on 5V (so output lines get 5V, and not 3.3V)
//      use the following external pull down resistors to make "LOW" = 0 the default 
//         for unconnected status lines:
//         10 kOhm for  ACK, BUSY, PAPER, SELECT, ERROR to GND 
//
//  note: built-in LED (pin 13) not usable if ethernet interface is active. 
//        use instead pin 19 with external LED 
//        attention: pin 19 used as 3rd data line in parallel port interface
//          there could be conflicts if at startup pin 19 "blinks" while data lines are expected to remain low
//   
//  2024-04-13   v1.0
//  2024-04-15   v1.1  cleanup 
// 

#include <SPI.h>
#include <Ethernet.h>

// Ethernet Konfiguration
// byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // till 2024-05-10 
// byte mac[] = { 0x6C, 0x2B, 0x59, 0x3D, 0x82, 0xF4 };  // tried out 2024-05-11
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xDF, 0x8F };  // tried out 2024-05-11   from sticker on shield!
byte ip[] = { 192, 168, 1, 199 };

// Server-Objekt mit Standardport 80
EthernetServer server(80);

const int high = 1;
const int low = 0; 

String outputString;
String inputString;

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {2,3,19,5,6,7,8,9};
const int GPIO_status_line[5] = {14,15,16,17,18};

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

int index; 

// for test LED
const int test_led = 19;   // pins 9,19 work, 10-13 not !?!

void setup() {

  // for communication with serial monitor via USB
  Serial.begin(115200);   // default = standard = well tested

  // Ethernet initialisation
  Ethernet.begin(mac, ip);

  // Server ready, waiting for requests
  server.begin();

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

  // say hello upon startup 
  delay(1000);
  for(int m=1; m<=7; m++) {
    digitalWrite(test_led, high); 
    delay(300);
    digitalWrite(test_led, low);
    delay(300);
  }
  
}

void loop() {
	
  // Check if a client = notebook or ....  has connected
  EthernetClient client = server.available();

  if (client)
  { 

/*
    server.println("HTTP/1.1 200 OK");
    server.println("Content-Type: text/html");
    server.println();
    server.print("<HTML><HEAD><TITLE>");
    server.print("Arduino Ethernet");
    server.print("</TITLE>");
    server.print("</HEAD><BODY>");
    server.print("<h1> Arduino ethernet parallelport interface </h1>");
    */
    

    
  // Read the first line of the inputString
  inputString = client.readStringUntil('\r');
  
  outputString = ""; 
  
  // Match the request
  
  // get data from PC
  if(index=inputString.indexOf(F("/D")) != -1) {  
    
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
  if(index=inputString.indexOf(F("/S")) != -1) {  
    
    factor = 1; 
    statusbyte = 0;
    for(int i=0; i<5; i++) {
      bit[i] = digitalRead(GPIO_status_line[i]);
      statusbyte = statusbyte + bit[i]*factor; 
      factor = factor*2; 
    }
    
    statusbyte = statusbyte + 10;  // guarantees 2 characters
    outputString = String(statusbyte);
    // Serial.print(outputString);
    
  }

 



   // inputString = "77/dt/1X";  // testing 
   //  outputString = "29";   // testing
  
  // allow access to test led   eg via curl command from shell 
  if (inputString.indexOf(F("/dt/1")) != -1) {
    digitalWrite(test_led, high);
  }
  if (inputString.indexOf(F("/dt/0")) != -1) {
    digitalWrite(test_led, low);
  }



    // Send the response to the client
  server.print(F("HTTP/1.1 200 OK\r\nConnt-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n"));  // needed !
  server.print(outputString); 
  server.print("\n");
  

/*

  server.print("\n");
  
  server.print(inputString); 
  server.print("\n");

  server.print(index); 
  server.print("\n");
  
  server.print(outputString); 
  server.print("\n");

    server.print("</BODY></HTML>");

*/
    
    delay(10); 
    client.stop();

  }  // end client

}  // end loop 

  
