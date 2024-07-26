//
// iowarrior_usb_parallelport_test.c
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h> 
#include <math.h>

#include "iowkit.h"

#define FALSE 0
#define TRUE 1

// does not exist in C
// using namespace std;

IOWKIT_HANDLE iows[IOWKIT_MAX_DEVICES];


// Write simple value
BOOLEAN WriteSimple(IOWKIT_HANDLE devHandle, DWORD value)
{
  IOWKIT_REPORT rep;
  IOWKIT56_IO_REPORT rep56;
  IOWKIT28_IO_REPORT rep28;
  
  /*
    int pidhandle; 
    pidhandle = (IowKitGetProductId(devHandle));
    printf(" %d = pidhandle \n",pidhandle);
    //  returns 5380 for io warrior 28 
  */
  
  // Init report
  switch (IowKitGetProductId(devHandle))
  {
    // Write simple value to IOW40
    case IOWKIT_PRODUCT_ID_IOW40:
    memset(&rep, 0xff, sizeof(rep));
    rep.ReportID = 0;
    rep.Bytes[3] = (UCHAR)value;
    return IowKitWrite(devHandle, IOW_PIPE_IO_PINS, (PCHAR)&rep, IOWKIT40_IO_REPORT_SIZE) == IOWKIT40_IO_REPORT_SIZE;
    // Write simple value to IOW24
    case IOWKIT_PRODUCT_ID_IOW24:
    memset(&rep, 0xff, sizeof(rep));
    rep.ReportID = 0;
    rep.Bytes[0] = (UCHAR)value;
    return IowKitWrite(devHandle, IOW_PIPE_IO_PINS, (PCHAR)&rep, IOWKIT24_IO_REPORT_SIZE) == IOWKIT24_IO_REPORT_SIZE;
    // Write simple value to IOW56
    case IOWKIT_PRODUCT_ID_IOW56:
    memset(&rep56, 0xff, sizeof(rep56));
    rep56.ReportID = 0;
    rep56.Bytes[6] = (UCHAR)value;
    return IowKitWrite(devHandle, IOW_PIPE_IO_PINS, (PCHAR)&rep56, IOWKIT56_IO_REPORT_SIZE) == IOWKIT56_IO_REPORT_SIZE;
    case IOWKIT_PRODUCT_ID_IOW28:
    memset(&rep28, 0xff, IOWKIT28_IO_REPORT_SIZE);
    rep28.ReportID = 0;
    rep28.Bytes[0] = (UCHAR)(value/(256*256)) & 0xFF;   // port 0
    rep28.Bytes[1] = (UCHAR)(value/256) & 0xFF;   // port 1
    // rep28.Bytes[2] = (UCHAR)value;  
    rep28.Bytes[2] = (UCHAR)value & 0x03;   // original    port 2
    
    // printf(" %lu  = value   \n", value);
    // printf(" %d  = byte 0   \n",(UCHAR)(value/(256*256)) & 0xFF);
    // printf(" %d  = byte 1   \n",(UCHAR)(value/256) & 0xFF);
    // printf(" %d  = byte 2   \n",(UCHAR)value & 0x03); 
    
    return IowKitWrite(devHandle, IOW_PIPE_IO_PINS, (PCHAR)&rep28, IOWKIT28_IO_REPORT_SIZE) == IOWKIT28_IO_REPORT_SIZE;
    case IOWKIT_PRODUCT_ID_IOW100:
    return 0;
    default:
    return FALSE;
  }
}

