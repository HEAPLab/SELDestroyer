
#ifndef SSD1306_H
#define	SSD1306_H

#include <stdint.h>
#include <stdbool.h>

void SSD1306_init(void);
void SSD1306_start_scroll_right(uint8_t start, uint8_t stop);
void SSD1306_start_scroll_left(uint8_t start, uint8_t stop);
void SSD1306_start_scroll_diag_right(uint8_t start, uint8_t stop);
void SSD1306_start_scroll_diag_left(uint8_t start, uint8_t stop);
void SSD1306_stop_scroll(void);
void SSD1306_dim(char dim);
void SSD1306_set_text_wrap(char w);
void SSD1306_invert_display(char i);
void SSD1306_gotoXY(uint8_t x, uint8_t y);
void SSD1306_putc(uint8_t c);
void SSD1306_putc_stretch(uint8_t c, uint8_t size_mult);
void SSD1306_putc_custom(const char *c);
void SSD1306_clear(void);
void SSD1306_fill_screen(void);

#endif	/* SSD1306_H */
