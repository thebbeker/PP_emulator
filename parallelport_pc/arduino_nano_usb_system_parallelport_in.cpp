int parallelport_in() { 

//
//  Function to address parallelport interface, reading status lines. 
//  Here: Arduino Nano via USB
//    using system calls to avoid boost::asio which is not available on all platforms
// 
//  2024-04-20   version 1.0   Thomas Hebbeker
//
//  requires arduino code = arduino_usb_parallelport.ino
//   
  
  int statusbits;
  int statusbyte;

  static bool first = true;
  const bool debug = false;

  string command; 
  
  const int pause_usec = 200000;   
  
  const char* usbport = ARDUINO_NANO_USBPORT; 
  string usb;
  usb = usbport;  
  
  ifstream stdout_file; 
  string filename = "stdout_file.txt"; 
  

  if(first) {
  	cout << endl << " >>> parallelport_in <<<  arduino_usb  (system)  version 1.0  2024-04-20   T.Hebbeker  " << endl; 
    first = false;
  }

  statusbyte = 0; 

   command = " cat " + usb + " > " + filename + " & ";
   system(command.c_str());
     usleep(pause_usec);   // not too fast....
   
   command = "echo S >" + usb; 
   system(command.c_str());
     usleep(pause_usec);   // not too fast....
   
 //    system(command.c_str());                // for the moment need to repeat, for reliability, not yet understood
  usleep(pause_usec);   // not too fast....

  // here read output file written by Arduino Nano =  stdout_file.txt 
  statusbits = 0; 
  stdout_file.open(filename.c_str()); 
  stdout_file >> statusbits;
  stdout_file.close();
  
  statusbyte = statusbits-10;   //  correct back from .ino offset
  if(statusbyte<0) statusbyte = 0;
        
  if(debug) cerr << "  >>> parallelport_in <<<  DEBUG:  with command " << command << " arduino gives = " 
                 << statusbyte << endl; 
  
  // kill cat at end of execution to clean up.
  system("pkill cat"); 
  
  return statusbyte; 
  
}
