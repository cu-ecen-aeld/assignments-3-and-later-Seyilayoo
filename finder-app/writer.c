#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_FACILITY LOG_USER

int main(int argc, char *argv[]) {
  // Check for correct number of arguments
  if (argc != 3) {
    syslog(LOG_ERR, "Usage: %s <string> <filename>", argv[0]);
    return 1;
  }

  // Open the file for writing 
  int fd = creat(argv[1], 0644);  // creat opens for writing, truncates
  if (fd == -1) {
    syslog(LOG_ERR, "Error opening file: %s", strerror(errno));
    return 1;
  }

  // Write the string to the file
  size_t written = write(fd, argv[2], strlen(argv[2]));
  if (written != strlen(argv[2])) {
    syslog(LOG_ERR, "Error writing to file");
    close(fd);
    return 1;
  }

  // Log successful write
  syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);

  // Close the file
  close(fd);

  return 0;
}

