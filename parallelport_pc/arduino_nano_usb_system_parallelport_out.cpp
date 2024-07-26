void parallelport_out(int data) {    //  byte = 0 .. 255 
  
//
//  Function to address parallelport interface, setting data lines. 
//  Here: Arduino Nano via USB
//   using system calls to avoid boost::asio which is not available on all platforms
// 
//  2024-04-20   version 1.0   Thomas Hebbeker
//
//  requires arduino code = arduino_usb_parallelport.ino
//     
  
  static bool first = true;
  const bool debug = false;
  
  int data_copy = data;  // dont modify input to function

  string command;
  string data_string;
  
  const char* usbport = ARDUINO_NANO_USBPORT; 
  string usb;
  usb = usbport;  
	
  if(first) {
    cout << endl << " >>> parallelport_out    arduino usb (system)  version 1.0  2024-04-20 <<<    data = " << data  << endl; 
    first = false;
  }
  
  if(debug) cout << " >>> parallelport_out <<<  DEBUG: data = " <<  data << endl; 
  
  if(data_copy < 0) data_copy = 0;
  if(data_copy > 255) data_copy = 255;
  
  data_string = to_string(data_copy); 
  command = "echo D" + data_string + " >" + usb; 
  if(debug) cout << " >>> parallelport_out <<<  DEBUG:  input data, command = " 
                 <<  data  << ",  " << command << endl; 
  system(command.c_str());
  
}
