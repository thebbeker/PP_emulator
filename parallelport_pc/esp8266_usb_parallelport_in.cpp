int parallelport_in() { 

//
//  Function to address parallelport interface, reading status lines. 
//  Here: ESP8266 NodeMCU via USB
// 
//  2024-04-04   version 1.0   Thomas Hebbeker
//  2024-04-15           2.0   cleanup
//
//  requires esp8266 code = esp8266_usb_parallelport.ino
//   
  
  int statusbyte;

  static bool first = true;
  const bool debug = false;
  
  const int pause_microseconds = 1000; 

  string digit;
  string command; 
  char c_from_esp8266;
   
   int k; 
   
   if(first) {
    cout << endl << " >>> parallelport_in <<<  esp8266_usb   version 2.0  2024-04-15   T.Hebbeker  " << endl; 
    first = false;
  }

  statusbyte = 0; 

   command = "S\n"; 
   boost::asio::write(port, boost::asio::buffer(command));
  
   usleep(pause_microseconds);   // not too fast....    
   
   boost::asio::read(port, boost::asio::buffer(&c_from_esp8266,2));   //  2 characters 
    
   usleep(pause_microseconds);   // not too fast....    

   statusbyte = stoi(&c_from_esp8266) - 10; 
    
   if(debug) cout << "  >>> parallelport_in <<<  DEBUG:  with command " << command.substr(0,1) << " esp8266 gives = " 
        << &c_from_esp8266 << " = " << statusbyte << endl; 

   return statusbyte; 
  
}
