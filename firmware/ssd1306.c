/* 
 * File:   ssd1306.h
 * Author: hammond
 *
 * Created on 11 May 2019, 4:32 PM
 */

#include "ssd1306.h"
#include "i2c.h"
#include "global.h"

#include <stdbool.h>
    
//------------------------------ Definitions ---------------------------------//

#define SSD1306_I2C_ADDRESS   0x3C

#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT            64

#define SSD1306_SETCONTRAST          0x81
#define SSD1306_DISPLAYALLON_RESUME  0xA4
#define SSD1306_DISPLAYALLON         0xA5
#define SSD1306_NORMALDISPLAY        0xA6
#define SSD1306_INVERTDISPLAY_       0xA7
#define SSD1306_DISPLAYOFF           0xAE
#define SSD1306_DISPLAYON            0xAF
#define SSD1306_SETDISPLAYOFFSET     0xD3
#define SSD1306_SETCOMPINS           0xDA
#define SSD1306_SETVCOMDETECT        0xDB
#define SSD1306_SETDISPLAYCLOCKDIV   0xD5
#define SSD1306_SETPRECHARGE         0xD9
#define SSD1306_SETMULTIPLEX         0xA8
#define SSD1306_SETLOWCOLUMN         0x00
#define SSD1306_SETHIGHCOLUMN        0x10
#define SSD1306_SETSTARTLINE         0x40
#define SSD1306_MEMORYMODE           0x20
#define SSD1306_MEMORYMODE_PAGE      0x02
#define SSD1306_MEMORYMODE_HORIZ     0x00
#define SSD1306_MEMORYMODE_VERT      0x01
    
#define SSD1306_COLUMNADDR           0x21
#define SSD1306_PAGEADDR             0x22
#define SSD1306_COMSCANINC           0xC0
#define SSD1306_COMSCANDEC           0xC8
#define SSD1306_SEGREMAP             0xA0
#define SSD1306_CHARGEPUMP           0x8D
#define SSD1306_EXTERNALVCC          0x01
#define SSD1306_SWITCHCAPVCC         0x02

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                      0x2F
#define SSD1306_DEACTIVATE_SCROLL                    0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A

#define ENABLE_FANCY_STUFF 0

static uint8_t _vccstate, x_pos = 1, y_pos = 1;

//--------------------------------------------------------------------------//

static char wrap = true;

