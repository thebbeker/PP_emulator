//
//  >>> parallelport_test.cpp <<<
//
//
//  this program accesses parallel port from PC 
//  it assumes an appropriate PP emulator
//
//
// install with 
//    g++ -o parallelport_test.exe  parallelport_test.cpp -lcurl
//  
//    eventually need 
//      -L /usr/lib/  -lcurl
//      -L /usr/lib/ -lboost_system -lboost_thread -lpthread
//
//
// V 1.0   T.H. 2024-03-25
// V 1.1   T.H. 2024-04-08
// V 1.2   T.H. 2024-04-14  complete set of 9 options to select from 
// V 2.0   T.H. 2024-04-22  simplify, straighten code
// V 3.0   T.H. 2024-04-27  simplify, add benchmark reading status lines, add mpremote option
// V 4.0   T.H. 2024-05-11  add morse test "Hello"
//  
//
//   note: here cerr instead of cout is used for compatibility with parallelport_in (for raspberry) 
//
//
//


// define PP emulator to be used:
// #define ARDUINO_NANO_USB  
// #define ARDUINO_NANO_USB_SYSTEM  
// #define ARDUINO_UNO_USB  
#define ARDUINO_ETHERNET
// #define ESP8266_USB      // not fully functional due to RX/TX lines used as output 
// #define ESP8266_WLAN
// #define ESP32_USB  
// #define ESP32_WLAN
// #define ESP32_BLUETOOTH   
// #define RASPBERRY_USB_AMPY  
// #define RASPBERRY_USB_MPREMOTE
// #define RASPBERRY_USB_CPP  
// #define IOWARRIOR_USB   // this does not work, since we need special SDK and C language


// for ARDUINO_NANO_USB and for ARDUINO_NANO_USB_SYSTEM: 
// #define ARDUINO_NANO_USBPORT "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0"   
#define ARDUINO_NANO_USBPORT "/dev/ttyUSB0" 
#define ARDUINO_NANO_USB_BAUDRATE 115200 

// for ARDUINO_UNO_USB : 
#define ARDUINO_UNO_USBPORT "/dev/ttyACM0" 
#define ARDUINO_UNO_USB_BAUDRATE 115200 

// for ARDUINO_ETHERNET :
#define ARDUINO_IPADDRESS "http://192.168.1.199"

// for ESP8266_USB : 
#define ESP8266_USBPORT "/dev/ttyUSB1" 
#define ESP8266_USB_BAUDRATE 115200  
// #define ESP8266_USB_BAUDRATE 230400  // too fast, not working

// for ESP8266_WLAN :
#define ESP8266_IPADDRESS "http://192.168.1.30/"

// for ESP32_USB : 
#define ESP32_USBPORT "/dev/ttyUSB1" 
#define ESP32_USB_BAUDRATE 115200 

// for ESP32_WLAN :
#define ESP32_IPADDRESS "http://192.168.1.7/"

// for ESP32_BLUETOOTH : 
#define ESP32_BLUETOOTH_BTPORT "/dev/rfcomm0"

// for RASPBERRY_USB_AMPY and for RASPBERRY_USB_CPP and ...: 
#define RASPBERRY_USBPORT "/dev/ttyACM0" 

// for RASPBERRY_USB_CPP: 
#define RASPBERRY_USB_BAUDRATE 115200 

#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <math.h>
#include <cstdlib>
#include <unistd.h>  // for sleep
#include <chrono>

using namespace std;
using namespace std::chrono;

#ifdef ARDUINO_NANO_USB 
  #include <boost/asio.hpp>
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef ARDUINO_UNO_USB 
  #include <boost/asio.hpp>
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef ESP8266_USB 
  #include <boost/asio.hpp>
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef ESP32_USB 
  #include <boost/asio.hpp>
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef RASPBERRY_USB_CPP 
  #include <boost/asio.hpp>
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

// void parallelport_out(int input_byte) { }
// int parallelport_in() { } 
#ifdef ARDUINO_NANO_USB 
  #include "arduino_nano_usb_parallelport_out.cpp"
  #include "arduino_nano_usb_parallelport_in.cpp"
#endif
#ifdef ARDUINO_NANO_USB_SYSTEM 
  #include "arduino_nano_usb_system_parallelport_out.cpp"
  #include "arduino_nano_usb_system_parallelport_in.cpp"
#endif
#ifdef ARDUINO_UNO_USB 
  #include "arduino_uno_usb_parallelport_out.cpp"
  #include "arduino_uno_usb_parallelport_in.cpp"
