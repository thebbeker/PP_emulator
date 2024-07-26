//
//  >>>  raspberry_pico_usb_parallelport.ino <<<      T.Hebbeker  
//  
//  = Software to emulate parallel port (centronics port, D-SUB connector) via USB interface
//  raspberry pico board must be complemented by electronics and female D-SUB connector
//   
//  raspberry pico translates between notebook/PC and parallel port.
//  Notebook sends requests - via USB cable - to raspberry board which in turn addresses parallel port.
//
//  Compilation and uploading to raspberry via raspberry IDE
//  use board = "raspberry pico"  (if this is appropriate)
//  and chose appropriate USB port
//  upload (flush) code: press raspberry reset button when inserting USB cable. 
//
//  sets 8 output "data line bits" and reads 5 "status bits" of raspberry pico, 
//    connected to those bits of parallel port
//
//  On the PC/notebook a c++ program raspberry_usb_parallelport.cpp with subroutine 
//     void parallelport_usb_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate commands to set/unset the appropriate pins of the raspberry board
//  similarly 
//     int parallelport_usb_in() {  }   //   = 0 .. 31 
//  should receive control bits when appropriate  command is sent 
//
//  parallel port data lines:
//  ---------------------------
//  raspberry board pins       => parallel port = PP
//     2                       => Data out bit 0   Pin 2
//     3                       => Data out bit 1   Pin 3
//     6                       => Data out bit 2   Pin 4
//     7                       => Data out bit 3   Pin 5
//    10                       => Data out bit 4   Pin 6
//    11                       => Data out bit 5   Pin 7
//    14                       => Data out bit 6   Pin 8
//    15                       => Data out bit 7   Pin 9
//
//  note: cant use D0 and D1 (RX and TX) since are used by UART - USB communication    
//
// parallel port status lines:                                    
//  ---------------------------
//  raspberry board pins         <= parallel port = PP
//    18                         <= Data in Pin 10  ACK                    
//    19                         <= Data in Pin 11  BUSY inverted         
//    20                         <= Data in Pin 12  PAPER (_END)         
//    21                         <= Data in Pin 13  SELECT                  
//    22                         <= Data in Pin 15  ERROR            
//
// the following hardware connections are needed:
//
//    GND(raspberry) = GND(PP) (lines 18-25) 
//
//    5V (raspberry) = pin 1 (PP) = Strobe (inverted)
// 
//    all incoming status lines from parallel port might have up to + 5V.
//      thus use the followint voltage dividers for ACK, BUSY, PAPER, SELECT  
//      GND - 47 kOhm --- 22 kOhm - PP      this scales down 5V input at PP to 3.4 V at raspberry
//                     |
//                    Pico
//
//   can be tested by shell commands like     echo "D255\n" > /dev/ttyUSB0
//   
//  2024-04-17   v1.0
// 

String inputString = "";      // a String to hold incoming data for serial communication
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data for serial communication 

const int high = 1;
const int low = 0; 

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] =  {2,3,6,7,10,11,14,15};
const int GPIO_status_line[5] = {18,19,20,21,22}; 

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// for on-board built-in LED
const int builtin_led = 25;   


void setup() {

  // for USB communication
  Serial.begin(115200);   // default = standard = well tested

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  // prepare builtin LED 
  pinMode(builtin_led, OUTPUT);
  
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

     // get data from PC
     if(inputString.substring(0,1) == "D") {
     
       databyte = inputString.substring(1,4).toInt(); 
       
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

   
  // allow access to builtin led   
  if (inputString.indexOf(F("/dt/1")) != -1) {
    digitalWrite(builtin_led, high);
  }
  if (inputString.indexOf(F("/dt/0")) != -1) {
    digitalWrite(builtin_led, low);
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

   }  // end StringComplete
        

}  // end loop 
