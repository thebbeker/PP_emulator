int parallelport_in() { 

//
//  Function to address parallelport interface, reading status lines. 
//  Here: Arduino Nano via USB
// 
//  2024-04-04   version 1.0   Thomas Hebbeker
//  2024-04-08           2.0   cleanup
//
//  requires arduino code = arduino_usb_parallelport.ino
//   
  
  int statusbyte;

  static bool first = true;
  const bool debug = false;
  
  const int pause_microseconds = 1000; 

  string command; 
  char c_from_arduino;

  if(first) {
  	cout << endl << " >>> parallelport_in <<<  arduino_usb   version 2.0  2024-04-08   T.Hebbeker  " << endl; 
    first = false;
  }

  statusbyte = 0; 

   command = "S\n"; 
   boost::asio::write(port, boost::asio::buffer(command));
  
   usleep(pause_microseconds);   // not too fast....    
   
   boost::asio::read(port, boost::asio::buffer(&c_from_arduino,2));   //  2 characters 
    
   usleep(pause_microseconds);   // not too fast....    

   statusbyte = stoi(&c_from_arduino) - 10; 
    
   if(debug) cout << "  >>> parallelport_in <<<  DEBUG:  with command " << command.substr(0,1) << " arduino gives = " 
        << &c_from_arduino << " = " << statusbyte << endl; 

   return statusbyte; 
	
}
