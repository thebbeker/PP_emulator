
//
//  Function to address parallelport interface, setting data lines. 
//  Here: ESP8266 NodeMCU via wlan
// 
//  2024-02-28   version 1.0   Thomas Hebbeker
//  2024-04-12           2.0   cleanup
//
//  requires esp8266 code = esp8266_wlan_parallelport.ino
//     

#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static int fcall = 0;

void parallelport_out(int data) {    //  byte = 0 .. 255 
  
  static bool first = true;
  const bool debug = false;
  
  const int pause_microseconds = 100; 

  int data_copy = data;  // dont modify input to function

  string command;
  string data_string;
  
  CURL *curl;
  CURLcode res;
  string readBuffer;

  
  if(first) {
    cout << endl << " >>> parallelport_out    esp8266 wlan   version 2.0  2024-04-12 <<<    data = " << data  << endl; 
    first = false;
  }
  
  const char* ipaddress = ESP8266_IPADDRESS; 
  string ip;
  ip = ipaddress;   
  
  if(debug) cout << " >>> parallelport_out <<<  DEBUG: data = " <<  data << endl; 
  
  if(data_copy < 0) data_copy = 0;
  if(data_copy > 255) data_copy = 255;
  
  curl = curl_easy_init();  
  
  if(curl) {
    
    data_string = to_string(data_copy); 
    command = ip + "D" + data_string;
    if(debug) cout << " >>> parallelport_out <<<  DEBUG:  input data, command = " 
                       <<  data  << ",  " << command << endl; 
                       
    curl_easy_setopt(curl, CURLOPT_URL,command.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);    // needed!  
    res = curl_easy_perform(curl);  

    usleep(pause_microseconds);   // not too fast....
  
   curl_easy_cleanup(curl);
   
  }
  
}
