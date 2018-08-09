#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"

void* xrealloc(void *oldptr, size_t newsize)
{
        void *newptr;

	if ((newptr = realloc(oldptr, newsize)) == NULL) {
                fatal("out of memory");
	}

	return newptr;
}

char* xstrdup(char *s)
{
	s = strdup(s);
        if (s == NULL)
                fatal("out of memory");
	return s;
}