const char Font[255] = {
0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x5F, 0x00, 0x00,
0x00, 0x07, 0x00, 0x07, 0x00,
0x14, 0x7F, 0x14, 0x7F, 0x14,
0x24, 0x2A, 0x7F, 0x2A, 0x12,
0x23, 0x13, 0x08, 0x64, 0x62,
0x36, 0x49, 0x56, 0x20, 0x50,
0x00, 0x08, 0x07, 0x03, 0x00,
0x00, 0x1C, 0x22, 0x41, 0x00,
0x00, 0x41, 0x22, 0x1C, 0x00,
0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
0x08, 0x08, 0x3E, 0x08, 0x08,
0x00, 0x80, 0x70, 0x30, 0x00,
0x08, 0x08, 0x08, 0x08, 0x08,
0x00, 0x00, 0x60, 0x60, 0x00,
0x20, 0x10, 0x08, 0x04, 0x02,
0x3E, 0x51, 0x49, 0x45, 0x3E,
0x00, 0x42, 0x7F, 0x40, 0x00,
0x72, 0x49, 0x49, 0x49, 0x46,
0x21, 0x41, 0x49, 0x4D, 0x33,
0x18, 0x14, 0x12, 0x7F, 0x10,
0x27, 0x45, 0x45, 0x45, 0x39,
0x3C, 0x4A, 0x49, 0x49, 0x31,
0x41, 0x21, 0x11, 0x09, 0x07,
0x36, 0x49, 0x49, 0x49, 0x36,
0x46, 0x49, 0x49, 0x29, 0x1E,
0x00, 0x00, 0x14, 0x00, 0x00,
0x00, 0x40, 0x34, 0x00, 0x00,
0x00, 0x08, 0x14, 0x22, 0x41,
0x14, 0x14, 0x14, 0x14, 0x14,
0x00, 0x41, 0x22, 0x14, 0x08,
0x02, 0x01, 0x59, 0x09, 0x06,
0x3E, 0x41, 0x5D, 0x59, 0x4E,
0x7C, 0x12, 0x11, 0x12, 0x7C,
0x7F, 0x49, 0x49, 0x49, 0x36,
0x3E, 0x41, 0x41, 0x41, 0x22,
0x7F, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x49, 0x49, 0x49, 0x41,
0x7F, 0x09, 0x09, 0x09, 0x01,
0x3E, 0x41, 0x41, 0x51, 0x73,
0x7F, 0x08, 0x08, 0x08, 0x7F,
0x00, 0x41, 0x7F, 0x41, 0x00,
0x20, 0x40, 0x41, 0x3F, 0x01,
0x7F, 0x08, 0x14, 0x22, 0x41,
0x7F, 0x40, 0x40, 0x40, 0x40,
0x7F, 0x02, 0x1C, 0x02, 0x7F,
0x7F, 0x04, 0x08, 0x10, 0x7F,
0x3E, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x09, 0x09, 0x09, 0x06,
0x3E, 0x41, 0x51, 0x21, 0x5E,
0x7F, 0x09, 0x19, 0x29, 0x46
};
const char Font2[220] = {
0x26, 0x49, 0x49, 0x49, 0x32,
0x03, 0x01, 0x7F, 0x01, 0x03,
0x3F, 0x40, 0x40, 0x40, 0x3F,
0x1F, 0x20, 0x40, 0x20, 0x1F,
0x3F, 0x40, 0x38, 0x40, 0x3F,
0x63, 0x14, 0x08, 0x14, 0x63,
0x03, 0x04, 0x78, 0x04, 0x03,
0x61, 0x59, 0x49, 0x4D, 0x43,
0x00, 0x7F, 0x41, 0x41, 0x41,
0x02, 0x04, 0x08, 0x10, 0x20,
0x00, 0x41, 0x41, 0x41, 0x7F,
0x04, 0x02, 0x01, 0x02, 0x04,
0x40, 0x40, 0x40, 0x40, 0x40,
0x00, 0x03, 0x07, 0x08, 0x00,
0x20, 0x54, 0x54, 0x78, 0x40,
0x7F, 0x28, 0x44, 0x44, 0x38,
0x38, 0x44, 0x44, 0x44, 0x28,
0x38, 0x44, 0x44, 0x28, 0x7F,
0x38, 0x54, 0x54, 0x54, 0x18,
0x00, 0x08, 0x7E, 0x09, 0x02,
0x18, 0xA4, 0xA4, 0x9C, 0x78,
0x7F, 0x08, 0x04, 0x04, 0x78,
0x00, 0x44, 0x7D, 0x40, 0x00,
0x20, 0x40, 0x40, 0x3D, 0x00,
0x7F, 0x10, 0x28, 0x44, 0x00,
0x00, 0x41, 0x7F, 0x40, 0x00,
0x7C, 0x04, 0x78, 0x04, 0x78,
0x7C, 0x08, 0x04, 0x04, 0x78,
0x38, 0x44, 0x44, 0x44, 0x38,
0xFC, 0x18, 0x24, 0x24, 0x18,
0x18, 0x24, 0x24, 0x18, 0xFC,
0x7C, 0x08, 0x04, 0x04, 0x08,
0x48, 0x54, 0x54, 0x54, 0x24,
0x04, 0x04, 0x3F, 0x44, 0x24,
0x3C, 0x40, 0x40, 0x20, 0x7C,
0x1C, 0x20, 0x40, 0x20, 0x1C,
0x3C, 0x40, 0x30, 0x40, 0x3C,
0x44, 0x28, 0x10, 0x28, 0x44,
0x4C, 0x90, 0x90, 0x90, 0x7C,
0x44, 0x64, 0x54, 0x4C, 0x44,
0x00, 0x08, 0x36, 0x41, 0x00,
0x00, 0x00, 0x77, 0x00, 0x00,
0x00, 0x41, 0x36, 0x08, 0x00,
0x02, 0x01, 0x02, 0x04, 0x02
};


