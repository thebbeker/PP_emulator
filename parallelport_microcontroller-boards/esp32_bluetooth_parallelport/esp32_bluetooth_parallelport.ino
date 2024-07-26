//
//  >>>  esp32_bluetooth_parallelport.ino <<<      T.Hebbeker  
//  
//  = software to emulate parallel port (centronics port, D-SUB 25 connector) via bluetooth
//  esp32 board must be complemented by electronics and female D-SUB connector
// 
//  PC/notebook sends requests - via bluetooth - to esp32 board 
//    which in turn addresses parallel port
//  exact speficifation: ESP-32 Dev Kit C V4 from AZ-Delivery
//   
//  compilation and uploading to esp32 via Arduino IDE
//  Arduino IDE:  board = ESP32 dev module 
//   port = /dev/ttybluetooth0  or /dev/ttybluetooth1  or ...  (can be used also in cpp programs)
//
//  sets eight output "data bits" and reads five "status bits" of esp32, 
//    connected to those bits of parallel port
//
//  on PC/notebook a c++ program esp32_bluetooth_parallelport.cpp with subroutine 
//     void parallelport_out(int byte) {  }   //  byte = 0 .. 255 
//  should send appropriate bluetooth commands to set/unset the appropriate pins of the esp32 board
//  similarly 
//     int parallelport_in() {  }   //   = 0 .. 31
//  should receive status bits when appropriate bluetooth command is sent 
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
//    13 = GPIO13         <= Data in bit 6   Pin 10  ACK             
//    35 = GPIO35         <= Data in bit 7   Pin 11  BUSY inverted        
//    34 = GPIO34         <= Data in bit 5   Pin 12  PAPER (_END)         
//    39 = GPIO39         <= Data in bit 4   Pin 13  SELECT   
//    36 = GPIO36         <= Data in bit 4   Pin 13  ERROR    
//
// the following hardware connections, voltage dividers and pull up resistors are needed:
//
//    GND(ESP8288) = GND(PP) (lines 18-25) 
//
//    5V (esp32) = pin 1 (PP) = Strobe (inverted)
// 
//    all incoming status lines from parallel port might have + 5V, too much for EPS8266,
//      thus use the followint voltage dividers for ACK, BUSY, PAPER, SELECT  
//      GND - 47 kOhm --- 22 kOhm - PP      this scales down 5V input to 3.4 V at esp32
//                     |
//                    ESP
//
//      These resistor values are not yet optimized !!!
//
//    Note: during boot of the esp32 the output data lines are NOT all at "LOW" !
//    Note: the use of RX and TX as output lines prevents Serial.print to serial monitor!
//
//    To connect to bluetooth use terminal command such as   
//        sudo rfcomm bind /dev/rfcomm0  D8:BC:38:E6:23:76 1   (put proper address!)
//    For testing from terminal use commands such as    echo 'B100' >/dev/rfcomm0 
//
//  2024-04-03   v1.0   
//  2024-04-09   v2.0   clean up code 
//

#include "BluetoothSerial.h" //Serial Bluetooth Interface

BluetoothSerial SerialBT;

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
String outputString = "";     // a String to hold outgoing data

const int high = 1;
const int low = 0; 

char buttonresult;

int databyte; 
int statusbyte; 

const int GPIO_data_line[8] = {2,4,5,14,27,26,25,33};
const int GPIO_status_line[5] = {13,35,34,39,36};

int bit[8];
static int oldbit[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

int factor; 

// replace missing on-board LED by external yellow low current LED plus 470 Ohm resistor
const int test_led = 23;   

int printflag = 1; 

void setup() {

  
  // for serial communication
  Serial.begin(115200);   // default = standard 
    
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  SerialBT.begin("ESP32_parallelport"); //Bluetooth Name des ESP32

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
 
 
  if(printflag>0) Serial.println(" \n ESP32 is ready. Please connect via Bluetooth. \n ");
  
  // say hello, use GPIO23 with external yellow LED, replacing missing builtin led
  delay(1000);
  for(int m=1; m<=8; m++) {
    digitalWrite(test_led, high); 
    delay(300);
    digitalWrite(test_led, low);
    delay(300);
  }

}

void loop() {

   char buttonresult[20];

   while (SerialBT.available()) {
    // get the new byte:
    char inChar = (char)SerialBT.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  } // end while
  
  if (stringComplete) {
	 
	 if(printflag>1) Serial.print(" \n inputString = ");
	  if(printflag>1) Serial.print(inputString);
    if(printflag>1) Serial.print(" \n ");

    // get data from PC
     if(inputString.substring(0,1) == "D") {

       databyte = inputString.substring(1,4).toInt(); //  = position 1 to 3 
       // databyte = databyte - 100; 
       
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

       statusbyte = statusbyte + 10;  // guarantees 2 characters
       outputString = String(statusbyte);
       if(printflag>1) Serial.print(outputString);
       sprintf(buttonresult,"%i",statusbyte);
       SerialBT.write(buttonresult[0]);
       SerialBT.write(buttonresult[1]);
       SerialBT.write(10);   // LF
     }

     inputString = "";
     stringComplete = false;

   }

}

  
