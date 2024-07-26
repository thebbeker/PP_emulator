
void parallelport_out(int data) {    //  byte = 0 .. 255 
  
// 
//  2024-03-24   version 1.0   Thomas Hebbeker
//  2024-04-16   version 2.0   cleanup 
//
//   note: here cerr instead of cout is used for compatibility with parallelport_in (for raspberry) 
//
  
  static bool first = true;
  const bool debug = false;
  int bit[8];
  static int oldbit[8];
  int k;
  int data_copy = data;  // dont modify input to function
  // static string digit[8];
  string digit;
  string command;
  
  const char* usbport = RASPBERRY_USBPORT; 
  string usb;
  usb = usbport;
  
  static int gpio[8] = {2,3,6,7,10,11,14,15};
    
  
  static ofstream file_on[8];
  
  static string filename_on[8];
    
  static ofstream file_off[8];
  
  static string filename_off[8];
  
  if(first) {      // prepare 16 files to be sent to raspberrry pico micropython 
    cerr << endl << " >>> parallelport_out <<<    raspberry_usb  ampy  version 2.0  2024-04-16   data = " << data  << endl; 
    for(int i=0; i<8; i++) {
       oldbit[i] = -1;    // undefined at this stage !
       digit = to_string(i); 
       filename_on[i] = "file_on_" + digit + ".py";
       filename_off[i] = "file_off_" + digit + ".py";
       digit = to_string(gpio[i]); 
       file_on[i].open(filename_on[i],ios::out);  
       file_on[i] << "from machine import Pin" << endl;
       file_on[i] << "gpiopin = Pin("+ digit + ",Pin.OUT)" << endl; 
       file_on[i] << "gpiopin.value(1)" << endl; 
       file_on[i].close();
       file_off[i].open(filename_off[i],ios::out);  
       file_off[i] << "from machine import Pin" << endl;
       file_off[i] << "gpiopin = Pin("+ digit + ",Pin.OUT)" << endl; 
       file_off[i] << "gpiopin.value(0)" << endl; 
       file_off[i].close();
    }
    first = false;
  }
  
  
  if(data_copy < 0) data_copy = 0;
  if(data_copy > 255) data_copy = 255;
  
// decompose data into 8 bits
  for(int i = 0; i < 8; i++) {
     bit[i] = ((data_copy >> i) & 0x01);
  }

  
  // set raspberry output bits

  for(int i=0; i<8; i++) {
    if(bit[i]!=oldbit[i]) {
    
    if(bit[i]==1) command = " ampy --port " + usb + " run " + filename_on[i];
      if(bit[i]==0) command = " ampy --port " + usb + " run " + filename_off[i];
    
      if(debug) cerr << " >>> parallelport_out <<<  DEBUG:  standard mode   i, bit, command = " 
                     <<  i << "  " << bit[i] << "  " << command << endl;    
      

      system(command.c_str()); 
      
      oldbit[i]=bit[i]; 
    }
  }

}
