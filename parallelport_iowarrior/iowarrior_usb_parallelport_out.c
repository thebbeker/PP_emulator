
void parallelport_out(int data) {    //  byte = 0 .. 255 
  
  // 
  //  2024-03-18   version 1.0   Thomas Hebbeker
  //
  //
  //       addresses only port 1 of io warrior
  //
  
  // ULONG rc;
  DWORD value;
  
  
  static bool first = true;
  const bool debug = false;
  
  int data_copy = data; 
  
  // the following makes no difference for speed...
  if(data_copy > 255) data_copy = 255;
  if(data_copy < 0) data_copy = 0;
  
  
  // value = data_copy * 256; 
  value = data_copy << 8;  
  
  
  if(first) {
    printf(" >>> parallelport_out    iowarrior_usb   version 1.1  2024-03-03 <<<    data = %d \n \n", data);
    first = false;
  }
  
  if(debug) printf(" >>> parallelport_out <<<  DEBUG: data = %d \n ", data_copy); 
  
  // set iowarrior output bits    port 1!
  // value = data_copy * 256;  
  // rc = WriteSimple(iows[0], value);
  WriteSimple(iows[0], value);
  
  return; 
  
}
