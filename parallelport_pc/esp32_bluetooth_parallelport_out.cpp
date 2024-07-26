void parallelport_out(int data) {    //  byte = 0 .. 255 
    
// 
//  Function to address parallelport interface, setting data lines. 
//  Here: ESP32 NodeMCU via Bluetooth
//
//
//  requires arduino IDE code = esp32_bluetooth_parallelport.ino
//
//  2024-04-04   version 1.0   Thomas Hebbeker
//  2024-04-09   version 2.0   cleanup
// 
//  BTPORT (eg /dev/frcomm0 ) must have been defined and assigned in the calling routine
//  

  static bool first = true;
  const bool debug = false;
  
  const int pause_usec = 60000; 
  
  int data_copy = data;  // dont modify input to function
  
  string command;
  string data_string;
  string port = ESP32_BLUETOOTH_BTPORT; 
  
  if(first) {
    cout << endl << " >>> parallelport_out    esp32_bluetooth   version 2.0  2024-04-09 <<<    data = " << data  << endl; 
    first = false;
  }
  
  if(debug) cout << " >>> parallelport_out <<<  DEBUG: data = " <<  data << endl; 
      
  if(data_copy < 0) data_copy = 0;
  if(data_copy > 255) data_copy = 255;
  
  // set esp32 output bits
  
  // data_copy = data_copy + 100;  // make sure we have 3 digits

  data_string = to_string(data_copy); 
  command = " echo 'D" + data_string + "' > " + port;
  if(debug) cout << " >>> parallelport_out <<<  DEBUG:  input data, command = " 
                       <<  data  << ",  " << command << endl; 
  system(command.c_str());
  usleep(pause_usec); 
             
}