static void ssd1306_command(uint8_t c) {
    uint8_t data[2] = {0x00, c};   // Co = 0, D/C = 0
    
    i2c_send(SSD1306_I2C_ADDRESS, data, 2);
    
}

/*uint8_t vccstate = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = SSD1306_I2C_ADDRESS*/
void SSD1306_init(void) {
  _vccstate = SSD1306_SWITCHCAPVCC;

  // Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(SSD1306_MEMORYMODE_HORIZ);    // 0x0 act like ks0108
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(SSD1306_LCDHEIGHT - 1);
  ssd1306_command(SSD1306_COMSCANDEC);
  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80
  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (_vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x22); }
  else
    { ssd1306_command(0xF1); }
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x30);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(0xFF);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  if (_vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x10); }
  else
    { ssd1306_command(0x14); }
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

}

#if ENABLE_FANCY_STUFF

void SSD1306_start_scroll_right(uint8_t start, uint8_t stop) {
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);  // start page
  ssd1306_command(0X00);
  ssd1306_command(stop);   // end page
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_start_scroll_left(uint8_t start, uint8_t stop) {
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_start_scroll_diag_right(uint8_t start, uint8_t stop) {
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_start_scroll_diag_left(uint8_t start, uint8_t stop) {
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_stop_scroll(void) {
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void SSD1306_dim(uint8_t contrast) {
  if (_vccstate == SSD1306_EXTERNALVCC)
    contrast = contrast > 0x9F ? 0x9F : contrast;
  else
    contrast = contrast > 0xCF ? 0xCF : contrast;

  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

#endif 

void SSD1306_set_text_wrap(char w) {
  wrap = w;
}

void SSD1306_invert_display(char i) {
  if (i)
    ssd1306_command(SSD1306_INVERTDISPLAY_);
  else
    ssd1306_command(SSD1306_NORMALDISPLAY);
}

void SSD1306_gotoXY(uint8_t x, uint8_t y) {
  if((x > 21) || y > 8)
    return;
  x_pos = x;
  y_pos = y;
}

void SSD1306_putc(uint8_t c) {
  uint8_t font_c;
  
  
  if((c < ' ') || (c > '~')) {
      c = '?';      
  }
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(6 * (x_pos - 1));
  ssd1306_command(6 * (x_pos - 1) + 4); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(y_pos - 1); // Page start address (0 = reset)
  ssd1306_command(y_pos - 1); // Page end address
  

  uint8_t c_data[6];

  
  c_data[0]= 0x40;
  
  for(uint8_t i = 0; i < 5; i++ ) {
    uint8_t offs;
    if(c == '#') {
        font_c = 0xFF;
    } else if(c < 'S') {
        offs = (c - 32)*5;
        font_c = Font[offs + i];
    } else {
        offs = (c - 'S')*5;
        font_c = Font2[offs + i];
    }
    
    c_data[i+1] = font_c;
  }

  i2c_send(SSD1306_I2C_ADDRESS, c_data, 6);
  
  x_pos = x_pos % 21 + 1;
  if (wrap && (x_pos == 1))
    y_pos = y_pos % 8 + 1;

}


void SSD1306_putc_stretch(uint8_t c, uint8_t size_mult) {
  uint8_t font_c;
  
  ssd1306_command(SSD1306_MEMORYMODE);         // 0x20
  ssd1306_command(SSD1306_MEMORYMODE_VERT);    // 0x0 act like ks0108
  
  if((c < ' ') || (c > '~'))
    c = '?';
  
  
  if(size_mult < 1 || size_mult > 4) {
      size_mult = 1;
  }
  
  for(uint8_t i=0; i<(size_mult - 1); i++) {
      if(x_pos % size_mult != 1) {
        x_pos++;
      } else {
          break;
      }
  }
  
  if(x_pos > 21) {
      x_pos = 1;
      if(wrap) {
          y_pos+=size_mult;
      }
  }
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(6 * (x_pos - 1));
  ssd1306_command(6 * (x_pos - 1) + (size_mult*5 - 1)); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(y_pos - 1); // Page start address (0 = reset)
  ssd1306_command((y_pos - 1)+(size_mult - 1)); // Page end address
  
  
  i2c_send_start();
  i2c_send_raw_byte(SSD1306_I2C_ADDRESS << 1);
  i2c_send_raw_byte(0x40);

  
  for(uint8_t i = 0; i < 5; i++ ) {
    
    uint8_t offs;
    uint8_t b[4] = {0};
    
    if(c < 'S') {
        offs = (c - 32)*5;
        font_c = Font[offs + i];
    } else {
        offs = (c - 'S')*5;
        font_c = Font2[offs + i];
    } 
    
    if(size_mult == 1) {
        //no need to stretch, 1 byte == 1 byte
        i2c_send_raw_byte(font_c);
    } else {
        //multiply out the bits by the size (i.e. stretch them across the bytes
        // of b)
        for(uint8_t i = 0; i < 8; i++) {
            if(font_c & (1 << i)) {
                for(uint8_t p = 0; p < size_mult; p++) {
                
                    uint8_t pos = (i*size_mult + p) % 8;
                    uint8_t bn = (i*size_mult + p) / 8;
                    
                    b[bn] |= (1 << pos);
                    
                }
            }
        }
        
        //for each of the new stretched bytes, print them across rows and columns
        for(uint8_t i = 0; i < size_mult; i++) {
            for(uint8_t bn = 0; bn < size_mult; bn++) {
                i2c_send_raw_byte(b[bn]);
            }
        }
    } 
  }
  i2c_send_stop();

  x_pos += size_mult;
  if(x_pos > 21) {
      x_pos = 1;
      if(wrap) {
          y_pos+=size_mult;
      }
  }

  
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(SSD1306_MEMORYMODE_HORIZ);    // 0x0 act like ks0108
  
}


void SSD1306_putc_custom(const char *c) {
  uint8_t line;
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(6 * (x_pos - 1));
  ssd1306_command(6 * (x_pos - 1) + 4); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(y_pos - 1); // Page start address (0 = reset)
  ssd1306_command(y_pos - 1); // Page end address
  
  uint8_t c_data[6];
  c_data[0] = 0x40;
  
  for(uint8_t i = 0; i < 5; i++ ) {
    line = c[i];
    c_data[i+1] = line;
  }
  i2c_send(SSD1306_I2C_ADDRESS, c_data, 6);

  x_pos = x_pos % 21 + 1;
  if (wrap && (x_pos == 1))
    y_pos = y_pos % 8 + 1;

}

void SSD1306_clear(void) {

  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);    // Column start address
  ssd1306_command(127);  // Column end address

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0);   // Page start address (0 = reset)
  ssd1306_command(7);   // Page end address

  i2c_send_many_bytes(SSD1306_I2C_ADDRESS, 0x40, 0x00, SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8);
  
}

void SSD1306_fill_screen(void) {

  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);    // Column start address
  ssd1306_command(127);  // Column end address

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0);   // Page start address (0 = reset)
  ssd1306_command(7);   // Page end address

  i2c_send_many_bytes(SSD1306_I2C_ADDRESS, 0x40, 0xFF, SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8);


}
