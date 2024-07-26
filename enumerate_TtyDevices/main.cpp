#include <stdlib.h>
#include <boost/asio.hpp>
#include "enumerateTtyDevices.hpp"
#include <iostream>

static_assert(BOOST_ASIO_HAS_SERIAL_PORT, "serial port must be available to boost library");

int main(void)
{
  std::cout << "starting PP Emulator Client" << std::endl;

  const auto ttyDeviceNames =  getTtyDeviceNames();
  std::cout << "found " << ttyDeviceNames.size() << " devices" << std::endl;
  for (const auto &ttyDeviceName : ttyDeviceNames)
  {
    std::cout << "TTY device name: '" << ttyDeviceName << "'" << std::endl;
  }

  return EXIT_SUCCESS;
}
