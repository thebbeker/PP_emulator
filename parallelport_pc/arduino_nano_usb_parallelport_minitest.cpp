//
//  >>> arduino_nano_usb_parallelport_minitest.cpp <<<   Thomas Hebbeker 2024
//
#define USBPORT "/dev/ttyUSB0"   // this needs to be adapted

#include <iostream>
#include <unistd.h>  // for sleep
#include <boost/asio.hpp>   // for USB communication

using namespace std;

boost::asio::io_context io;         // for setting up USB communication
boost::asio::serial_port port(io);  // for setting up USB communication

#include "arduino_nano_usb_parallelport_out.cpp"   // make PP set data lines

int main()
{
  port.open(USBPORT);     // setting up USB communication
  port.set_option(boost::asio::serial_port_base::baud_rate(115200)); // speed  
  
  sleep(7);   // wait till arduino is ready after boot 

  parallelport_out(1);
  sleep(1);
  parallelport_out(0);
        
	port.close();     // for ending USB connection 
}
	