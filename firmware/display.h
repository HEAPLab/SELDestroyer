#ifndef DISPLAY_H /* Guard against multiple inclusion */
#define DISPLAY_H

#include "ssd1306.h"

void display_init_sequence(void);
void display_update(void);
void display_set_frozen(void);

#endif /* I2C_H */
