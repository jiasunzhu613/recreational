#include "pty.h"

int main() {
    struct termios original_termios;

    // same as: open("/dev/ptmx", O_RDWR);
    int ptm_fd = posix_openpt(O_RDWR);// give read write access

    int grant_error = grantpt(ptm_fd);
    if (grant_error != 0) {
        perror("grantpt erorred");
        return 1;
    }

    int unlock_error = unlockpt(ptm_fd);
    if (unlock_error != 0) {
        perror("unlockpt errored");
        return 1;
    }

    char *pts_name = ptsname(ptm_fd);
    if (pts_name == NULL) {
        perror("error on ptsname retrieval");
        return 1;
    }

    // Save original terminal settings and set to RAW mode
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios; // define a new termios config to put parent terminal to raw mode
    cfmakeraw(&raw);
    raw.c_oflag |= OPOST | ONLCR; // TODO: figure out what this is doing...
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    int pid = fork(); // Fork out child process for execing into shell

    if (pid < 0) {
        perror("error on fork");
        return 1;
    }

    if (pid == 0) { // child process
        int sid = setsid();

        if (sid == -1) {
            perror("error on creating new session");
            exit(1);
        }

        // dup2 pts fd into all standard fds
        // Open pts_name to get fd
        int pts_fd = open(pts_name, O_RDWR); // give read write access
        if (pts_fd == -1) {
            perror("error opening pts fd");
            exit(1);
        }
        for (int i = 0; i < 3; i++) {
            int dup_error = dup2(pts_fd, i); // dup2(oldfd, newfd) => dups newfd and points it to the file description of oldfd
            if (dup_error == -1) {
                perror("error on fd duping");
                exit(1);
            }
        }
        close(pts_fd); // close after duping into standard file descriptors
        close(ptm_fd); // not needed in child process 

        // tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);

        // Exec into bash script
        char *command[] = {"/bin/sh", NULL};
        execvp(command[0], command);
        exit(1);
    }

    // Setup fd_set for select IO multiplexing
    fd_set readfds;

    // End session when child dies
    // Pseudo terminal loop
    while (1) {
        FD_ZERO(&readfds); // Zero out select fd_set (blank slate)
        FD_SET(STDIN_FILENO, &readfds); // Set fds for every iteration of select
        FD_SET(ptm_fd, &readfds);

        // Since we only care about if the child dies, we can disregard status
        if (waitpid(pid, NULL, WNOHANG) > 0) {
            break; // break out of psuedo terminal loop
        }

        // NOTE: +1 for select to be able to include upper bound fd
        if (select(ptm_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("error on select");
            return 1;
        }

        // Check if stdin had any input to read from user
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // read input and write it into ptm_fd which will be treated as input for pts_fd
            char c; // NOTE: this can easily be a char buffer to read more characters
            size_t length = read(STDIN_FILENO, &c, sizeof(char));
            size_t write_error = write(ptm_fd, &c, sizeof(char));
            if (write_error == -1) {
                perror("error on write to ptm");
                return 1;
            }

            if (c == '\n') {
                printf("zhupty$ ");
            }
        }
        
        // Check if ptm_fd has any input to read (this is technically the output from pts_fd)
        if (FD_ISSET(ptm_fd, &readfds)) {
            char c; // NOTE: this can easily be a char buffer to read more characters
            size_t length = read(ptm_fd, &c, sizeof(char));
            putchar(c);
        }
    }

    // Set parent terminal back in canonical mode after pty ends
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    return 0;
}