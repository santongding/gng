#include "file-lock.h"
#include <stdexcept>


#include <sys/file.h>
#include <unistd.h>

exclusive_lock_file::exclusive_lock_file(const std::string& filename)
    : filename(filename)
{

    fd = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        throw std::runtime_error("Cannot open file " + filename + ".");
    }
    const bool success = flock(fd, LOCK_EX | LOCK_NB) == 0;
    if (!success) {
        close(fd);
        fd = -1;
    }
    if (!success) {
        throw std::runtime_error("Cannot open file " + filename + " for exclusive access.");
    }
}

exclusive_lock_file::~exclusive_lock_file()
{
    if (fd >= 0) {
        flock(fd, LOCK_UN);
        close(fd);
        std::remove(filename.c_str());
    }
}