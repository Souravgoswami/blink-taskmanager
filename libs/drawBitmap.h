#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Paint the bitmaps

void drawBitmap(char *art, int col_offset = 0, int row_offset = 0) {
	char ch[2], c ;

	unsigned short len = strlen_P(art) ;
	unsigned char repeat = 0 ;
	unsigned char row = row_offset, col = col_offset ;

	for(unsigned short i = 0 ; i < len ; ++i) {
		ch[0] = pgm_read_byte_near(art + i) ;
		ch[1] = '\0' ;
		repeat = atoi(ch) ;

		if (repeat > 0) {
			++i ;
		} else {
			repeat = 1 ;
		}

		c = pgm_read_byte_near(art + i) ;

		if (c == '`') {
			for(unsigned char r = 0 ; r < repeat ; ++r) {
				display.drawPixel(col, row, SSD1306_WHITE) ;
				col++ ;
			}
		} else if (c == ' ') {
			col += repeat ;
		} else if (c == 'x') {
			col = col_offset ;
			row += repeat ;
		}
	}
}
