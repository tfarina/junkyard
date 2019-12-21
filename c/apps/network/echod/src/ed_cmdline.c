#include "ed_cmdline.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "ed_rcode.h"
#include "ed_utils.h"
#include "ed_version.h"

#define CRLF "\x0d\x0a"

int show_help = 0;
int show_version = 0;

int ed_cmdline_parse(int argc, char **argv, char const *program_name, ed_config_t *config) {
  int opt_char, value;

  static char const short_options[] =
    "h"  /* help */
    "V"  /* version */
    "d"  /* daemon mode */
    "c:" /* configuration file */
    "L:" /* log file */
    "P:" /* pid file */
    "u:" /* user identity to run as */
    "l:" /* interface to listen on */
    "p:" /* tcp port number to listen on */
    "b:" /* tcp backlog queue limit */
    ;

  static struct option const long_options[] = {
    { "help",        no_argument,       NULL, 'h' }, /* help */
    { "version",     no_argument,       NULL, 'V' }, /* version */
    { "daemonize",   no_argument,       NULL, 'd' }, /* daemon mode */
    { "conffile",    required_argument, NULL, 'c' }, /* configuration file */
    { "logfile",     required_argument, NULL, 'L' }, /* log file */
    { "pidfile",     required_argument, NULL, 'P' }, /* pid file */
    { "user",        required_argument, NULL, 'u' }, /* user identity to run as */
    { "interface",   required_argument, NULL, 'l' }, /* interface to listen on */
    { "port",        required_argument, NULL, 'p' }, /* tcp port number to listen on */
    { "backlog",     required_argument, NULL, 'b' }, /* tcp backlog queue limit */
    { NULL,          0,                 NULL,  0  }
  };

  opterr = 0;

  for (;;) {
    opt_char = getopt_long(argc, argv, short_options, long_options, NULL);
    if (opt_char == -1) {
      /* no more options */
      break;
    }

    switch (opt_char) {
    case 'h':
      show_help = 1;
      break;

    case 'V':
      show_version = 1;
      break;

    case 'd':
      config->daemonize = 1;
      break;

    case 'c':
      config->conffile = optarg;
      break;

    case 'L':
      config->logfile = optarg;
      break;

    case 'P':
      config->pidfile = optarg;
      break;

    case 'u':
      config->username = optarg;
      break;

    case 'l':
      config->interface = optarg;
      break;

    case 'p':
      value = atoi(optarg);
      if (value <= 0) {
	fprintf(stderr, "%s: option -p requires a non zero number\n", program_name);
	return ED_ERROR;
      }
      if (!ed_valid_port(value)) {
	fprintf(stderr, "%s: option -s value %d is not a valid port\n", program_name, value);
	return ED_ERROR;
      }

      config->port = value;
      break;

    case 'b':
      value = atoi(optarg);
      if (value <= 0) {
	fprintf(stderr, "%s: option -b requires a non zero number\n", program_name);
	return ED_ERROR;
      }

      config->backlog = value;
      break;

    case '?':
      fprintf(stderr, "%s: invalid option -- '%c'\n", program_name, optopt);
      fprintf(stderr, "Try '%s --help' for more information.\n", program_name);
      exit(EXIT_FAILURE);
      return ED_ERROR;

    default:
      fprintf(stderr, "%s: invalid option -- '%c'\n", program_name, optopt);
      return ED_ERROR;
      /* NOTREACHED */
    }
  }

  return ED_OK;
}

void ed_cmdline_help(char const *program_name) {
  fprintf(stderr,
	  "Usage: %s [OPTION]..." CRLF CRLF, program_name);

  fprintf(stderr, "Mandatory arguments to long options are mandatory for short options too." CRLF CRLF);

  fprintf(stderr, "Options:" CRLF);
  fprintf(stderr, "  -d, --daemonize            run Echod as daemon (background mode)" CRLF);
  fprintf(stderr, "  -c, --conffile=FILE        read configuration from the specified file" CRLF);
  fprintf(stderr, "  -L, --logfile=FILE         write log messages to the specified file" CRLF);
  fprintf(stderr, "  -P, --pidfile=FILE         write process id to the specified file" CRLF);
  fprintf(stderr, "  -u, --user=NAME            user identity to run as" CRLF);
  fprintf(stderr, "  -l, --interface=S          interface to listen on" CRLF);
  fprintf(stderr, "  -p, --port=VALUE           set the tcp port to listen on" CRLF);
  fprintf(stderr, "  -b, --backlog=VALUE        the backlog argument of listen() applied to the" CRLF);
  fprintf(stderr, "                             listening socket" CRLF);
  fprintf(stderr, "  -h, --help                 display this help and exit" CRLF);
  fprintf(stderr, "  -V, --version              output version information and exit" CRLF);
}

void ed_cmdline_display_version(void) {
  printf("Echod version %s\n", ED_VERSION_STR);
  fflush(stdout);
}
