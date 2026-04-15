#include "pty.h"

int main() {
    struct termios original_termios;
    // same as: open("/dev/ptmx", O_RDWR);
    int ptm_fd =  posix_openpt(O_RDWR);// give read write access

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
    struct termios raw = original_termios;
    cfmakeraw(&raw);
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
            int dup_error = dup2(pts_fd, i);
            if (dup_error == -1) {
                perror("error on fd duping");
                exit(1);
            }
        }
        close(pts_fd); // close after duping into standard file descriptors
        close(ptm_fd); // not needed in child process 

        // Exec into bash script
        char *command[] = {"/bin/bash", NULL};
        execvp(command[0], command);
        exit(1);
        // if (exec_error == -1) {
        //     perror("error on exec");
        //     exit(1);
        // }
    }

    // Setup fd_set for select IO multiplexing
    fd_set readfds;

    // End session when child dies
    // Pseudo terminal loop
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // apparently this needs to be set every iteration for select?
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

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // read input and write it into ptm_fd which will be treated as input for pts_fd
            char c;
            size_t length = read(STDIN_FILENO, &c, sizeof(char));
            size_t write_error = write(ptm_fd, &c, sizeof(char));
            if (write_error == -1) {
                perror("error on write to ptm");
                return 1;
            }
        }
        
        if (FD_ISSET(ptm_fd, &readfds)) {
            char c;
            size_t length = read(ptm_fd, &c, sizeof(char));
            putchar(c);
        }
    }

    // Set parent terminal back in canonical mode after pty ends
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    return 0;
}