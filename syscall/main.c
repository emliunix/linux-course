#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>

int main(void) {
  long ret = syscall(327);
  errno = ret;
  if(0 != ret) {
    perror("error");
  }
  return ret;
}
