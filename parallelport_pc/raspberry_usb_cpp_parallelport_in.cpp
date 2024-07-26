int parallelport_in() { 

//
//  Function to address parallelport interface, reading status lines. 
//  Here: raspberry pico via USB
// 
//  2024-04-17   version 1.0   Thomas Hebbeker
//
//  requires raspberry code = raspberry_pico_usb_parallelport.ino
//  requires raspberry code = raspberry_pico_usb_parallelport.ino
//   
  
  int statusbyte;

  static bool first = true;
  const bool debug = false;
  
  const int pause_microseconds = 1000; 

  string command; 
  char c_from_raspberry;

  if(first) {
  	cout << endl << " >>> parallelport_in <<<  raspberry pico usb  cpp  version 1.0  2024-04-17   T.Hebbeker  " << endl; 
    first = false;
  }

  statusbyte = 0; 

   command = "S\n"; 
   boost::asio::write(port, boost::asio::buffer(command));
  
   usleep(pause_microseconds);   // not too fast....    
   
   boost::asio::read(port, boost::asio::buffer(&c_from_raspberry,2));   //  2 characters 
    
   usleep(pause_microseconds);   // not too fast....    

   statusbyte = stoi(&c_from_raspberry) - 10; 
    
   if(debug) cout << "  >>> parallelport_in <<<  DEBUG:  with command " << command.substr(0,1) << " raspberry gives = " 
        << &c_from_raspberry << " = " << statusbyte << endl; 

   return statusbyte; 
	
}
