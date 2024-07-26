//
//  >>> parallelport_basictest.cpp <<<
//
//
//  this program accesses parallel port from PC 
//  it assumes an appropriate PP emulator
//
//  here only one data line (0) and one status line (ACK) are tested. 
//
//  install with 
//    g++ -o parallelport_basictest.exe  parallelport_basictest.cpp -lcurl
//  
//    eventually you need 
//      -L /usr/lib/  -lcurl
//      -L /usr/lib/ -lboost_system -lboost_thread -lpthread
//
// V 1.0   Thomas Hebbeker 2024-04-08
//
//


// define PP emulator to be used:
// #define ARDUINO_USB  
// #define ESP8266_USB   
// #define ESP8266_WLAN
#define ESP32_USB   
// #define ESP32_WLAN
// #define ESP32_BLUETOOTH   
// #define RASPBERRY_USB   
// #define IOWARRIOR_USB   // this does not work, since we need special SDK and C language

// for ARDUINO_USB : 
// #define ARDUINO_USBPORT "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0"   
#define ARDUINO_USBPORT "/dev/ttyUSB0"  
#define ARDUINO_BAUDRATE 115200 

// for ESP8266_USB : 
#define ESP8266_USBPORT "/dev/ttyACM0" 
#define ESP8266_BAUDRATE 115200 

// for ESP32_USB : 
#define ESP32_USBPORT "/dev/ttyUSB0" 
#define ESP32_BAUDRATE 115200 

// for ESP32_WLAN :
#define ESP32_IPADDRESS "http://192.168.1.7/"

// for ESP32_BLUETOOTH : 
#define ESP32_BLUETOOTH_BTPORT "/dev/rfcomm0"

#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <math.h>
#include <cstdlib>
#include <unistd.h>  // for sleep
#include <boost/asio.hpp>
#include <chrono>

using namespace std;

#ifdef ARDUINO_USB 
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef ESP8266_USB 
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

#ifdef ESP32_USB 
  boost::asio::io_context io;
  boost::asio::serial_port port(io);
#endif

// void parallelport_out(int input_byte) { }
// int parallelport_in() { } 
#ifdef ARDUINO_USB 
  #include "arduino_usb_parallelport_out.cpp"
  #include "arduino_usb_parallelport_in.cpp"
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



string pp_emulator_name; 

int main()
{
  
  int status;
  
  int ACK;
  
  cout << endl << " >>> parallelport_basictest.cpp <<<  V 1.0   T.H. 2024-04-08" 
       << endl << endl;
  
  cout << " this program uses PP emulator to address parallel port " << endl << endl;    
  
  #ifdef ARDUINO_USB
    pp_emulator_name = "Arduino nano USB";
    cout << " PP emulator to be used = " << pp_emulator_name << endl;   
    cout << " Baud rate set to " << ARDUINO_BAUDRATE << endl;  
    port.open(ARDUINO_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ARDUINO_BAUDRATE));
    cout << " USB port used " << ARDUINO_USBPORT << endl << endl;   
    #endif
  
  #ifdef ESP8266_USB
    pp_emulator_name = "NodeMCU ESP8266 USB";
    cout << " PP emulator to be used = " << pp_emulator_name << endl;   
    cout << " Baud rate set to " << ESP8266_BAUDRATE << endl;  
    port.open(ESP8266_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ESP8266_BAUDRATE));
    cout << " USB port used " << ESP8266_USBPORT << endl << endl;   
    #endif
    
    #ifdef ESP32_USB
    pp_emulator_name = "NodeMCU ESP32 USB";
    cout << " PP emulator to be used = " << pp_emulator_name << endl;   
    cout << " Baud rate set to " << ESP32_BAUDRATE << endl;  
    port.open(ESP32_USBPORT);
    port.set_option(boost::asio::serial_port_base::baud_rate(ESP32_BAUDRATE));
    cout << " USB port used " << ESP32_USBPORT << endl << endl;   
  #endif
  
  #ifdef ESP32_WLAN
    pp_emulator_name = "NodeMCU ESP32 WLAN";
      int ntest = 1000;
    cout << " PP emulator to be used = " << pp_emulator_name << endl; 
    cout << " IP address = " << ESP32_IPADDRESS << endl << endl;  
  #endif
    
  #ifdef ESP32_BLUETOOTH
    pp_emulator_name = "NodeMCU ESP32 BLUETOOTH";
      int ntest = 1000;
    cout << " PP emulator to be used = " << pp_emulator_name << endl;   
    cout << " Bluetooth port used " << ESP32_BLUETOOTH_BTPORT << endl << endl;   
  #endif
  
    
  sleep(9);   // wait till PP emulator is ready after booting 
  
  cout << endl << " The following basic code tests first Parallel Port data line. " << endl
       << " The attached LED (plus series resistor to GND), will blink once. " << endl; 
  sleep(3);
  parallelport_out(1);
  sleep(3);
  parallelport_out(0);
  
  cout << " LED blink test completed " << endl; 
  sleep(2);
  
  cout << endl << " Next we check out the status line ACK. " << endl
       << " Press the button connected to this line to set it to high (you got 5 seconds). " << endl; 
  sleep(5);
  status = parallelport_in();
  ACK = status & 0b00001; 
  
  cout << " Status line ACK = " << ACK << endl; 
  
}
