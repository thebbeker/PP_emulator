//
//  >>>  esp32_usb_parallelport.ino <<<      T.Hebbeker  
//  
//  = software to emulate parallel port (centronics port, D-SUB 25 connector) via USB interface
//  esp32 NodeMCU board must be complemented by electronics and female D-SUB connector
//   
//  PC/notebook sends requests - via usb cable - to esp32 board 
//    which in turn addresses parallel port
//  exact speficifation: ESP-32 Dev Kit C V4 from AZ-Delivery
//   
//  compilation and uploading to esp32 via Arduino IDE
//  use board = "ESP32 dev module"  (if this is appropriate)
//  and chose appropriate USB port, eg = /dev/ttyUSB0  or /dev/ttyUSB1 
//
//  sets eight output "data bits" and reads five "status bits" of esp32, 
//    connected to those bits of parallel port
//
//  on PC/notebook a c++ program esp32_usb_parallelport.cpp with subroutine 
//     void parallelport_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate usb commands to set/unset the appropriate pins of the esp32 board
//  similarly 
//     int parallelport_in() {  }   //   = 0 .. 31
//  should receive status bits when appropriate usb command is sent 
//
//  parallel port input lines:
//  ---------------------------
//    esp32 board pins           => parallel port 
//    D2  = GPIO02               => Data out bit 0   Pin 2
//    D4  = GPIO04               => Data out bit 1   Pin 3
//    D5  = GPIO05               => Data out bit 2   Pin 4
//    14  = GPIO14               => Data out bit 3   Pin 5
//    27  = GPIO27               => Data out bit 4   Pin 6
//    26  = GPIO26               => Data out bit 5   Pin 7
//    25  = GPIO25               => Data out bit 6   Pin 8
//    33  = GPIO33               => Data out bit 7   Pin 9
//   
// parallel port status lines:                                    
//  ---------------------------
//  esp32 board pins <= parallel port = PP
//    13 = GPIO13         <= Data in  Pin 10  ACK        
//    35 = GPIO35         <= Data in  Pin 11  BUSY inverted        
//    34 = GPIO34         <= Data in  Pin 12  PAPER (_END)         
//    39 = GPIO39         <= Data in  Pin 13  SELECT   
//    36 = GPIO36         <= Data in  Pin 15  ERROR         
//
// the following hardware connections, voltage dividers and pull up resistors are needed:
//
//    GND(ESP8288) = GND(PP) (lines 18-25) 
//
//    5V (esp32) = pin 1 (PP) = Strobe (inverted)
// 
//    all incoming status lines from parallel port might have + 5V, too much for EPS8266,
//      thus use the followint voltage dividers for ACK, BUSY, PAPER, SELECT  
//      GND - 470 Ohm --- 220 Ohm - PP      this scales down 5V input at PP to 3.4 V at esp32
//                     |
//                    ESP
//
//      These resistor values are not yet optimized !!!
//
//    Note: during boot of the esp32 the output data lines are NOT all at "LOW" !
//    Note: the use of RX and TX as output lines prevents use of serial monitor!
//
//   can be tested by shell commands like echo "D255\n" > /dev/ttyUSB0  (wait till boot of esp32 completed)
//
//
//  2024-03-31   v1.0   inspired by esp8266_usb_parallelport.ino 
//  2024-04-09   v2.0   clean up code 
//

String inputString = "";      // a String to hold incoming data for serial communication
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data for serial communication

const int high = 1;
const int low = 0; 

int buttonvalue; 

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {2,4,5,14,27,26,25,33};
const int GPIO_status_line[5] = {13,35,34,39,36};

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// replace missing on-board LED by external yellow low current LED plus 470 Ohm resistor
const int test_led = 23;   


void setup() {

  // for USB communication
  Serial.begin(115200);   // default = standard 

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
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
  for(int m=1; m<=5; m++) {
    digitalWrite(test_led, high); 
    delay(300);
    digitalWrite(test_led, low);
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
    
     // get data from PC
     if(inputString.substring(0,1) == "D") {
     
       // databyte = inputString.substring(1,8).toInt(); 
       databyte = inputString.substring(1,4).toInt(); //  = position 1 to 3 
       
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
     if(inputString.substring(0,1) == "S") {
    
       factor = 1; 
       statusbyte = 0;
       for(int i=0; i<5; i++) {
         bit[i] = digitalRead(GPIO_status_line[i]);
         statusbyte = statusbyte + bit[i]*factor; 
         factor = factor*2; 
       }

       // statusbyte = 29;   // test

       statusbyte = statusbyte + 10;  // guarantees 2 characters
       outputString = String(statusbyte);
       Serial.print(outputString);

     }
  
     inputString = "";
     stringComplete = false;


   }
   

}