ULONG ReadSimple(IOWKIT_HANDLE devHandle, IOWKIT56_IO_REPORT *rep56)
{
  memset(rep56, 0x00, IOWKIT56_IO_REPORT_SIZE);
  
  switch (IowKitGetProductId(devHandle))
  {
    case IOWKIT_PRODUCT_ID_IOW40:
    return IowKitRead(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT40_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW24:
    return IowKitRead(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT24_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW56:
    return IowKitRead(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT56_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW28:
    return IowKitRead(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT28_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW100:
    return IowKitRead(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT100_IO_REPORT_SIZE);
    default:
    return 0;
  }
}

ULONG ReadSimpleNonBlocking(IOWKIT_HANDLE devHandle, IOWKIT56_IO_REPORT *rep56)
{
  memset(rep56, 0x00, IOWKIT56_IO_REPORT_SIZE);
  
  //IOWKIT28_IO_REPORT r28;
  //memset(&r28, 0x00, IOWKIT28_IO_REPORT_SIZE);
  
  switch (IowKitGetProductId(devHandle))
  {
    case IOWKIT_PRODUCT_ID_IOW40:
    return IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT40_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW24:
    return IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT24_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW56:
    return IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT56_IO_REPORT_SIZE);
    case IOWKIT_PRODUCT_ID_IOW28:
    return IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT28_IO_REPORT_SIZE);;
    case IOWKIT_PRODUCT_ID_IOW100:
    return IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, (PCHAR)rep56, IOWKIT100_IO_REPORT_SIZE);
    default:
    return 0;
  }
}

void drainBuffers(IOWKIT_HANDLE devHandle)
{
  char dummy[64];
  int pid = 0;
  int readsize = 0;
  
  pid = (IowKitGetProductId(devHandle));
  switch (pid)
  {
    case IOWKIT_PRODUCT_ID_IOW40:
    readsize = IOWKIT40_IO_REPORT_SIZE;
    break;
    case IOWKIT_PRODUCT_ID_IOW24:
    readsize = IOWKIT24_IO_REPORT_SIZE;
    break;
    case IOWKIT_PRODUCT_ID_IOW56:
    case IOWKIT_PRODUCT_ID_IOW28:
    case IOWKIT_PRODUCT_ID_IOW100:
    readsize = IOWKIT56_IO_REPORT_SIZE;
    break;
  }
  while (IowKitReadNonBlocking(devHandle, IOW_PIPE_IO_PINS, dummy, readsize))
  ;
}

// void parallelport_out(int input_byte) { }
#include "iowarrior_usb_parallelport_out.c"

// int parallelport_in(i) { }
#include "iowarrior_usb_parallelport_in.c"



void led_on(int morsecode){
  int morsecode_copy = morsecode; 
  long int dit = 200000; // microseconds,  thus 200 milliseconds 
  int signal[5]; // at most 5 elements per character
  int i = 0;
  while (morsecode_copy > 0) { // decode backwards
    signal[i] = morsecode_copy % 10;       // bit shift possibly more elegant
    morsecode_copy = morsecode_copy/10; 
    i++;
  }
  for (int k = i-1; k>=0; k--) {
    if(signal[k] < 4) {
      parallelport_out(1);    // first data line  LED on
      usleep(signal[k]*dit); 
      parallelport_out(0);    // first data line  LED off
      usleep(dit); 
      } else {
      usleep(signal[k]*dit);   // LED stays off - pause!
    }
  }
  usleep(3*dit); 
}

void sendmorse(char character) {   // write in c for compatibility 
  // %%% https://www.geeksforgeeks.org/morse-code-implementation/
  printf("  >>> sendmorse  <<<   character =  %c \n", character);
  switch(character) {    // here we put only those characters we need for Hello, World...
    case 'H': led_on(1111);
    case 'e': led_on(1);
    case 'l': led_on(1311);
    case ' ': led_on(4);   //  pause 4 dit plus 3 dit automatically added after each character
  }
}

int main(int argc, char *argv[])
{
  
  // int status;
  
  int number = 0;
  
  int status; 
  
  // does not exist ist c!
  //  string message;
  
  /*     
    int ACK;
    int BUSY;
    int PAPER;
    int ERROR;
    int SELECT;
  */
  
  //  2024-05-01  add speed test for status lines 
  //              correct speed test for data lines, using parallelport_out
  
  // cout does not exist in C
  printf("\n >>> iowarrior_usb_parallelport_test.cpp <<<  V 2.0   T.H. 2024-05-01 \n\n"); 
  
  printf(" this program uses iowarrior 28 board to address parallel port via /dev/usb/iowarrorN \n\n");   
  
  // general initialization copied from arduino usb test
  
  
  
  // IOWKIT_HANDLE iows[IOWKIT_MAX_DEVICES];
  int i, j;
  ULONG bits;
  int numIows;
  unsigned short sn[9];  // serial number
  char snt[9];
  ULONG rc;
  DWORD pid;
  IOWKIT_HANDLE devHandle;
  IOWKIT56_IO_REPORT rep56;   //  ???
  // IOWKIT28_IO_REPORT rep28;
  
  // int pidhandle; 
  // pidhandle = IowKitGetProductId(devHandle);
  
  // Open device
  devHandle = IowKitOpenDevice();
  if (devHandle == NULL)
  {
    printf("Failed to open device\n");
    goto out;
  }
  
  // Get version of SDK/API/DLL
  printf("API: %s\n", IowKitVersion());
  // Get number of IOWs in system
  numIows = IowKitGetNumDevs();
  printf("%d = IOWs in system = numIOWS \n", numIows);
  printf(" %d = IOWKIT_PID_IOW28 \n",IOWKIT_PID_IOW28);
  // printf(" %d = pidhandle \n",pidhandle);
  printf(" %d = IOWKIT_PRODUCT_ID_IOW28 \n",IOWKIT_PRODUCT_ID_IOW28);
  printf(" %d = IOWKIT_PRODUCT_ID_IOW56 \n",IOWKIT_PRODUCT_ID_IOW56);
  printf(" %d = IOWKIT_PRODUCT_ID_IOW100 \n",IOWKIT_PRODUCT_ID_IOW100);
  printf(" %lu = IOWKIT28_IO_REPORT_SIZE \n",IOWKIT28_IO_REPORT_SIZE);
  printf(" %d = IOW_PIPE_IO_PINS \n",IOW_PIPE_IO_PINS);
  
  
  
  // Get all IOW handles
  for (i = 0; i < numIows; i++)
  {
    // Get device handle and init object
    iows[i] = IowKitGetDeviceHandle(i + 1);
    // Get serial number
    IowKitGetSerialNumber(iows[i], sn);
    pid = IowKitGetProductId(iows[i]);
    printf(" %d  %p  %lu = i, iows[i], pid \n", i, iows[i], pid); 
    
    for (j = 0; j < 9; j++)
    snt[j] = sn[j];
    
    printf("%d PID %x, S/N \"%s\", Rev %lX\n", i + 1, (unsigned int)pid, snt, IowKitGetRevision(iows[i]));
    IowKitSetWriteTimeout(iows[i], 1000);
    IowKitSetTimeout(iows[i], 2000);
  }
  
  printf(" now indirect call to asio via parallelport_out \n"); 
  parallelport_out(255);
  usleep(7000000);
  parallelport_out(0);
  usleep(3000000);
  
  int dummy; 
  
  
  // test 0
  printf("\n  Test 0. We use output line 1 for sending Hello, World by Morse code \n"); 
  //  sleep(1); 
  printf("\n  Enter 1 (0/-1) if ready to start (test is skipped/program is ended)  \n");
  dummy = scanf("%d",&number); 
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    sleep(3); 
    sendmorse('H'); 
    sendmorse('e'); 
    sendmorse('l'); 
    sendmorse('l'); 
    sendmorse('o');
    sendmorse(','); 
    sendmorse(' '); 
    sendmorse('W');
    sendmorse('o');
    sendmorse('r');
    sendmorse('l'); 
    sendmorse('d'); 
    sleep(3); 
    printf("\n  Enter 1 (0) if this 1st test was successfull (failed)  \n");
    dummy = scanf("%d",&number);
    if(number==0) {
      printf(" Something wrong, need to be fixed. Terminate program.  \n"); 
      return(-1);
    }
  } 
  
  // test 1 
  printf("\n  Test 1. Now we turn the individual output lines on, one at a time, starting with lowest bit (red LED) \n"); 
  printf("   Then we turn again the individual output lines on, one at a time, now starting with highest bit (green LED) \n");
  // sleep(2); 
  printf("\n  Enter 1 (0/-1) if ready to start (test is skipped/program is ended)  \n");
  dummy = scanf("%d",&number); 
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    int data;
    int factor = 1; 
    for(int n=1; n<=8; n++) {
      data = factor;
      parallelport_out(data);
      factor = factor*2;
      sleep(1); 
    }
    parallelport_out(0);
    factor = 128; 
    for(int n=8; n>=1; n--) {
      data = factor;
      parallelport_out(data);
      factor = factor/2;
      sleep(1); 
    }
    parallelport_out(0);
    printf("\n  Enter 1 (0) if this 1st test was successfull (failed)  \n");
    dummy = scanf("%d",&number);
    if(number==0) {
      printf(" Something wrong, need to be fixed. Terminate program.  \n"); 
      return(-1);
    }
  }
  
  
  
  
  //  return;
  
  
  /*  
    
    printf("Blinking ALL LEDs...\n");
    //srand(time(NULL));
    // Blinking'
    for (i = 0; i < 2; i++)
    {
    
    printf("  i = %i \n", i); 
    
    //bits = 0x00000000;
    // Make every IOW blink
    // in the following assume numIows = 1   thus no loop needed
    // for (j = 0; j < numIows; j++)
    // {
    int j = 0;
    //  printf("  j = %i \n", j); 
    // bits = rand();
    printf("switch builtin LED oFF  / external LED on ! \n");
    //  bits = 0xFFFFFFFF;   // = original 
    // bits = 0x00000001;   // = test    = port P2.0
    // bits = 0x00000002;   // = test    = port P2.1
    // bits = 0x00000100;   // = test    = port P1.0
    bits = 0x00020000;   // = test    = port P0.1
    // bits = 0xFF00FFFF;   // this does NOT disable port no 2 
    // bits = 0xFFFF00FF;   // this does NOT disable port no 2 
    // bits = 0xFFFF0000;   // this DOES disable port no 2 
    // bits = 0xFFFFFF00;   // this DOES disable port no 2    NOT consistent with IOW28_Datasheet.pdf
    // Write to simple endpoint
    rc = WriteSimple(iows[j], bits);
    // usleep(20000);
    usleep(2900000);
    printf("switch builtin LED on /  external LED off ! \n");
    printf(" %lu  = bits \n",bits);
    bits = 0;
    // Write to simple endpoint
    rc = WriteSimple(iows[j], bits);
    usleep(2900000);
    //                        ReadSimpleNonBlocking(iows[j], &rep56);
    //  } // for j 
    // Sleep for 25ms
    usleep(25000);
    }
    
    printf("Blinking complete\n");
  */
  
  // Set LEDs off
  for (i = 0; i < numIows; i++)
  {
    // Write to simple endpoint   - enable input bits 
    WriteSimple(iows[i], 0xFFFF00FF);
    usleep(20000);
    //ReadSimpleNonBlocking(iows[i], &rep56);
  }
  
  
  //Clear the buffers for reading the button
  for (i = 0; i < numIows; i++)
  {
    drainBuffers(iows[i]);
  }
  
  // test 2 
  
  int ACK;
  int BUSY;
  int PAPER;
  int ERROR;
  int SELECT;
  
  //  int status; 
  
  printf(" \n Test 2. Now we play with the status lines. \n ");
  printf("   Enter 1 (0/-1) if ready to start (test is skipped/program is ended) \n");
  dummy = scanf("%d",&number);
  if(number==-1) {
    return(0);
  }
  if(number==1) {
    test2:
    printf(" Next we read the status lines - while you press some of the buttons at the same time! \n ");
    printf(" Please press those buttons simultaneously ! NOW! (you got five seconds) \n");
    sleep(5); 
    
    status = parallelport_in(); 
    //  parallelport_out(status); 
    
    printf(" DEBUG status = %d \n",status); 
    
    ACK = status & 0b00001; 
    BUSY = (status & 0b00010)/2; 
    PAPER = (status & 0b00100)/4; 
    SELECT =(status & 0b01000)/8; 
    ERROR =(status & 0b10000)/16; 
    printf(" ACK, BUSY, PAPER, SELECT, ERROR = %d %d %d %d %d \n", ACK, BUSY, PAPER, SELECT, ERROR);
    printf(" Enter 8 (2,-1) if the complete 2nd test was successfull ! (to be repeated, terminate program) \n");
    dummy = scanf("%d",&number);
    if(number==2) goto test2;
    if(number==-1) {
      printf(" Something wrong, need to be fixed. Terminate program. \n"); 
      return(-2);
    }
  }
  
  
  // can we be sure that all commands are executed ?
  WriteSimple(iows[i], 0);
  int nspeed = 10000; 
  printf(" \n Prepare for data line Speed Test - use your stopwatch! nspeed = %d \n",nspeed);
  usleep(3000000); 
  printf(" START  data lines \n");
  
  for (i = 0; i < nspeed; i++)
  {
    parallelport_out(i%255);    // much less fast than direct WriteSimple call !  measure 100 sec for 100000  - why so slow???
    // WriteSimple(0, 0xFFFFFFFF);
    // usleep(1); 
    // WriteSimple(0, 0x00000000);
  }
  printf(" STOP  data lines \n");
  
  
  // DWORD immediatebits;
  // BOOL  immediateresult;
  
  // Set LEDs off
  for (i = 0; i < numIows; i++)
  {
    // Write to simple endpoint   - enable input bits 
    WriteSimple(iows[i], 0xFFFF00FF);
    usleep(20000);
    //ReadSimpleNonBlocking(iows[i], &rep56);
  }
  
  
  
  //Clear the buffers for reading the button
  for (i = 0; i < numIows; i++)
  {
    drainBuffers(iows[i]);
  }
  
  
  // can we be sure that all commands are executed ?
  
  nspeed = 10000000;      // incredibly fast if no button is pressed:   for 10 million calls only 5 seconds
  printf(" \n Prepare for status line Speed Test - use your stopwatch! nspeed = %d \n",nspeed);
  usleep(3000000); 
  printf(" START  status lines \n");
  
  for (i = 0; i < nspeed; i++)
  {
    status = parallelport_in(); 
    if(status > 0) printf(" status = %d \n", status); 
  }
  printf(" STOP  status lines \n");
  
  
  // DWORD immediatebits;
  // BOOL  immediateresult;
  
  
  printf("Now push the button on the Starterkit (waits for 20 seonds)\n");
  printf("PID %lx, Rev %lx\n", IowKitGetProductId(iows[0]), IowKitGetRevision(iows[0]));
  
  // set all GPIOs to 1 - that means can use them as inputs!
  WriteSimple(0, 0xFFFF00FF);
  
  
  // trial: 
  // immediateresult = IowKitReadImmediate(iows[0], &immediatebits);
  // printf("  Immediate  %d %d %lu \n",i,immediateresult,immediatebits);   // seems result is always false and 0 
  // if(immediateresult) printf("  Heureka - immediate = true \n"); 
  
  usleep(3000000); 
  
  // int statusbit;
  int statusvalues; 
  
  // we assume that at beginning all status inputs = 0 = GND:
  statusvalues = 0; 
  
  for (i = 0; i < 100; i++)
  {
    
    //  statusvalues = 0;    NO!   since iowarrior reacts only when something is changing, keep old value!
    
    /* 
      rc = ReadSimpleNonBlocking(iows[0], &rep56);   
      // rc = ReadSimple(iows[0], &rep56);    //   waits for input - blocks execution until input changes! No good! 
      // probably need IowKitReadImmediate ! 
      printf(" after readsimple:  %d  %lu  %02x %d   debug \n", i, rc, rep56.Bytes[0], rep56.Bytes[0]);
      if (rc)
      {
      printf(" action found: ReadNonBlocking(): rc=%u ", (int)rc);
      
      for (j = 0; j < (int)rc - 1; j++)
      printf("%02x %d    ", rep56.Bytes[j], rep56.Bytes[j]);  // end for 
      
      // here we need only the first Byte:
      statusvalues = rep56.Bytes[0];
      // and we filter out only bits numbers 2 - 6
      statusvalues = statusvalues & 0x7C; 
      // and we shift back to bits 0 - 4:
      statusvalues = statusvalues/4; 
      
      printf("\n");
      }
      
      printf("  statusvalues read directly    =     %d   \n ", statusvalues);  
    */
    
    statusvalues = parallelport_in(); 
    printf(" statusvalues from parallelport_in = %d  \n", statusvalues); 
    usleep(200000);   //  0.2 sec   (* 100 = 20 sec !)
  }
  
  // Close device
  IowKitCloseDevice(devHandle);
  out:
  return 0;
}
