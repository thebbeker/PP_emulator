//
// iowarrior_usb_parallelport_minitest.c
//

// code inspired and partly copied from iowkittest.c from Code Mercenaries

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







int main(int argc, char *argv[])
{
  
  // int status;
  
  //  int number = 0;
  
  int status; 

  // cout does not exist in C
  printf("\n >>> iowarrior_usb_parallelport_minitest.cpp <<<  V 2.0   T.H. 2024-05-01 \n\n"); 
  
  printf(" this program uses iowarrior 28 board to address parallel port via /dev/usb/iowarrorN \n\n");   
  
  // general initialization copied from arduino usb test
  
  
  
  // IOWKIT_HANDLE iows[IOWKIT_MAX_DEVICES];
  int i;
  // înt j;
  // ULONG bits;
  int numIows;
  // unsigned short sn[9];  // serial number
  // char snt[9];
  // ULONG rc;
  // DWORD pid;
  IOWKIT_HANDLE devHandle;
  // IOWKIT56_IO_REPORT rep56;   //  ???
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
  // printf(" API: %s \n", IowKitVersion());
  // Get number of IOWs in system
  numIows = IowKitGetNumDevs();
  // printf(" IOWs in system = numIOWS = %d \n", numIows);
  // printf(" IOWKIT_PID_IOW28 = %d \n",IOWKIT_PID_IOW28);
  // printf(" %d = pidhandle \n",pidhandle);
  /* 
  printf(" %d = IOWKIT_PRODUCT_ID_IOW28 \n",IOWKIT_PRODUCT_ID_IOW28);
  printf(" %d = IOWKIT_PRODUCT_ID_IOW56 \n",IOWKIT_PRODUCT_ID_IOW56);
  printf(" %d = IOWKIT_PRODUCT_ID_IOW100 \n",IOWKIT_PRODUCT_ID_IOW100);
  printf(" %lu = IOWKIT28_IO_REPORT_SIZE \n",IOWKIT28_IO_REPORT_SIZE);
  printf(" %d = IOW_PIPE_IO_PINS \n",IOW_PIPE_IO_PINS);
  */ 
  
  
  
  
  // Get all IOW handles
  for (i = 0; i < numIows; i++)
  {
    // Get device handle and init object
    iows[i] = IowKitGetDeviceHandle(i + 1);
    // Get serial number
    // IowKitGetSerialNumber(iows[i], sn);
    // pid = IowKitGetProductId(iows[i]);
    // printf(" i, iows[i], PID = %d  %p  %lu \n", i, iows[i], pid); 
    
    // for (j = 0; j < 9; j++)
    // snt[j] = sn[j];
    
    // printf(" i+1 = %d, PID = %x, S/N = \"%s\", Rev = %lX\n \n", i + 1, (unsigned int)pid, snt, IowKitGetRevision(iows[i]));
	//  printf(" PID = %x, serialnumber = \"%s\", Rev = %lX\n \n",  (unsigned int)pid, snt, IowKitGetRevision(iows[i]));
    IowKitSetWriteTimeout(iows[i], 1000);
    IowKitSetTimeout(iows[i], 2000);
  }
  
   printf(" Here comes the blink test:  \n");

  parallelport_out(0);
  sleep(3);
  parallelport_out(1);
  sleep(3);
  parallelport_out(0);
   
  printf(" LED blink test completed \n "); 
  
  printf("\n Next we check out the status line ACK.  \n");
  
  // set GPIOs to 1 - that means can use them as inputs!
  WriteSimple(iows[0], 0xFFFF00FF);   // spare port 1 = data lines
  
  printf( " Press the button connected to this line to set it to high (you got 5 seconds). \n "); 
  sleep(5);
  status = parallelport_in();
  int ACK = status & 0b00001; 
  
  printf(" Status line ACK = %d \n \n ",ACK); 
   
  // Close device
  IowKitCloseDevice(devHandle);
  out:
  return 0;
}
