#include "fnet.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

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

int fnet_set_reuseaddr(int fd, char *err)
{
  int reuse = 1;

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
    fnet_set_error(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
    return FNET_ERR;
  }

  return FNET_OK;
}

int fnet_create_socket(int domain)
{
  int sockfd;

  if ((sockfd = socket(domain, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "failed to create socket\n");
    return FNET_ERR;
  }

  return sockfd;
}

static int fnet_create_socket_r(char *err, int domain)
{
        int sockfd;

        if ((sockfd = socket(domain, SOCK_STREAM, 0)) == -1) {
                fnet_set_error(err, "error creating socket: %s", strerror(errno));
                return FNET_ERR;
        }

        return sockfd;
}

int fnet_unix_server(const char *path, int backlog)
{
  int sockfd;
  struct sockaddr_un sa;
  size_t salen;

  if ((sockfd = fnet_create_socket(AF_UNIX)) == FNET_ERR) {
    return FNET_ERR;
  }

  memset(&sa, 0, sizeof(sa));
  sa.sun_family = AF_UNIX;
  strncpy(sa.sun_path, path, sizeof(sa.sun_path));
  salen = strlen(path) + 1 + offsetof(struct sockaddr_un, sun_path);

  if (bind(sockfd, (const struct sockaddr*)&sa, salen) == -1) {
    fprintf(stderr, "bind() failed: %s\n", strerror(errno));
    close(sockfd);
    return FNET_ERR;
  }

  if (listen(sockfd, backlog) == -1) {
    fprintf(stderr, "listen() failed: %s\n", strerror(errno));
    close(sockfd);
    return FNET_ERR;
  }

  return sockfd;
}

int fnet_generic_accept(int sockfd, struct sockaddr *sa, socklen_t *salen)
{
  int fd;

  for (;;) {
    fd = accept(sockfd, sa, salen);
    if (fd == -1) {
      if (errno == EINTR) {
	continue;
      } else {
        fprintf(stderr, "accept() failed: %s\n", strerror(errno));
        return FNET_ERR;
      }
    }
    break;
  }

  return fd;
}

int fnet_unix_socket_accept(int sockfd)
{
  int fd;
  struct sockaddr_un sa;
  socklen_t salen = sizeof(sa);

  if ((fd = fnet_generic_accept(sockfd, (struct sockaddr *)&sa, &salen)) == -1) {
    return -1;
  }

  return fd;
}

int fnet_unix_client(char *err, const char *path)
{
        int sockfd;
        struct sockaddr_un sa;

        if ((sockfd = fnet_create_socket_r(err, AF_UNIX)) == FNET_ERR) {
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
