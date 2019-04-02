#include "ec_net.h"

#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

int ec_net_tcp_socket_connect(char const *host, int port) {
  char portstr[6];  /* strlen("65535") + 1; */
  struct addrinfo hints, *addrlist, *cur;
  int rv;
  int tcpfd;

  snprintf(portstr, sizeof(portstr), "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(host, portstr, &hints, &addrlist)) != 0) {
    error("getaddrinfo failed: %s", gai_strerror(rv));
    return -1;
  }

  /* Loop through all the results and connect to the first we can. */
  for (cur = addrlist; cur != NULL; cur = cur->ai_next) {
    if ((tcpfd = socket(cur->ai_family, cur->ai_socktype,
                        cur->ai_protocol)) == -1) {
      error("socket failed: %s", strerror(errno));
      break;
    }

    if (connect(tcpfd, cur->ai_addr, cur->ai_addrlen) == 0) {
      break;
    }

    /* If we can't connect, try the next one. */
    close(tcpfd);
    tcpfd = -1;
  }

  /* Oops, we couldn't connect to any address. */
  if (tcpfd == -1 && cur == NULL) {
    freeaddrinfo(addrlist);
    return -1;
  }

  freeaddrinfo(addrlist);

  return tcpfd;
}