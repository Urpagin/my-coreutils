#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct {
    char* source;
    char* destination;
} Args;

Args parse_args(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(-1);
    }

    return (Args){argv[1], argv[2]};
}

int main(int argc, char* argv[]) {

    Args args = parse_args(argc, argv);

    // Open input file in read-only mode.
    int fd_in = open(args.source, O_RDONLY);
    // Open out file in write + create mode. And add permissions for user?
    int fd_out = open(args.destination, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);

    // Buffer of 4M
    const ssize_t BUF_SIZE = 4 * 1024;
    char byte_buf[BUF_SIZE];

    ssize_t total_written = 0;
    unsigned long begin = (unsigned long)time(NULL);

    for (;;) {
        ssize_t read_once = read(fd_in, byte_buf, BUF_SIZE);
        ssize_t written = write(fd_out, byte_buf, read_once);

        if (written == -1) {
            fprintf(stderr, "Error writing to output file!\n");
            exit(-1);
        }

        total_written += written;
        unsigned long now = (unsigned long)time(NULL);
        if ((now - begin) >= 1)  {
            begin += 1;
            fflush(stdout);
            printf("Written %dMiB to %s.\r", (int)(total_written / (1024 * 1024)), args.destination);
            fflush(stdout);
        }

        if (read_once == 0) {
            printf("Written %dMiB to %s.\r", (int)(total_written / (1024 * 1024)), args.destination);
            fflush(stdout);
            break;
        }

    }

    // Add newline.
    printf("\n");


    printf("Successfully copied '%s' to '%s'.\n", args.source, args.destination);
    return 0;
}
