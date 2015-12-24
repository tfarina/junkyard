/*
 * Heavily modified from https://github.com/reagent/http
 */

#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECVSIZE 1024

typedef struct {
	char *data;
	size_t len;
	size_t cap;
} sbuf;

#if defined(__GNUC__)
#define NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

static NORETURN void fatal(const char *fmt, ...)
{
        va_list args;

        va_start(args, fmt);

        fprintf(stderr, "fatal: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");

        va_end(args);

        exit(EXIT_FAILURE);
}

static void *xrealloc(void *p, size_t n)
{
	p = realloc(p, n);
        if (p == NULL)
                fatal("out of memory: %lu", n);
	return p;
}

static void sbuf_setlen(sbuf *b, size_t len)
{
        b->len = len;
        b->data[len] = '\0';
}

static void sbuf_init(sbuf *b)
{
	b->data = NULL;
	b->len = 0;
	b->cap = 0;
}

static void sbuf_free(sbuf *b)
{
	free(b->data);
	sbuf_init(b);
}

static void sbuf_grow(sbuf *b, size_t extra)
{
	size_t want;

        want = b->len + extra;
        if (want > b->cap) {
                b->cap = 2 * want;
                if (b->cap < 64)
                        b->cap = 64;
                b->data = xrealloc(b->data, b->cap);
        }
}

static void sbuf_append(sbuf *b, const void *data, size_t len)
{
	sbuf_grow(b, len);
	memcpy(b->data + b->len, data, len);
        sbuf_setlen(b, b->len + len);
}

static ssize_t fd_write(int fd, char *buf, size_t len)
{
  ssize_t rv;

  do {
    rv = write(fd, buf, len);
  } while (rv == -1 && errno == EINTR);

  return rv;
}

static ssize_t fd_write_all(int fd, char *buf, size_t len)
{
  ssize_t bytes_sent, total_bytes_sent = 0;

  while (len) {
    bytes_sent = fd_write(fd, buf, len);
    if (bytes_sent <= 0)
      return -1;

    printf("bytes sent: %ld\n", bytes_sent);

    buf += bytes_sent;
    len -= bytes_sent;
    total_bytes_sent += bytes_sent;
  }

  return total_bytes_sent;
}

static ssize_t fd_read(int fd, char *buf, size_t len)
{
  ssize_t rv;

  do {
    rv = read(fd, buf, len);
  } while (rv == -1 && errno == EINTR);

  return rv;
}

static ssize_t fd_read_all(int fd, char *buf, size_t len)
{
  char *b = buf;

  while (len) {
    ssize_t bytes_received;

    bytes_received = fd_read(fd, b, len);
    if (bytes_received < 0)
      return -1;
    if (bytes_received == 0)
      break;

    b += bytes_received;
    len -= bytes_received;
  }

  return b - buf;
}

int main(int argc, char **argv) {
  int rv;
  char host[] = "google.com";
  int port = 80;
  char portstr[6];  /* strlen("65535") + 1; */
  struct addrinfo hints, *addrlist, *cur;
  int sockfd;
  char request[1024];
  size_t bytes_to_send;
  ssize_t bytes_received;
  char data[RECVSIZE];
  sbuf response;

  snprintf(portstr, sizeof(portstr), "%d", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(host, portstr, &hints, &addrlist)) != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(rv));
    exit(EXIT_FAILURE);
  }

  /* Loop through all the results and connect to the first we can. */
  for (cur = addrlist; cur != NULL; cur = cur->ai_next) {
    printf("Connecting to %.200s port %s.\n", host, portstr);

    if ((sockfd = socket(cur->ai_family, cur->ai_socktype,
                         cur->ai_protocol)) == -1) {
      continue;
    }

    if (connect(sockfd, cur->ai_addr, cur->ai_addrlen) == 0) {
      break;
    } else {
      close(sockfd);
      sockfd = -1;
    }
  }

  if (sockfd == -1) {
    fprintf(stderr, "Failed to connect to %s\n", host);
    freeaddrinfo(addrlist);
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(addrlist);

  printf("Connection established.\n");

  sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
          "/", host);

  bytes_to_send = strlen(request);

  printf("bytes to send: %ld\n", bytes_to_send);

  if (fd_write_all(sockfd, request, strlen(request)) == -1)
    fatal("Failed to write the HTTP request");

  printf("HTTP request sent, awaiting response...\n"),

  printf("receiving data...");

  sbuf_init(&response);

  while ((bytes_received = fd_read_all(sockfd, data, sizeof(data))) > 0) {
    sbuf_append(&response, data, bytes_received);
  }

  printf("finished receiving data.\n\n");

  printf("%s\n", response.data);

  sbuf_free(&response);

  return 0;
}
