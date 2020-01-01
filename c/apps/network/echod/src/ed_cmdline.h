#ifndef ED_CMDLINE_H_
#define ED_CMDLINE_H_

#include "ed_config.h"

extern int show_help;
extern int show_version;
extern int show_config;

/**
 * Parse command line options into |config|.
 */
int ed_cmdline_parse(int argc, char **argv, ed_config_t *config);

void ed_cmdline_display_help(char const *program_name);

void ed_cmdline_display_version(char const *program_name);

#endif  /* ED_CMDLINE_H_ */
