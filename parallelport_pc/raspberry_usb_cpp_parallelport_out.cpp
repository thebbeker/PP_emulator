void parallelport_out(int data) {    //  byte = 0 .. 255 
  
//
//  Function to address parallelport interface, setting data lines. 
//  Here: raspberry Pico via USB
// 
//  2024-04-17   version 1.0   Thomas Hebbeker
//
//  requires raspberry code = raspberry_usb_parallelport.ino
//     
  
  static bool first = true;
  const bool debug = false;
  
  const int pause_microseconds = 100; 

  int data_copy = data;  // dont modify input to function

  string command;
  string data_string;

	
  if(first) {
    cout << endl << " >>> parallelport_out    raspberry pico usb  cpp  version 1.0  2024-04-17 <<<    data = " << data  << endl; 
    first = false;
  }
  
  if(debug) cout << " >>> parallelport_out <<<  DEBUG: data = " <<  data << endl; 
  
  if(data_copy < 0) data_copy = 0;
  if(data_copy > 255) data_copy = 255;
  
  data_string = to_string(data_copy); 
  command = "D" + data_string + "\n";
  if(debug) cout << " >>> parallelport_out <<<  DEBUG:  input data, command = " 
                       <<  data  << ",  " << command << endl; 
  boost::asio::write(port, boost::asio::buffer(command));
  usleep(pause_microseconds);   // not too fast....
  
}
