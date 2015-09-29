/* https://vcansimplify.wordpress.com/2013/03/14/c-socket-tutorial-echo-server/ */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "die.h"

#define BACKLOG 1024
#define BUFSIZE 8129

static char *ipaddr(const struct sockaddr *sa) {
  struct sockaddr_in *sin = (struct sockaddr_in *)sa;
  static char str[128];

  if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
    return NULL;

  return str;
}

static void handle_client(int fd) {
  char buf[BUFSIZE];
  int ret;

  while ((ret = read(fd, buf, sizeof(buf))) > 0 && write(fd, buf, ret) > 0);

  sleep(1);  /* allow socket to drain before signalling the socket is closed */
  close(fd);

  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  struct sockaddr_in servaddr;
  int port;
  int listen_fd;
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  int client_fd;
  int pid;

  if (argc != 2) {
    fprintf(stderr, "usage: tcpserver #port-number\n");
    exit(EXIT_FAILURE);
  }

  port = atoi(argv[1]);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    die("socket failed: %s", strerror(errno));

  if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    die("bind failed");

  if (listen(listen_fd, BACKLOG) < 0)
    die("listen failed");

  fprintf(stderr,
          "The server is now ready to accept connections on port %d\n",
          port);

  while (1) {
    if ((client_fd = accept(listen_fd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
      die("accept failed");

    printf("Connection from %s:%d\n", ipaddr((struct sockaddr *)&cliaddr), ntohs(cliaddr.sin_port));

    if ((pid = fork()) < 0) {
      die("fork failed");
    } else {
      if (pid == 0) {	/* child */
	(void)close(listen_fd);
        handle_client(client_fd);
      } else {	/* parent */
        (void)close(client_fd);
      }
    }
  }

  return EXIT_SUCCESS;
}
