/* Copyright (c) 1997-2016, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file video.c This file holds implementations of functions
  presenting output to the VGA screen. */
#include <stdint.h>

/*! Max number of columns in the VGA buffer. */
#define MAX_COLS                (80)
/*! Max number of columns in the VGA buffer. */
#define MAX_ROWS                (25)

int strlen(const char *str);
void WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, int x, int y);

struct screen_position
{
 unsigned char character; /*!< The character part of the byte tuple used for
                               each screen position. */
 unsigned char attribute; /*!< The character part of the byte tuple used for
                               each screen position. */
};
/*!< Defines a VGA text mode screen position. */

struct screen
{
 struct screen_position positions[MAX_ROWS][MAX_COLS];
 /*!< The VGA screen. It is organized as a two dimensional array. */
};
/*!< Defines a VGA text mode screen. */

/*! points to the VGA screen. */
static struct screen* const
screen_pointer = (struct screen*) 0xB8000;

int xPosition, yPosition = 0;
int textColor = 2;
int backgroundColor = 0;

void cls();

/* Clear the screen */
void cls() {
	int i;
	int h;	
	
	for (h = 0; h < MAX_COLS; h++) {
		for(i = 0; i < MAX_ROWS; i++) {
			WriteCharacter(' ', textColor, backgroundColor,h,i);
		}
	}
}

void
kprints(const char* string) {
	int x;
	int stringLength = strlen(string);
	for (x = 0; x < stringLength; x++) {
		if(string[x] == '\n') {
			yPosition++;
			xPosition = 0;
			break;	//Don't print '\n', it will become an o/0 on the screen) - cursor has already been moved.
		}
		WriteCharacter(string[x], textColor, backgroundColor,xPosition,yPosition);
		xPosition++;
	}
} 


/* Returns length of string until it find a 0. */
int strlen(const char *str) {
		int numberOfChars = 0;
		while(str[numberOfChars] != '\0') {
			++numberOfChars;
		}
		return numberOfChars;
}

void WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, int x, int y)
{
     uint16_t attrib = (backcolour << 4) | (forecolour & 0x0F);
     volatile uint16_t * where;
     where = (volatile uint16_t *)0xB8000 + (y * 80 + x) ;
     *where = c | (attrib << 8);
}

void
kprinthex(const register uint32_t value)
{
 /* Implement me! */
}
