#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h> // for ptsname

int main() {
    int master_fd = open("/dev/ptmx", O_RDONLY);

    char *pts_name = ptsname(master_fd);

    printf("Master fd: %d, matching slave: %s\n", master_fd, ptsname);

    return 0;
}