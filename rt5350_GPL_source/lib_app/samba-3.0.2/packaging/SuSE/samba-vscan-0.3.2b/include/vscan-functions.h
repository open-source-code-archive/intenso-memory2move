#ifndef __VSCAN_FUNCTIONS_H_
#define __VSCAN_FUNCTIONS_H_

#include "vscan-global.h"

#define MAX_ENC_LENGTH_STR 8196

BOOL set_boolean(BOOL *b, char *value);
void vscan_syslog(char *printMessage, ...);
void vscan_syslog_alert(char *printMessage, ...);
char* encode_string (const char *s);

#endif /* __VSCAN_FUNCTIONS_H */
