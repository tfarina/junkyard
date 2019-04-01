#ifndef LOG_H_
#define LOG_H_

#include "macros.h"

void fatal(const char *fmt, ...) ATTRIBUTE_PRINTF(1, 2) NORETURN;
void error(const char *fmt, ...) ATTRIBUTE_PRINTF(1, 2);
void warning(const char *fmt, ...) ATTRIBUTE_PRINTF(1, 2);
void info(const char *fmt, ...) ATTRIBUTE_PRINTF(1, 2);

#endif  /* LOG_H_ */
