#include "external.h"
#include <ctype.h>
#include <string.h>

/*****************************************************************************/
/* STRCASECMP() - Case-insensitive strcmp.                                   */
/*****************************************************************************/
int strcasecmp(const char* s1, const char* s2)
{
	char c1, c2;
	do { c1 = *s1++; c2 = *s2++; }
	while (c1 && c2 && (tolower(c1) == tolower(c2)));

	return tolower(c1) - tolower(c2);
}

/*****************************************************************************/
/* STRNCASECMP() - Case-insensitive strncmp.                                 */
/*****************************************************************************/
int strncasecmp(const char* s1, const char* s2, size_t n)
{
	char c1, c2;
	
	if (!n) return 0;
	
	do { c1 = *s1++; c2 = *s2++; }
	while (--n && c1 && c2 && (tolower(c1) == tolower(c2)));
	
	return tolower(c1) - tolower(c2);
}