//
// Created by Nico on 06/03/2020.
//

#include <joda/misc/OSUtility.h>

#ifdef __linux__
#include <sys/utsname.h>
#elif _WIN32

#else

#endif

void OSUtility::initKernel() {
#ifdef __linux__
  struct utsname unameData;
  uname(&unameData);
  OSUtility::kernel = std::string(unameData.sysname) + " - " + unameData.release;

#elif _WIN32
  OSUtility::kernel = "Windows";
#else
  OSUtility::kernel = "unknown";
#endif
}

void OSUtility::initOS() {
#ifdef __linux__
  FILE *fp;
  char buffer[50] = " ";
  fp = popen("lsb_release -ds", "r");
  if (fp != NULL) {
    fgets(buffer, 50, fp);
    pclose(fp);
  }

  OSUtility::os = std::string(buffer);

#elif _WIN32
  OSUtility::os = "Windows";
#else
  OSUtility::os = "unknown";
#endif
}

const std::string &OSUtility::getOS() {
  if (os.empty()) initOS();
  return os;
}

const std::string &OSUtility::getKernel() {
  if (kernel.empty()) initKernel();
  return kernel;
}

std::string OSUtility::os{};
std::string OSUtility::kernel{};