#endif
#ifdef ARDUINO_ETHERNET 
  #include "arduino_ethernet_parallelport_out.cpp"
  #include "arduino_ethernet_parallelport_in.cpp"
#endif
#ifdef ESP8266_USB 
  #include "esp8266_usb_parallelport_out.cpp"
  #include "esp8266_usb_parallelport_in.cpp"
#endif
#ifdef ESP8266_WLAN
  #include "esp8266_wlan_parallelport_out.cpp"
  #include "esp8266_wlan_parallelport_in.cpp"
#endif
#ifdef ESP32_USB 
  #include "esp32_usb_parallelport_out.cpp"
  #include "esp32_usb_parallelport_in.cpp"
#endif
#ifdef ESP32_WLAN
  #include "esp32_wlan_parallelport_out.cpp"
  #include "esp32_wlan_parallelport_in.cpp"
#endif
#ifdef ESP32_BLUETOOTH
  #include "esp32_bluetooth_parallelport_out.cpp"
  #include "esp32_bluetooth_parallelport_in.cpp"
#endif
#ifdef RASPBERRY_USB_AMPY
  #include "raspberry_usb_ampy_parallelport_out.cpp"
  #include "raspberry_usb_ampy_parallelport_in.cpp"
#endif
#ifdef RASPBERRY_USB_MPREMOTE
  #include "raspberry_usb_mpremote_parallelport_out.cpp"
  #include "raspberry_usb_mpremote_parallelport_in.cpp"
#endif
#ifdef RASPBERRY_USB_CPP
  #include "raspberry_usb_cpp_parallelport_out.cpp"
  #include "raspberry_usb_cpp_parallelport_in.cpp"
#endif


string pp_emulator_name; 


void led_on(int morsecode){
  int morsecode_copy = morsecode; 
  long int dit = 200000; // microseconds,  thus 200 milliseconds 
  int signal[5]; // at most 5 elements per character
  int i = 0;
  while (morsecode_copy > 0) { // decode backwards
    signal[i] = morsecode_copy % 10;       // bit shift possibly more elegant
    morsecode_copy = morsecode_copy/10; 
    i++;
  }
  for (int k = i-1; k>=0; k--) {
    // printf("  >>> led_on <<<  DEBUG:  i,k,signal[k] =  %d, %d, %d \n", i,k,signal[k]);
    if(signal[k] < 4) {
      parallelport_out(1);    // first data line  LED on
      usleep(signal[k]*dit); 
      parallelport_out(0);    // first data line  LED off
      usleep(dit); 
    } else {
      usleep(signal[k]*dit);   // LED stays off - pause!
    }
  }
  usleep(3*dit); 
}

void sendmorse(char character) {   // write in c for compatibility 
// %%% https://www.geeksforgeeks.org/morse-code-implementation/
  printf("  >>> sendmorse  <<<   character =  %c \n", character);
  switch(character) {    // here we put only those characters we need for Hello, World...
    case 'H': led_on(1111);  break;
    case 'E': led_on(1);  break;
    case 'L': led_on(1311);  break;
    case 'O': led_on(333);  break;  //  pause 4 dit plus 3 dit automatically added after each character
  }
}


