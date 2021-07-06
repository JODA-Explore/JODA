//
// Created by Nico Schäfer on 5/16/17.
//

#include "../include/joda/misc/MemoryUtility.h"

#include <glog/logging.h>
#include <cmath>

#include <boost/format.hpp>
#include <cstring>
#include <iomanip>
#include <sstream>

MemoryUtility::MemorySize::MemorySize(const MemoryT bytes) : bytes(bytes) {}

MemoryUtility::MemoryT MemoryUtility::MemorySize::convertToBytes(
    const MemoryT xbytes, const int powerOfThousand) {
  return std::floor(xbytes * std::pow(1024, powerOfThousand));
}

MemoryUtility::MemoryT MemoryUtility::MemorySize::convertToBytesSI(
    const MemoryT xbytes, const int powerOfThousand) {
  return std::floor(xbytes * std::pow(1000, powerOfThousand));
}

MemoryUtility::MemoryT MemoryUtility::MemorySize::getBytes() const {
  return bytes;
}

MemoryUtility::MemoryT MemoryUtility::MemorySize::getBase(
    const int base) const {
  return std::floor(bytes / std::pow(1024, base));
}

MemoryUtility::MemoryT MemoryUtility::MemorySize::getBaseSI(
    const int base) const {
  return std::floor(bytes / std::pow(1000, base));
}

double MemoryUtility::MemorySize::getFBase(const int base) const {
  return (static_cast<double>(bytes)) / std::pow(1024, base);
}

double MemoryUtility::MemorySize::getFBaseSI(const int base) const {
  return (static_cast<double>(bytes)) / std::pow(1000, base);
}

std::string MemoryUtility::MemorySize::getHumanReadable() const {
  double remainingBytes = bytes;
  size_t suffix = 0;
  std::vector<std::string> suffixes = {"Bi",  "KBi", "MBi", "GBi",
                                       "TBi", "PBi", "EBi"};
  while (remainingBytes > 1024 && suffix < suffixes.size() - 1) {
    remainingBytes /= 1024;
    suffix++;
  }
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << remainingBytes << " "
     << suffixes[suffix];

  DLOG(INFO) << bytes << " / 1024^" << suffix << " = " << ss.str();

  return ss.str();
}

std::string MemoryUtility::MemorySize::getHumanReadableSI() const {
  double remainingBytes = bytes;
  size_t suffix = 0;
  std::vector<std::string> suffixes = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
  while (remainingBytes > 1000 && suffix < suffixes.size() - 1) {
    remainingBytes /= 1000;
    suffix++;
  }
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << remainingBytes << " "
     << suffixes[suffix];
  return ss.str();
}

MemoryUtility::MemorySize MemoryUtility::totalRam() {
  struct sysinfo memInfo {};
  sysinfo(&memInfo);

  auto totalPhysMem = memInfo.totalram;
  // Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;
  return totalPhysMem;
}

MemoryUtility::MemorySize MemoryUtility::sysRamUsage() {
  struct sysinfo memInfo {};
  sysinfo(&memInfo);

  auto physMemUsed = memInfo.totalram - memInfo.freeram;
  // Multiply in next statement to avoid int overflow on right hand side...
  physMemUsed *= memInfo.mem_unit;
  return physMemUsed;
}

MemoryUtility::MemorySize MemoryUtility::remainingRam() {
  struct sysinfo memInfo {};
  sysinfo(&memInfo);

  long long physMemFree = memInfo.freeram;
  // Multiply in next statement to avoid int overflow on right hand side...
  physMemFree *= memInfo.mem_unit;
  return physMemFree;
}

int MemoryUtility::parseLine(char* line) {
  // This assumes that a digit will be found and the line ends in " Kb".
  auto i = static_cast<int>(strlen(line));
  const char* p = line;
  while (*p < '0' || *p > '9') {
    p++;
  }
  line[i - 3] = '\0';
  i = atoi(p);
  return i;
}

MemoryUtility::MemorySize MemoryUtility::procRamUsage() {
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];

  while (fgets(line, 128, file) != nullptr) {
    if (strncmp(line, "VmRSS:", 6) == 0) {
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return MemoryUtility::MemorySize::convertToBytes(
      result, MemoryUtility::MemorySize::KB);
}

std::string MemoryUtility::getRamUsage() {
  return "RAM: " + totalRam().getHumanReadable() +
         " | Used: " + sysRamUsage().getHumanReadable() +
         " | Free: " + humanReadablePercent(getFreeRam()) +
         " | Proc: " + procRamUsage().getHumanReadable();
}

double MemoryUtility::getFreeRam() {
  return 1.0 - (static_cast<double>(sysRamUsage().getBytes())) /
                   totalRam().getBytes();
}

std::string MemoryUtility::humanReadablePercent(double num) {
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(0) << num * 100;
  return ss.str() + "%";
}
