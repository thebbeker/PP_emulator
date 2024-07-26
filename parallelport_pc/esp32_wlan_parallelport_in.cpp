int parallelport_in() { 
  
  //
  //  Function to address parallelport interface, reading status lines. 
  //  Here: ESP32 NodeMCU via wlan
  // 
  //  2024-04-04   version 1.0   Thomas Hebbeker
  //  2024-04-12           2.0   cleanup
  //
  //  requires esp32 code = esp32_wlan_parallelport.ino
  //   
  
  
  static bool first = true;
  const bool debug = false;
  
  int statusbyte; 
  
  const int pause_microseconds = 100; 
  
  string command; 
  
  const char* ipaddress = ESP32_IPADDRESS; 
  string ip;
  ip = ipaddress;   
  
  CURL *curl;
  CURLcode res;
  string readBuffer;
  
  
  if(first) {
    cout << endl << " >>> parallelport_in <<<  esp32_wlan   version 2.0  2024-04-12   T.Hebbeker  " << endl; 
    first = false;
  }
  
  curl = curl_easy_init();   // this should be moved to calling program, do it only once, not for each call to this routine, to save time! 
  //  no, less than 10% improvement
  
  statusbyte = 0; 
  
  if(curl) {
    
    command = ip + "S"; 
    curl_easy_setopt(curl, CURLOPT_URL,command.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);    // needed!  
    res = curl_easy_perform(curl);   //  needed!
    
    usleep(pause_microseconds);   // not too fast....    
    
    statusbyte = stoi(readBuffer.substr(17,2)) - 10; 
    
    if(debug) cout << "  >>> parallelport_in <<<  DEBUG:  with command " << command << " esp32 gives = " 
    << readBuffer << " (" << readBuffer.substr(17,2) << ") = " << statusbyte << endl; 
    
    curl_easy_cleanup(curl);
    
  }
  
  return statusbyte; 
  
}