int main()
{
  
  int status;
  int number = 0;
  int value; 
  string message;
  
  int ACK;
  int BUSY;
  int PAPER;
  int ERROR;
  int SELECT;
  
  cerr <<  endl << " >>> parallelport_test.cpp <<<  V 4.0   T.H. 2024-05-11" 
  << endl << endl;
  
  cerr <<  " this program uses PP emulator to address parallel port " << endl << endl;   
  
  int ntest_datalines = 100000;
  int ntest_statuslines = 1000;
  
  #ifdef ARDUINO_NANO_USB
    pp_emulator_name = "Arduino Nano USB";
    ntest_datalines = 100000;
	ntest_statuslines = 10000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << ARDUINO_NANO_USB_BAUDRATE << endl;  
	cerr <<  " USB port to be used " << ARDUINO_NANO_USBPORT << endl << endl;   
    port.open(ARDUINO_NANO_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ARDUINO_NANO_USB_BAUDRATE));
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  #ifdef ARDUINO_NANO_USB_SYSTEM
    pp_emulator_name = "Arduino Nano USB (via System calls) ";
    ntest_datalines = 10000;
	ntest_statuslines = 100;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << ARDUINO_NANO_USB_BAUDRATE << endl;  
    cerr <<  " USB port to be used " << ARDUINO_NANO_USBPORT << endl << endl;   
    cerr <<  " Make sure reset of board is prevented by connecting reset pin to GND " << endl; 
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;       
  #endif
  
  #ifdef ARDUINO_UNO_USB
    pp_emulator_name = "Arduino Uno USB";
    ntest_datalines = 100000;
	ntest_statuslines = 10000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << ARDUINO_UNO_USB_BAUDRATE << endl;  
	cerr <<  " USB port to be used " << ARDUINO_UNO_USBPORT << endl << endl;  
    port.open(ARDUINO_UNO_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ARDUINO_UNO_USB_BAUDRATE));
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
    cerr <<  " Note: some output lines are on during boot of board " << endl; 
    cerr <<  " IMPORTANT: data lines 1 (=2nd) and 2 (=3rd) not usable! " << endl
    << " (used for RX/TX and lack of GPIO pins!) " << endl;   
  #endif
  
  #ifdef ARDUINO_ETHERNET
    pp_emulator_name = "ARDUINO Uno with ETHERNET shield";
    ntest_datalines = 1000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl; 
    cerr <<  " IP address = " << ARDUINO_IPADDRESS << endl << endl;  
  #endif
  
  #ifdef ESP8266_USB
    pp_emulator_name = "NodeMCU ESP8266 USB";
    ntest_datalines = 100000;
    ntest_statuslines = 10000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << ESP8266_USB_BAUDRATE << endl;  
	cerr <<  " USB port to be used " << ESP8266_USBPORT << endl << endl;   
    port.open(ESP8266_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ESP8266_USB_BAUDRATE));
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  #ifdef ESP8266_WLAN
    pp_emulator_name = "NodeMCU ESP8266 WLAN";
    ntest_datalines = 1000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl; 
    cerr <<  " IP address = " << ESP8266_IPADDRESS << endl << endl;  
  #endif
  
  #ifdef ESP32_USB
    pp_emulator_name = "NodeMCU ESP32 USB";
    ntest_datalines = 50000;
	ntest_statuslines = 10000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << ESP32_USB_BAUDRATE << endl;  
	cerr <<  " USB port to be used " << ESP32_USBPORT << endl << endl;  
    port.open(ESP32_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ESP32_USB_BAUDRATE));
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  #ifdef ESP32_WLAN
    pp_emulator_name = "NodeMCU ESP32 WLAN";
    ntest_datalines = 1000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl; 
    cerr <<  " IP address = " << ESP32_IPADDRESS << endl << endl;  
  #endif
  
  #ifdef ESP32_BLUETOOTH
    pp_emulator_name = "NodeMCU ESP32 BLUETOOTH";
    ntest_datalines = 100;
    ntest_statuslines = 100;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Bluetooth port used " << ESP32_BLUETOOTH_BTPORT << endl << endl;   
  #endif
  
  #ifdef RASPBERRY_USB_AMPY
    pp_emulator_name = "RASPBERRY USB AMPY";
    ntest_datalines = 30;
    ntest_statuslines = 10;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " USB port to be used " << RASPBERRY_USBPORT << endl << endl;   
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  #ifdef RASPBERRY_USB_MPREMOTE
    pp_emulator_name = "RASPBERRY USB MPREMOTE";
    ntest_datalines = 100;
    ntest_statuslines = 20;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    // cerr <<  " USB port to be used " << RASPBERRY_USBPORT << endl << endl;   
    cerr <<  " USB port automatically selected " << endl << endl;   
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  #ifdef RASPBERRY_USB_CPP
    pp_emulator_name = "RASPBERRY USB CPP";
    ntest_datalines = 100000;
	ntest_statuslines = 10000;
    cerr <<  " PP emulator to be used = " << pp_emulator_name << endl;   
    cerr <<  " Baud rate set to " << RASPBERRY_USB_BAUDRATE << endl;  
	cerr <<  " USB port to be used " << RASPBERRY_USBPORT << endl << endl;   
    port.open(RASPBERRY_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(RASPBERRY_USB_BAUDRATE));
    cerr <<  " Note: please close Arduino IDE or other software accessing USB port " << endl;   
  #endif
  
  
  sleep(9);   // wait till PP emulator is ready after booting 
  
  cerr <<  endl << " Now first call to parallelport_out - to turn all outputs on " << endl; 
  parallelport_out(255);
  sleep(3);
  parallelport_out(0);
  sleep(3);
  
  
  // test 1 
  cerr <<  endl << " Test 1. Now we turn the individual output lines on, one at a time, starting with lowest bit " << endl; 
  cerr <<  "   Then we turn again the individual output lines on, one at a time, now starting with highest bit " << endl;
  cerr <<  endl << " Enter 1 (0/-1) if ready to start (test is skipped/program is ended) " << endl;
  cin >> number; 
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test1:
    for(int n=1; n<=8; n++) {
      parallelport_out(pow(2,n-1));
      usleep(500000); 
    }
    parallelport_out(0);
    for(int n=8; n>=1; n--) {
      parallelport_out(pow(2,n-1));
      usleep(500000); 
    }
    parallelport_out(0);
    
  }
  
  // test 2 
  cerr <<  endl << " Test 2. Now we read the status lines. " << endl;
  cerr <<  endl << " Enter 1 (0/2/-1) if ready to start (test is skipped/test 1 is repeated/program is ended) " << endl;
  cin >> number; 
  if(number==2) goto test1;
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test2:
    cerr <<  endl << " Please press some status line buttons (simultaneously) ! NOW! (you got five seconds)" << endl;
    sleep(5); 
    
    status = parallelport_in(); 
    #ifdef RASPBERRY_USB_AMPY
    //   sleep(2);
    #endif 
    parallelport_out(status); 
    
    cerr <<  " DEBUG status = " << status << endl; 
    
    ACK = status & 0b00001; 
    BUSY = (status & 0b00010)/2; 
    PAPER = (status & 0b00100)/4; 
    SELECT =(status & 0b01000)/8; 
    ERROR =(status & 0b10000)/16; 
    cerr <<  " ACK, BUSY, PAPER, SELECT, ERROR = " << ACK << " " << BUSY << " " << PAPER << " " << SELECT << " " << ERROR  << endl;
    
  }
  
  
  // test 3 
  // https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
  
  cerr <<  endl << " Test 3. Speed test for data lines. Measure execution time for " 
     << ntest_datalines << " output commands " << endl; 
  cerr <<  endl << " Enter 1 (0/2/-1) if ready to start (test is skipped/test 2 is repeated/program is ended) " << endl;
  cin >> number; 
  parallelport_out(0); 
  if(number==2) goto test2;
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test3: 
    auto start = high_resolution_clock::now();
    for(long n=1; n<=ntest_datalines; n++) {
      parallelport_out(n % 256);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cerr <<  " Duration measured for data lines = " << duration.count() << " seconds. " << endl;
    cerr <<  " Duration per cycle = " << (1.*duration.count())/ntest_datalines << " seconds.  = " 
    << (1.*duration.count())/ntest_datalines*1000 << " milli-seconds. "<< endl;
    parallelport_out(0);
  }
  
  
  // test 4 
  
  cerr <<  endl << " Test 4. Speed test for status lines. Measure execution time for " 
     << ntest_statuslines << " input commands " << endl; 
  parallelport_out(0); 
  cerr <<  endl << " Enter 1 (0/2/-1) if ready to start (test is skipped/test 3 is repeated/program is ended) " << endl;
  cin >> number; 
  if(number==2) goto test3;
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test4: 
    auto start = high_resolution_clock::now();
    for(long n=1; n<=ntest_statuslines; n++) {
      status = parallelport_in();
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cerr <<  " Duration measured for status lines = " << duration.count() << " seconds. " << endl;
    cerr <<  " Duration per cycle = " << (1.*duration.count())/ntest_statuslines << " seconds.  = " 
    << (1.*duration.count())/ntest_statuslines*1000 << " milli-seconds. "<< endl;
 //   return(-1);
  }
   
  // test 5 
  
  cerr <<  endl << " Test 5. Morse code 'Hello' example " << endl; 
  parallelport_out(0); 
  cerr <<  endl << " Enter 1 (0/2/-1) if ready to start (test is skipped/test 4 is repeated/program is ended) " << endl;
  cin >> number; 
  if(number==2) goto test4;
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test5: 
    parallelport_out(0);
    sleep(3); 
    sendmorse('H'); 
    sendmorse('E'); 
    sendmorse('L'); 
    sendmorse('L'); 
    sendmorse('O');
    sleep(3); 
    cerr <<  endl << " Enter 2 (0) if the 5th test should to be repeated (else terminate program) " << endl;
    cin >> number; 
    if(number==2) goto test5;
    cerr <<  " Terminate program. " << endl; 
 //   return(-1);
  }
  
  // switch all off
  parallelport_out(0);
  
  // curl_easy_cleanup(curl);
  
  
  // for RASPBERRY_USB : 
  #ifdef RASPBERRY_USB_AMPY
    // delete the .py auxiliary files 
    string command = "rm -f *.py";
    system(command.c_str()); 
  #endif
  
}

