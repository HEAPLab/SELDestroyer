#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>

void protocol_digest_char(char c);

void protocol_update(void);

extern bool protocol_alert_SEL;


#endif /* PROTOCOL_H */