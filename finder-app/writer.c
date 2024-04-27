#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
//#include <fcnt1.h>
#include <errno.h>

#define LOG_FACILITY LOG_USER

int main(int argc, char *argv[]) {
  // Check for correct number of arguments
  if (argc != 3) {
    syslog(LOG_ERR, "Usage: %s <string> <filename>", argv[0]);
    return 1;
  }

  // Open the file for writing
  FILE *fp = fopen(argv[2], "w");
  if (fp == NULL) {
    syslog(LOG_ERR, "Error opening file: %s", strerror(errno));
    return 1;
  }

  // Write the string to the file
  size_t written = fwrite(argv[1], sizeof(char), strlen(argv[1]), fp);
  if (written != strlen(argv[1])) {
    syslog(LOG_ERR, "Error writing to file");
    fclose(fp);
    return 1;
  }

  // Log successful write
  syslog(LOG_DEBUG, "Writing %s to %s", argv[1], argv[2]);

  // Close the file
  fclose(fp);

  return 0;
}
