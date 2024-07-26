//
//  >>>  arduino_uno_usb_parallelport.ino <<<      T.Hebbeker  
//  
//  = Software to emulate parallel port (centronics port, D-SUB connector) via USB interface
//  Arduino Uno board must be complemented by electronics and female D-SUB connector
//   
//  Arduino Uno translates between notebook/PC and parallel port.
//  Notebook sends requests - via USB cable - to arduino board which in turn addresses parallel port.
//
//  Compilation and uploading to arduino via Arduino IDE
//  use board = "Arduino Uno"  
//  and chose appropriate USB port
//
//  sets 8 output "data line bits" and reads 5 "status bits" of Arduino Uno, 
//    connected to those bits of parallel port
//
//  On the PC/notebook a c++ program arduino_usb_parallelport.cpp with subroutine 
//     void parallelport_usb_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate commands to set/unset the appropriate pins of the arduino board
//  similarly 
//     int parallelport_usb_in() {  }   //   = 0 .. 31 
//  should receive control bits when appropriate  command is sent 
//
//  parallel port data lines:
//  ---------------------------
//  arduino board pins         => parallel port = PP
//    D2                       => Data out bit 0   Pin 2
//    D3                       => Data out bit 1   Pin 3
//    D4                       => Data out bit 2  (Pin 4)  => Pin 19 to make it compatible with SD card on ethernet shield
//    D5                       => Data out bit 3   Pin 5
//    D6                       => Data out bit 4   Pin 6
//    D7                       => Data out bit 5   Pin 7
//    D8                       => Data out bit 6   Pin 8
//    D9                       => Data out bit 7   Pin 9
//
//  note: cant use D0 and D1 (RX and TX) since are used by UART - USB communication    
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
//      This is ok for Arduino Uno which runs on 5V (so output lines get 5V, and not 3.3V)
//      use the following external pull down resistors to make "LOW" = 0 the default 
//         for unconnected status lines:
//         10 kOhm for  ACK, BUSY, PAPER, SELECT, ERROR to GND 
//
//    note: builtin LED (pin 13) not usable if ethernet shield is attached and used
//   
//  2024-04-14   v1.0   ! compatible with ethernet shield ! 
// 

String inputString = "";      // a String to hold incoming data for serial communication
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data for serial communication 

const int high = 1;
const int low = 0; 

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {2,3,19,5,6,7,8,9};
const int GPIO_status_line[5] = {14,15,16,17,18};

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// for on-board built-in LED
const int builtin_led = 13;   


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

  if (stringComplete) {

     // get data from PC
     if(inputString.substring(0,1) == "D") {
     
       databyte = inputString.substring(1,8).toInt(); 
       
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

       statusbyte = statusbyte + 10;  // guarantees 3 characters
       outputString = String(statusbyte);
       Serial.print(outputString);

     }
     
     inputString = "";
     stringComplete = false;

   }  // end StringComplete
        

}  // end loop 

  
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.

  from https://docs.arduino.cc/built-in-examples/communication/SerialEvent/

*/
void serialEvent() {
  while (Serial.available()) {
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
}
