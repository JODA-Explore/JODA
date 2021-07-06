//
// Created by Nico on 27/11/2018.
//

#include "JSONFileReader.h"
#include <fcntl.h>
#include <joda/document/FileOrigin.h>
#include <joda/misc/FileNameRepo.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "joda/config/config.h"

joda::docparsing::JSONFileReader::JSONFileReader(IQueue* iqueue, OQueue* oqueue,
                                                 void* /*ptr*/)
    : IWorkerThread(iqueue, oqueue, nullptr) {
  oqueue->registerProducer();
  DLOG(INFO) << "Started JSONFileReader";
}

joda::docparsing::JSONFileReader::~JSONFileReader() {
  oqueue->unregisterProducer();
  DLOG(INFO) << "Stopped JSONFileReader";
}

void joda::docparsing::JSONFileReader::work() {
  std::vector<OPayload> buff;
  buff.reserve(config::read_bulk_size);
  unsigned int i = 0;
  while (shouldRun) {
    if (!iqueue->isFinished()) {
      SampleFile sfile;
      iqueue->retrieve(sfile);
      auto& file = sfile.file;

      // Check for empty filename
      if (file.empty()) {
        LOG(WARNING) << "Empty filename was received by reader";
        continue;
      };

      // Get fileid
      auto fileID = g_FileNameRepoInstance.addFile(file);
      /*
       * Process file
       */
      // Open file
      int fd = open(file.c_str(), O_RDONLY);  // open file
      if (fd == -1) {
        LOG(ERROR) << "Could not open \"" << file << "\" for reading ("
                   << strerror(errno) << ").";
        continue;
      }
      // Get file stats
      struct stat fs {};
      if (fstat(fd, &fs) == -1) {
        LOG(ERROR) << "Could not stat \"" << file << "\" for reading ("
                   << strerror(errno) << ").";
        close(fd);
        continue;
      }

      posix_fadvise(fd, 0, 0,
                    POSIX_FADV_SEQUENTIAL);  // announce the desire to
                                             // sequentialy read this file
      // silent error handling - weak error

      // Mmap file
      char* buf =
          static_cast<char*>(mmap(nullptr, static_cast<size_t>(fs.st_size),
                                  PROT_READ, MAP_SHARED, fd, 0));
      if (buf == MAP_FAILED) {
        LOG(ERROR)
            << "Could not memoryServer mode did quit on every input map file \""
            << file << "\" (" << strerror(errno) << ").";
        close(fd);
        continue;
      }

      char* buff_end = buf + fs.st_size;
      char *begin = buf, *end = nullptr;

      int index = 0;
      // long line_start = 0;
      // long line_end = 0;
      // search for newline in the remainder in the file
      while ((end = static_cast<char*>(memchr(
                  begin, '\n', static_cast<size_t>(buff_end - begin)))) !=
             nullptr) {
        std::string line(begin, end);

        // Check sample
        if (!(sfile.sample < 1.0 &&
              (static_cast<double>(rand() % 100)) / 100.0 >=
                  sfile.sample)) {  // Sampling
          // line_end = line_start+line.length()+1;
          buff.emplace_back(std::make_unique<FileOrigin>(fileID, begin - buf,
                                                         end - buf + 1, index),
                            std::move(line));
          // line_start = line_end+1;
          i++;
        }

        if (i % config::read_bulk_size == 0) {
          oqueue->send(std::make_move_iterator(buff.begin()), i);
          buff.clear();
          i = 0;
        }

        if (end != buff_end) {
          begin = end + 1;
        } else {
          break;
        }
      }

      munmap(buf, static_cast<size_t>(fs.st_size));
      // silent error handling - weak error

      close(fd);
    } else {
      shouldRun = false;
    }
  }
  if (i > 0) {
    oqueue->send(std::make_move_iterator(buff.begin()), i);
    buff.clear();
  }
  oqueue->producerFinished();
}
