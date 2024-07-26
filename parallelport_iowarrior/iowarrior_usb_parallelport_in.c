
int parallelport_in() {    
  
  // 
  //  2024-04-23   version 1.0   Thomas Hebbeker
  //  2024-05-01   version 2.0   longer loop, with break 
  //
  //
  //      read parallel port status bits via io warrior
  //
  //  possibly can use drainBuffers(..) to erase history of button pressing, need less imax for loop
  //
  
  ULONG rc;
  IOWKIT56_IO_REPORT rep56;   
  
  static bool first = true;
  const bool debug = false;
  
  static int statusbyte;    // static since iowarrior reacts only to changes !
  
  int imax = 50;
  
  if(first) {
    printf(" >>> parallelport_in    iowarrior_usb   version 2.0  2024-05-01 <<<  \n \n");
    statusbyte = 0; 
    first = false;
  }
  
    
  //
  // this belongs in calling main program
  // 
  // set all relevant GPIOs to 1 - that means can use them as inputs!
  //  
  // WriteSimple(iows[0], 0xFFFFFFFF);
  // WriteSimple(iows[0], 0xFF000000);
  
  
  //  if(debug) printf(" >>> parallelport_in <<<   DEBUG  \n "); 
  
  for (int i = 1; i <= imax; i++)   // repeat to catch all status line changes - up to imax>=5 since there are 5 lines
  //  but there could have been more changes in the mean time (back and forth) so better use bigger imax...
  //  note:  switching lines several times back and force in between two calls 
  //         parallelport_in is not allowed  (or there must be a "clean" command somewhere)
  {
    
    rc = ReadSimpleNonBlocking(iows[0], &rep56);   
    if (debug) printf(" >>> parallelport_in <<<   DEBUG:  after ReadSimple  %lu  %02x %d  \n", rc, rep56.Bytes[0], rep56.Bytes[0]);
    if (rc)
    {
      // here we need only the first Byte:
      statusbyte = rep56.Bytes[0];
      // and we filter out only bits numbers 2 - 6
      statusbyte = statusbyte & 0x7C; 
      // and we shift back to bits 0 - 4:
      statusbyte = statusbyte/4; 
      if (debug) printf(" >>> parallelport_in <<<   DEBUG:  after success   %lu  %02x %d  statusbyte =  %d \n", rc, rep56.Bytes[0], rep56.Bytes[0], statusbyte);
    }
    else {      //  if (rc==0) 
      break;
    }
    
    usleep(20000);  
  }
  
  return statusbyte; 
  
}
