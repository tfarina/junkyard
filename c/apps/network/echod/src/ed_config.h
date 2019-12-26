#ifndef ED_CONFIG_H_
#define ED_CONFIG_H_

#include <unistd.h>

#define ED_USER "_echod"
#define ED_DEFAULT_LISTEN_ADDR "0.0.0.0"
#define ED_DEFAULT_LISTEN_PORT 7
#define ED_DEFAULT_LISTEN_BACKLOG 1024

typedef struct ed_config_s {
  pid_t pid;           /* process: server id */
  int  daemonize:1;    /* process: daemon mode */
  char *conffile;      /* process: configuration filename */
  char *logfile;       /* debug:   log filename */
  char *pidfile;       /* process: pid file */
  char *username;      /* process: run as another user */
  char *address;       /* network: local address to bind */
  int  port;           /* network: tcp listening port */
  int  backlog;        /* network: tcp backlog */
} ed_config_t;

void ed_config_init(ed_config_t *config);
void ed_config_load(ed_config_t *config);

#endif  /* ED_CONFIG_H_ */
