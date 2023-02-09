#include <fcntl.h>
#include <joda/parser/pipeline/LSFileMapper.h>
#include <sys/mman.h>
#include <sys/stat.h>

void joda::queryexecution::pipeline::tasks::load::LSFileMapperExec::fillBuffer(
    std::optional<Input>& ibuff, std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto input = std::move(ibuff.value());
    ibuff.reset();
    const auto& path = input.first;

    if (!std::filesystem::exists(path)) {
      LOG(ERROR) << "Can't open file '" << path << "': does not exist";
      return;
    }
    if (!std::filesystem::is_regular_file(path)) {
      LOG(ERROR) << "Can't open file '" << path << "': is not regular file";
      return;
    }

    // Get fileid
    auto fileID = g_FileNameRepoInstance.addFile(path);

    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
      LOG(ERROR) << "Can't open file '" << path << "': " << strerror(errno);
      return;
    }

    struct stat fs {};
    if (fstat(fd, &fs) == -1) {
      LOG(ERROR) << "Could not stat '" << path << "': " << strerror(errno);
      close(fd);
      return;
    }

    posix_fadvise(fd, 0, 0,
                  POSIX_FADV_SEQUENTIAL);  // announce the desire to
                                           // sequentialy read this file

    // Mmap file
    char* buf =
        static_cast<char*>(mmap(nullptr, static_cast<size_t>(fs.st_size),
                                PROT_READ, MAP_SHARED, fd, 0));

    if (buf == MAP_FAILED) {
      LOG(ERROR) << "Could not memory map file '" << path
                 << "': " << strerror(errno);
      close(fd);
      return;
    }

    char* buff_end = buf + fs.st_size;
    char *begin = buf, *end = nullptr;

    int index = 0;

    
    if(!obuff.has_value()){
      Output output;
      output.reserve(OutBulk);
      obuff = std::move(output);
    }

    // Search newlines
    while ((end = static_cast<char*>(
                memchr(begin, '\n', static_cast<size_t>(buff_end - begin)))) !=
           nullptr) {
      std::string line(begin, end);

      if (!(input.second < 1.0 && (static_cast<double>(rand() % 100)) / 100.0 >=
                                      input.second)) {  // Sampling
        obuff->emplace_back(std::make_unique<FileOrigin>(fileID, begin - buf,
                                                        end - buf + 1, index),
                           std::move(line));
      }

      if (obuff->size() >= OutBulk) {
        sendPartial(obuff);
        DCHECK(!obuff.has_value());
        Output output;
        output.reserve(OutBulk);
        obuff = std::move(output);
      }

      if (end != buff_end) {
        begin = end + 1;
      } else {
        break;
      }
    }

    // Unmap file
    munmap(buf, static_cast<size_t>(fs.st_size));

    // close file
    close(fd);

  }
}

joda::queryexecution::pipeline::tasks::load::LSFileMapperExec::
    LSFileMapperExec() {}
