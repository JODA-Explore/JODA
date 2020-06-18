//
// Created by Nico Schäfer on 5/16/17.
//

#ifndef JODA_MEMORYUTILITY_H
#define JODA_MEMORYUTILITY_H
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <cstdlib>
#include <cstdio>
#include <string>

/**
 * This class retrieves information about the host system and programs memory consumption.
 */ 
class MemoryUtility {

public:
  typedef unsigned long long MemoryT;
  /**
   * This class represents a memory size.
   * It stores the size in byte form, but is able to convert into all representations.
   */ 
  class MemorySize {
    public:
      /**
       * Most commenly used memory sizes mapped to their power of ten.
       */ 
      enum SIZE { B = 0, KB = 1, MB = 2, GB = 3, TB = 4, PB = 5, EB = 6 };

      /**
       * Initializes a memory size with a number of bytes.
       * @param bytes The number of bytes
      */ 
      MemorySize(const MemoryT bytes);

      /**
      * Converts a memory size that is stored in a different power of ten to bytes.
      * @param xbytes the number of <powerOfThousand>bytes to convert
      * @param powerOfThousand The power of thousand of the input
      * @return The number of bytes.
      */
      static MemoryT convertToBytes(const MemoryT xbytes, const int powerOfThousand = SIZE::B);

      /**
      * Converts a memory size that is stored in a different power of ten (SI) to bytes.
      * @param xbytes the number of <powerOfThousand>bytes (SI) to convert
      * @param powerOfThousand The power of thousand of the input
      * @return The number of bytes.
      */
      static MemoryT convertToBytesSI(const MemoryT xbytes, const int powerOfThousand = SIZE::B);

      /**
       * Returns the number of bytes of the memory representation
       */
      MemoryT getBytes() const;

      /**
       * Returns the number of Xbytes of the memory representation with the given base
       * @param base The power of ten of the required output.
       * @return The number of Xbytes
       */
      MemoryT getBase(const int base = SIZE::B) const;

      /**
       * Returns the number of Xbytes of the memory representation with the given base in SI notation
       * @param base The power of ten of the required output.
       * @return The number of SI Xbytes
       */
      MemoryT getBaseSI(const int base = SIZE::B) const;

      /**
       * Returns the number of Xbytes of the memory representation with the given base
       * @param base The power of ten of the required output.
       * @return The number of Xbytes
       */
      double getFBase(const int base = SIZE::B) const;

      /**
       * Returns the number of Xbytes of the memory representation with the given base in SI notation
       * @param base The power of ten of the required output.
       * @return The number of SI Xbytes
       */
      double getFBaseSI(const int base = SIZE::B) const;

      /**
       * Returns a human readable string of the memory size.
       * Uses a maximum of two playes after the comma.
       * @return a human readable string of the memory size
       */
      std::string getHumanReadable() const;

      /**
       * Returns a human readable string of the memory size in SI.
       * Uses a maximum of two playes after the comma.
       * @return a human readable string of the memory size
       */
      std::string getHumanReadableSI() const;
    private:
      // 0 to 18,446,744,073,709,551,615  => supports up to 18 exabyte
      MemoryT bytes;
  };
  
  /**
   * Retrieves the total installed memory of the system
   * @return the total system memory as MemorySize
   */ 
  static MemorySize totalRam();
  
  /**
   * Retrieves the total memory usage of the system
   * @return the total memory usage as MemorySize
   */ 
  static MemorySize sysRamUsage();

  /**
   * Retrieves the program memory usage of the system
   * @return the program memory usage as MemorySize
   */ 
  static MemorySize procRamUsage();

  /**
   * Retrieves the remaining free memory of the system
   * @return the remaining free memory as MemorySize
   */ 
  static MemorySize remainingRam();

  /**
   * Calculates the percentage of free memory.
   * @return the percentage of free memory in [0,1]
  */ 
  static double getFreeRam();

  /**
   * Creates a human readable memory usage report.
   * @return the report as string
   */ 
  static std::string getRamUsage();

  /**
   * Helper function to translate a percentage in [0,1] to human readable format in "[0,100]%"
   * @return human readable percentage
  */ 
  static std::string humanReadablePercent(double num);
private:
  static int parseLine(char* line);
};


#endif //JODA_MEMORYUTILITY_H
