#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFSIZE 1024

#define FNET_ERR -1
#define FNET_ERR_LEN 256

static void fnet_set_error(char *err, const char *fmt, ...)
{
       va_list ap;

       if (!err) {
               return;
       }

       va_start(ap, fmt);
       vsnprintf(err, FNET_ERR_LEN, fmt, ap);
       va_end(ap);
}

static int fnet_create_socket(char *err, int domain)
{
        int sockfd;

        if ((sockfd = socket(domain, SOCK_STREAM, 0)) == -1) {
                fnet_set_error(err, "error creating socket: %s", strerror(errno));
                return FNET_ERR;
        }

        return sockfd;
}

static int fnet_unix_client(char *err, const char *path) {
        int sockfd;
        struct sockaddr_un sa;

        if ((sockfd = fnet_create_socket(err, AF_UNIX)) == FNET_ERR) {
                fprintf(stderr, "%s\n", err);
                return FNET_ERR;
        }

        memset(&sa, 0, sizeof(sa));
        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, path, sizeof(sa.sun_path));

        if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
                fprintf(stderr, "connect failed: %s\n", strerror(errno));
                close(sockfd);
                return FNET_ERR;
        }

	return sockfd;
}

int main(int argc, char **argv)
{
        int sockfd;
        char neterr[FNET_ERR_LEN];
        const char *path;
        char buf[BUFSIZE];
        ssize_t n;
        char pre[10];
        int i;
        const char *space = " ";
        const char *newline = "\n";

        path = "server.socket";

	sockfd = fnet_unix_client(neterr, path);
	if (sockfd == FNET_ERR) {
	        return EXIT_FAILURE;
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
