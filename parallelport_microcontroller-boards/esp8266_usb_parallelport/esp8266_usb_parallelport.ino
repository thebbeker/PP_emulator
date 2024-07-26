//
//  >>>  esp8266_usb_parallelport.ino <<<      T.Hebbeker  
//  
//  = software to emulate parallel port (centronics port, D-SUB 25 connector)
//  NodeMCU ESP8266 12F board must be complemented by electronics and female D-SUB connector
//   
//  PC/notebook sends requests - via usb cable - to esp8266 board 
//    which in turn addresses parallel port
//   
//  compilation and uploading to ESP8266 via Arduino IDE
// 
//  Arduino IDE:  board = generic ESP8266 module
//   port = /dev/ttyUSB0  or /dev/ttyUSB1  or ...  (can be used also in cpp programs)
//
//  sets eight output "data bits" and reads five "status bits" of ESP8266, 
//    connected to those bits of parallel port
//
//  on PC/notebook a c++ program esp8266_usb_parallelport.cpp with subroutine 
//     void parallelport_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate usb commands to set/unset the appropriate pins of the esp8266 board
//  similarly 
//     int parallelport_in() {  }   //   = 0 .. 31
//  should receive status bits when appropriate usb command is sent 
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
//    S3 = SD3  = GPIO10         <= Data in bit 4   Pin 13  SELECT      
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
//    Note: the use of RX and TX as output lines 1,2 interferes with USB communication
//          THUS data lines 1,2 CAN NOT BE USED! 
//
//   can be tested by shell commands like echo "D255\n" > /dev/ttyUSB0
//
//  2024-03-16   v1.0   inspired by esp8266_wlan_parallelport.ino and arduino_usb_parallel-port.ino
//  2024-03-24   v1.1   move code from serialevent into loop() - else does not work. Help from David!
//  2024-04-15   v2.0   cleanup
//

String inputString = "";      // a String to hold incoming data for serial interface 
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data

const int high = 1;
const int low = 0; 

int buttonvalue; 

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {16,1,3,0,2,14,12,13};
const int GPIO_status_line[5] = {15,4,5,10,99};   // last value not used - instead ADC0

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// for on-board LED:
const int builtin_led = 2;   

int adcvalue;
int adcvalue_threshold = 64;   //  corresponding to about 1.6V/2  see above


void setup() {

  
  // for USB communication
  Serial.begin(115200);   // default = standard 
  // Serial.begin(230400);   // too fast, not working
  // Serial.begin(9600);    // slowly .... for testing

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  // prepare LED 
  pinMode(builtin_led, OUTPUT);

  // prepare GPIOs for data lines
  //  and set everything to low
  for(int i=0; i<8; i++) {
    if(i<1 || i>2 ) {
      pinMode(GPIO_data_line[i], OUTPUT);
      digitalWrite(GPIO_data_line[i], low);
    }
  }

  // prepare GPIOs for status lines
  for(int i=0; i<4; i++) {     // only 4 of 5 status lines
    pinMode(GPIO_status_line[i], INPUT);
  }
 
  // say hello with built-in LED
  delay(1000);
  for(int m=1; m<=5; m++) {
    digitalWrite(builtin_led, high); 
    delay(300);
    digitalWrite(builtin_led, low);
    delay(300);
  }

   
}

void loop() {

   while (Serial.available()) {
    // 
    // from https://docs.arduino.cc/built-in-examples/communication/SerialEvent/
    // 
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  
  if (stringComplete) {

    //   testing
     /*
     if(inputString.substring(0,3) == "D10") digitalWrite(GPIO_data_line[1],0);
     if(inputString.substring(0,3) == "D11") digitalWrite(GPIO_data_line[1],1);
     */
     //

     // get data from PC
	   if(inputString.substring(0,1) == "D") {
     
       // databyte = inputString.substring(1,8).toInt(); 
       databyte = inputString.substring(1,4).toInt(); // = position 1 to 3 
       
       for(int i=0; i<8; i++) {
         bit[i] = ((databyte >> i) & 0x01);
       }
    
       for(int i=0; i<8; i++) {
         if(i<1 || i>2 ) {
           if(bit[i]!=oldbit[i]) {
             oldbit[i]=bit[i]; 
             digitalWrite(GPIO_data_line[i],bit[i]);
           }
         }
       }

     }

     // send status data to PC
     if(inputString.substring(0,1) == "S") {
    
       factor = 1; 
       statusbyte = 0;
       for(int i=0; i<4; i++) {    // only 4 status lines !
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

     
     inputString = "";
     stringComplete = false;

   }  // end string complete
   

}  // end loop

  
