  
README_parallelport                                    Hebbeker   2024-07-26  
  
  
  
Detailed documentation = article "Parallel Port Revival" by Thomas Hebbeker,  
https://zenodo.org/records/13118320

  
  
  
Folder parallelport_microcontroller-boards:  
===========================================  
  
  
arduino_ethernet_parallelport: arduino_ethernet_parallelport.ino  
  firmware arduino board with communication via ethernet shield  
  
arduino_nano_usb_parallelport: arduino_nano_usb_parallelport.ino  
  firmware arduino nano with USB communication  
  
arduino_uno_usb_parallelport: arduino_uno_usb_parallelport.ino  
  firmware arduino uno with USB communication  
  
esp32_bluetooth_parallelport: esp32_bluetooth_parallelport.ino  
  firmware esp32 with legacy Bluetooth communication  
  
esp32_usb_parallelport: esp32_usb_parallelport.ino  
  firmware esp32 with USB communication  
  
esp32_wlan_parallelport: esp32_wlan_parallelport.ino  
  firmware esp32 with WLAN communication  
  
esp8266_usb_parallelport: esp8266_usb_parallelport.ino  
  firmware esp8266 with USB communication  
  
esp8266_wlan_parallelport: esp8266_wlan_parallelport.ino  
  firmware esp8266 with WLAN communication  
  
raspberry_pico_usb_parallelport: raspberry_pico_usb_parallelport.ino  
  firmware raspberry pico with USB communication  
    
  
  
Folder parallelport_iowarrior:  
==============================  
  
  
iowarrior_parallelport: iowarrior_usb_parallelport_in.c  
  C code on PC for reading from iowarrior 28 via USB  
  
iowarrior_parallelport: iowarrior_usb_parallelport_minitest.c  
  C code on PC for mini-testing of iowarrior 28 via USB  
  
iowarrior_parallelport: iowarrior_usb_parallelport_out.c  
  C code on PC for writing to iowarrior 28 via USB  
  
iowarrior_parallelport: iowarrior_usb_parallelport_test.c  
  C code on PC for mini-testing of iowarrior 28 via USB  
  
iowarrior_parallelport: Makefile_minitest  
  Makefile for iowarrior_usb_parallelport_minitest.c  
  
iowarrior_parallelport: Makefile_test  
  Makefile for iowarrior_usb_parallelport_test.c  
  
  
  
Folder parallelport_pc:  
=======================  
  
  
  
pc_parallelport: arduino_ethernet_parallelport_in.cpp  
  C++ code on PC for reading from arduino board via ethernet  
  
pc_parallelport: arduino_ethernet_parallelport_out.cpp  
  C++ code on PC for writing to arduino board via ethernet  
  
pc_parallelport: arduino_nano_usb_parallelport_in.cpp  
  C++ code on PC for reading from arduino nano via USB using boost  
  
pc_parallelport: arduino_nano_usb_parallelport_minitest.cpp  
  C++ code on PC for mini-testing of arduino nano via USB  
  
pc_parallelport: arduino_nano_usb_parallelport_out.cpp  
  C++ code on PC for writing to arduino nano via USB using boost  
  
pc_parallelport: arduino_nano_usb_system_parallelport_in.cpp  
  C++ code on PC for reading from arduino nano via USB using system commands  
  
pc_parallelport: arduino_nano_usb_system_parallelport_out.cpp  
  C++ code on PC for writing to arduino nano via USB using system commands  
  
pc_parallelport: arduino_uno_usb_parallelport_in.cpp  
  C++ code on PC for reading from arduino uno via USB using boost  
    
pc_parallelport: arduino_uno_usb_parallelport_out.cpp  
  C++ code on PC for writing to arduino uno via USB using boost  
  
pc_parallelport: esp32_bluetooth_parallelport_in.cpp  
  C++ code on PC for reading from esp32 via legacy bluetooth  
  
pc_parallelport: esp32_bluetooth_parallelport_out.cpp  
  C++ code on PC for writing to esp32 via legacy bluetooth  
  
pc_parallelport: esp32_usb_parallelport_in.cpp  
  C++ code on PC for reading from esp32 via USB using boost  
  
pc_parallelport: esp32_usb_parallelport_out.cpp  
  C++ code on PC for writing to esp32 via USB using boost  
  
pc_parallelport: esp32_wlan_parallelport_in.cpp  
  C++ code on PC for reading from esp32 via WLAN  
  
pc_parallelport: esp32_wlan_parallelport_out.cpp  
  C++ code on PC for writing to esp32 via WLAN  
  
pc_parallelport: esp8266_usb_parallelport_in.cpp  
  C++ code on PC for reading from esp8266 via USB using boost  
  
pc_parallelport: esp8266_usb_parallelport_out.cpp  
  C++ code on PC for writing to esp8266 via USB using boost  
  
pc_parallelport: esp8266_wlan_parallelport_in.cpp  
  C++ code on PC for reading from esp8266 via WLAN  
  
pc_parallelport: esp8266_wlan_parallelport_out.cpp  
  C++ code on PC for writing to esp8266 via WLAN  
  
pc_parallelport: parallelport_basictest.cpp  
  C++ code on PC for basic-testing of PP emulator boards  
  
pc_parallelport: parallelport_test.cpp  
  C++ code on PC for basic-testing of PP emulator boards  
  
pc_parallelport: raspberry_usb_ampy_parallelport_in.cpp  
   C++ code on PC for reading from raspberry pico via USB using ampy  
  
pc_parallelport: raspberry_usb_ampy_parallelport_out.cpp  
   C++ code on PC for writing to raspberry pico via USB using ampy  
     
pc_parallelport: raspberry_usb_cpp_parallelport_in.cpp  
   C++ code on PC for reading from raspberry pico via USB using boost  
  
pc_parallelport: raspberry_usb_cpp_parallelport_out.cpp  
   C++ code on PC for writing to raspberry pico via USB using boost  
  
pc_parallelport: raspberry_usb_mpremote_parallelport_in.cpp  
   C++ code on PC for reading from raspberry pico via USB using mpremote  
  
pc_parallelport: raspberry_usb_mpremote_parallelport_out.cpp  
   C++ code on PC for writing to raspberry pico via USB using mpremote  
     
     
     
Folder enumerate_TtyDevices:  
============================  
  
README.md    <==  Provides list of available non-virtual TTY device names (David Hebbeker)  
  
Makefile  
enumerateTtyDevices.cpp  
enumerateTtyDevices.hpp  
list.txt  
main.cpp  
  
  
  
Folder parallelport_wokwi-simulation:  
=====================================  
  
wokwi simulation of ESP32 board (David Hebbeker)  
  
src/main.cpp  
wokwi_files/diagram.json  
wokwi_files/wokwi.toml  
.clang-format  
.gitignore  
.vscode  
platformio.ini  
  
  
  
PS: This text file has two spaces before the LineFeed character at the end, to ensure correct git formatting!
