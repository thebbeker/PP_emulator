int parallelport_in() { 
  
  // 
  //  2024-03-04  v 1.0     Thomas Hebbeker
  //
  //
  //  receive 5 bits (status lines) from the parallel port 
  //    via bluetooth and ESP32 NodeMCU
  // 
  //  requires arduino IDE code = esp32_bluetooth_parallelport.ino
  //    to be uploaded to ESP32
  //   
  //
  //  BTPORT (eg /dev/frcomm0 ) must have been defined and assigned in the calling routine
  //  
  
  int statusbits;
  int statusbyte;
  
  static bool first = true;
  const bool debug = false;
  
  const int pause_usec = 200000;   
  
  string digit;
  string command; 
  ifstream stdout_file; 
  string filename = "stdout_file.txt"; 
  string port = ESP32_BLUETOOTH_BTPORT; 
  
  if(first) {
    cerr << " >>> parallelport_in <<<  esp32_bluetooth   2024-04-04   T.Hebbeker " << endl; 
    first = false;
  }
  
  statusbyte = 0; 
  
  command = " cat " + port + " > " + filename + " & ";
  system(command.c_str());
  usleep(pause_usec);   
  
  command = " echo 'S' > " + port;
  system(command.c_str());    
  usleep(pause_usec);   // not too fast....
  
  system(command.c_str());                // for the moment need to repeat, for reliability, not yet understood
  usleep(pause_usec);   // not too fast....
  
  // here read output file written by ESP32 =  stdout_file.txt 
  statusbits = 0; 
  stdout_file.open(filename.c_str()); 
  stdout_file >> statusbits;
  stdout_file.close();
  
  statusbyte = statusbits-10;   //  correct back from .ino offset
  
  if(debug) cerr << " for global command " << command << " esp32 gives = " << statusbyte << endl; 
  
  // kill cat at end of execution to clean up.
  system("pkill cat"); 
  
  return statusbyte; 
  
}
