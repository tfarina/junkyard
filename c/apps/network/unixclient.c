#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFSIZE 1024

#define NET_ERR_LEN 256

static void net_set_error(char *err, const char *fmt, ...)
{
       va_list ap;

       if (!err) {
               return;
       }

       va_start(ap, fmt);
       vsnprintf(err, NET_ERR_LEN, fmt, ap);
       va_end(ap);
}

static int net_create_socket(char *err, int domain)
{
        int sockfd;

        if ((sockfd = socket(domain, SOCK_STREAM, 0)) == -1) {
                net_set_error(err, "error creating socket: %s", strerror(errno));
                return -1;
        }

        return sockfd;
}

int main(int argc, char **argv) {
        int sockfd;
        char neterr[NET_ERR_LEN];
        struct sockaddr_un unix_addr;
        const char *path;
        char buf[BUFSIZE];
        ssize_t n;
        char pre[10];
        int i;
        const char *space = " ";
        const char *newline = "\n";

        path = "server.socket";

        if ((sockfd = net_create_socket(neterr, AF_UNIX)) == -1) {
                fprintf(stderr, "%s\n", neterr);
                return EXIT_FAILURE;
        }

        memset(&unix_addr, 0, sizeof(unix_addr));
        unix_addr.sun_family = AF_UNIX;
        strncpy(unix_addr.sun_path, path, sizeof(unix_addr.sun_path));

        if (connect(sockfd, (struct sockaddr *)&unix_addr, sizeof(unix_addr)) == -1) {
                fprintf(stderr, "connect failed: %s\n", strerror(errno));
                close(sockfd);
                return -1;
        }

        snprintf(pre, sizeof(pre), "NIXCT%d ", 1);

        if (write(sockfd, pre, strlen(pre)) <= 0) {
                fprintf(stderr, "could not write\n");
        }

        argc -= 1;
        argv += 1;

        for (i = 0; i < argc; i++) {
          printf(" %s", argv[i]);

          write(sockfd, space, strlen(space));

          write(sockfd, argv[i], strlen(argv[i]));
        }

        if (write(sockfd, newline, strlen(newline)) <= 0) {
          fprintf(stderr, "could not write\n");
        }

        while ((n = read(sockfd, buf, sizeof(buf))) > 0 ) {
                buf[n] = 0;
                fputs(buf, stdout);
        }

        return EXIT_SUCCESS;
}