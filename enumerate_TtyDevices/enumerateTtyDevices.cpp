#include "enumerateTtyDevices.hpp"
#include <libudev.h>
#include <stdexcept>
#include <cstring>

/**
 * Checks if the device has a "port" system attribute which is not defined.
 * \param device to be probed
 * \retval true if attribute is available but undefined
 */
static bool hasUndefinedPort(udev_device *const device)
{
  for (auto attribute = udev_device_get_sysattr_list_entry(device); attribute; attribute =
      udev_list_entry_get_next(attribute))
  {
    const auto *const name = udev_list_entry_get_name(attribute);
    if (!name)
      continue;
    if (std::strcmp(name, "port") == 0)
    {
      return !udev_list_entry_get_value(attribute);
    }
  }
  return false;
}

static bool hasDeviceAttribute(udev_device *const device)
{
  for (auto attribute = udev_device_get_sysattr_list_entry(device); attribute; attribute =
      udev_list_entry_get_next(attribute))
  {
    const auto *const name = udev_list_entry_get_name(attribute);
    if (!name)
      continue;
    if (std::strcmp(name, "device") == 0)
    {
      return true;
    }
  }
  return false;
}

std::vector<std::string> getTtyDeviceNames()
{
  auto *const udev_context = udev_new();
  if (!udev_context)
  {
    throw std::runtime_error("cannot create UDEV context");
  }

  auto *const enumeration = udev_enumerate_new(udev_context);
  if (!enumeration)
  {
    udev_unref(udev_context);
    throw std::runtime_error("cannot create device enumeration");
  }

  udev_enumerate_add_match_subsystem(enumeration, "tty");
  udev_enumerate_scan_devices(enumeration);

  auto *const devices = udev_enumerate_get_list_entry(enumeration);
  if (!devices)
  {
    udev_enumerate_unref(enumeration);
    udev_unref(udev_context);
    throw std::runtime_error("Failed to get device list.");
  }

  std::vector<std::string> deviceNodes;
  for (auto dev_list_entry = devices; dev_list_entry; dev_list_entry = udev_list_entry_get_next(dev_list_entry))
  {
    auto *const dev = udev_device_new_from_syspath(udev_context, udev_list_entry_get_name(dev_list_entry));
    if (hasDeviceAttribute(dev) // https://stackoverflow.com/a/15343570/5534993
    && !hasUndefinedPort(dev)   // https://stackoverflow.com/a/12301542/5534993
                         )// heuristic to filter virtual devices
    {
      deviceNodes.push_back(udev_device_get_devnode(dev));
    }
    udev_device_unref(dev);
  }

  udev_enumerate_unref(enumeration);
  udev_unref(udev_context);

  return deviceNodes;
}
