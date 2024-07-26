#pragma once
#include <vector>
#include <string>

/**
 * Provides the list of available non-virtual TTY device names (devnode).
 *
 * A heuristic is applied in order to filter for non-virtual devices.
 *
 * \returns a list of available serial ports
 */
std::vector<std::string> getTtyDeviceNames();
