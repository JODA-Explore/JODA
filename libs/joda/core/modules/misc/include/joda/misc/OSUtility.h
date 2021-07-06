//
// Created by Nico on 06/03/2020.
//

#ifndef JODA_OSUTILITY_H
#define JODA_OSUTILITY_H
#include <string>

/**
 * This class retrieves information about the hosts operation system.
 */
class OSUtility {
 public:
  /**
   * Retrieves the name of the OS.
   * For example Debian Buster, Ubuntu 18.04, ...
   * @return name string of the OS
   */
  static const std::string &getOS();
  /**
   * Retrieves the kernel version string
   * @return kernel version string
   */
  static const std::string &getKernel();

 private:
  static void initKernel();
  static void initOS();

  static std::string os;
  static std::string kernel;
};

#endif  // JODA_OSUTILITY_H
