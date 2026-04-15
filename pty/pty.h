#ifndef PTY_HEADER
#define PTY_HEADER

#define _XOPEN_SOURCE 600 // Needed to release header guards in stdlib.h for ptsname
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h> // for ptsname
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h> // Needed to put parent terminal in raw mode

#endif // PTY_HEADER