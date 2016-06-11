#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 256

int main(void) {
    int fd;
    char buf[BUF_SIZE];
    fd = open("/dev/hello", O_RDONLY);
    if(fd < 0) { // error 
        fprintf(stderr, "Failed to open /dev/hello.\n");
        return 0;
    }

    int len = read(fd, buf, BUF_SIZE - 1);
    buf[len] = '\0';
    puts(buf);
    return 0;
}
