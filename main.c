#include "base.h"
#include "darkt.h"
#include "lightt.h"
#include "dlose.h"
#include "llose.h"
#include "lwin.h"
#include "dwin.h"
//hi

word get_random(word max)
{
	return Timer0Counter % max;
}

byte get_random_tile()
{
	byte res = 1;
	if(get_random(4) == 1)
	{
		res = 2;
	}
	return res;
}

void clear_buffer()
{
	word i;
	for(i=0;i<4096;i++)
	{
		deref(0x9000+i) = 0;
	}
	for(i=0;i<16;i++)
	{
		deref(0xE000+i) = 0;
	}
}

void draw_tile(word tile,word x,word y)
{
	//drawbitmap(tiles + (tile*32),x+(y*32),2,14,buffer);
	word i = 0;
	word j = 0;
	word offset = (x*2) + ((y<<5)*16);
	tile = tile << 4;
	for(i = 0; i < 0x10; i++)
    {
        j++;
        if((j & 0x1F) == 0x02)
        {
            j+=31;
        }
        *((word *)(0x9020 + offset + j)) = *((word *)(lightt + tile + i));
        *((word *)(0x9820 + offset + j)) = *((word *)(darkt + tile + i));
    }
}

void delay(ushort after_ticks)
{
	if ((FCON & 2) != 0)
		FCON &= 0xfd;
	__DI();
	Timer0Interval = after_ticks;
	Timer0Counter = 0;
	Timer0Control = 0x0101;
	InterruptPending_W0 = 0;
	StopAcceptor = 0x50;
	StopAcceptor = 0xa0;
	StopControl = 2;
	__asm("nop");
	__asm("nop");
	__EI();
}

void drawbitmap(const byte* cocainer, word offset, byte width, byte height,byte buffer)
{
	word x;
	word y;
	word i = 0;
	word j = 0;
	for(y = 0; y < offset; y++)
	{
		++j;
		if((j&0x1f) == 0x18)
		{
			j += 0x8;
		}
	}

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if(buffer==1)
			{
				deref(0x9000+j) |= cocainer[i];
			} else {
				deref(0x9800+j) |= cocainer[i];
			}
			++j;
			if((j&0x1f) == 0x18)
			{
				j += 0x8;
			}
			++i;
		}
		j += 32-width;
		if((j&0x1f) == 0x18)
		{
			j += 0x8;
		}
	}
}

void render() {
	word i = 0;
	for(i=0;i<2048;i++)
	{
		deref(0xf037) = 0;
		deref(0xf820+i) = deref(0x9020+i);
		deref(0xf037) = 4;
		deref(0xf820+i) = deref(0x9820+i);
	}
}


enum BUTTON
{
	B_0 = 0xb,
	B_1 = 0x3f,
	B_2 = 0x37,
	B_3 = 0x2f,
	B_4 = 0x3e,
	B_5 = 0x36,
	B_6 = 0x2e,
	B_7 = 0x3d,
	B_8 = 0x35,
	B_9 = 0x2d,

	B_A = 0x3c,
	B_B = 0x34,
	B_C = 0x2c,
	B_D = 0x24,
	B_E = 0x1c,
	B_F = 0x14,

	B_G = 0x3d,
	B_H = 0x35,
	B_I = 0x2d,
	B_J = 0x25,
	B_K = 0x1d,

	B_L = 0x3e,
	B_M = 0x36,
	B_N = 0x2e,
	B_O = 0x26,
	B_P = 0x1e,

	B_Q = 0x3f,
	B_R = 0x37,
	B_S = 0x2f,
	B_T = 0x27,
	B_U = 0x1f,

	B_V = 0xb,
	B_W = 0xc,
	B_X = 0xd,
	B_Y = 0xe,
	B_Z = 0xf,



	BUTTON_COUNT = 0x40
};

enum SPECIAL_CHARS
{
	SP_EXE = '\n',
	SP_TAB = '\t',
	SP_SPACE = ' ',
	SP_BACKSPACE = '\b',

	SP_HOME = 0x2A,
	SP_END = 0x1A,
	SP_YES = 0x3A,
	SP_NO = 0x12,
	SP_OK = 0x21,
	SP_UP = 0x20,
	SP_DOWN = 0x22,
	SP_LEFT = 0x29,
	SP_RIGHT = 0x19,
	SP_PLUS = 0x10,
	SP_MINUS = 0x11,
	SP_ALT = 0x31,
	SP_ABC = 0x30,
	SP_ESC = 0x39,
	SP_SELECTLEFT,
	SP_SELECTRIGHT,
	SP_PASTE,
	SP_COPY,

	SPECIAL_MAX
};

const byte button_to_char[64] = {
	  0,        0,       0,    0,    0,    0,   0,    0,
	  0,        0,       0,  '0',  ' ',  ',', '.', '\n',
SP_PLUS, SP_MINUS,   SP_NO,  ']',  ')',    0,   0,    0,
	  0, SP_RIGHT,  SP_END,  '[',  '(', '\b', '#',  '%',
  SP_UP,    SP_OK, SP_DOWN, '\'',  '/',  '!', '@',  '$',
	  0,  SP_LEFT, SP_HOME,  ';', '\\',  '9', '6',  '3',
 SP_ABC,   SP_ALT,     '*',  '-',  '=',  '8', '5',  '2',
	  0,   SP_ESC,  SP_YES, '\t',  '`',  '7', '4',  '1',
};

const byte button_to_char_abc[64] = {
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0, 'V', 'W', 'X', 'Y', 'Z',
	0,   0,   0,   0, 'F',   0,   0,   0,
	0,   0,   0,   0, 'E', 'K', 'P', 'U',
	0,   0,   0,   0, 'D', 'J', 'O', 'T',
	0,   0,   0,   0, 'C', 'I', 'N', 'S',
	0,   0,   0,   0, 'B', 'H', 'M', 'R',
	0,   0,   0,   0, 'A', 'G', 'L', 'Q',
};

const byte button_to_char_alt[64] = {
	0,              0,        0,   0,   0,   0,   0,   0,
	0,              0,        0,   0,   0, '<', '>',   0,
	0,              0, SP_PASTE, '}', '^',   0,   0,   0,
	0, SP_SELECTRIGHT,        0, '{', '*',   0,   0,   0,
	0,              0,        0, '"', '?',   0,   0,   0,
	0,  SP_SELECTLEFT,        0, ':', '|',   0,   0,   0,
	0,              0,      '&', '_', '+',   0,   0,   0,
	0,              0,  SP_COPY,   0, '~',   0,   0,   0,
};

/*word byteToTwoBytes(unsigned char input) {
    byte highNibble = (input >> 4) & 0x0F;
    byte lowNibble = input & 0x0F;

    byte result = 0;

    // Convert the high nibble to its ASCII representation
    byte highChar = highNibble + '0';
    if (highChar > '9') {
        highChar += 7; // Adjust for hexadecimal A-F
    }

    // Convert the low nibble to its ASCII representation
    lowChar = lowNibble + '0';
    if (lowChar > '9') {
        lowChar += 7; // Adjust for hexadecimal A-F
    }

    // Combine the two ASCII characters into a word (16-bit)
    result = (highChar << 8) | lowChar;

    return result;
}
*/

byte lastbutton = 0xff;
byte CheckButtons()
{
	byte x;
	byte y;
	byte i = 0;
	for(x = 0x80; x != 0; x = x >> 1)
	{
		deref(0xf046) = x;
		for(y = 0x80; y != 0; y = y >> 1)
		{
			if((deref(0xf040) & y) == 0)
			{
				if(i != lastbutton)
				{
					lastbutton = i;
					return i;
				}
				return 0xff;
			}
			++i;
		}
	}
	lastbutton = 0x50;
	return 0xff;
}

void main()
{
	word x = 1;
	word y = 1;
	word i = 0;
	byte brush = 1;
	byte menu_option = 0;
	byte save = 0;
	deref(0xf0c7) = 0x01;
/*
	deref(0xf00a) = 0x81;
	deref(0xf030) = 0x04;
	deref(0xf031) = 0x15;
	deref(0xf032) = 0x12;
	deref(0xf033) = 0x03;
*/
	deref(0xf034) = 0x06;

//	deref(0xf035) = 0x17;
	deref(0xf036) = 0x08;
/*
	deref(0xf037) = 0x04;
	deref(0xf039) = 0;

	deref(0xf03d) = 0x07;

	deref(0xF4C6) = 0x50;
	deref(0xF4C7) = 0x88;
	deref(0xF4C8) = 0x01;
	deref(0xF4C9) = 0x49;
	deref(0xF4CA) = 0x88;
	deref(0xF4CB) = 0x12;
	deref(0xF400) = 0xFF;
	deref(0xF48A) = 0x69;
	deref(0xF48B) = 0x01;
	deref(0xF414) = 0x14;
	deref(0xF415) = 0x01;
	deref(0xF221) = 0x7F;
	deref(0xF223) = 0x7F;
	deref(0xF0D0) = 0x03;
	deref(0xF0D1) = 0x06;
	deref(0xF000) = 0x04;
	deref(0xF010) = 0x22;
	deref(0xF018) = 0x03;
	deref(0xF03D) = 0x07;
	deref(0xF04A) = 0x07;
	deref(0xF04C) = 0x07;
	*/
	//very sloppy text editor
	//My emu has alot of junk on screen, clearing both screen buffers helps.
	clear_buffer();
	move_tiles(0);

	while(1)
	{
		byte pb = CheckButtons();
		//display_tiles();
		if(pb==0xFF)
		{
			delay(100);
			continue;
		} else if(pb==SP_UP) {
			move_tiles(1);
		} else if(pb==SP_DOWN) {
			move_tiles(2);
		} else if(pb==SP_LEFT) {
			move_tiles(3);
		} else if(pb==SP_RIGHT) {
			move_tiles(4);
		}
		//display_tiles();
	}
}

byte get_tile(byte x, byte y)
{
	byte tile = 0;
	if(x >= 0 && x <= 3 && y >= 0 && y <= 3)
	{
		tile = deref(0xE000 + x + (y<<2));
	}
	return tile;
}

void set_tile(byte x, byte y,byte tile)
{
	if(x >= 0 && x <= 3 && y >= 0 && y <= 3)
	{
		deref(0xE000 + x + (y<<2)) = tile;
	}
}

void draw_tiles()
{
	byte i = 0;
	byte j = 0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			draw_tile(get_tile(i,j),i,j);
		}
	}
	render();
}

void move_tiles(byte dir)
{
	byte i = 0;
	byte j = 0;
	byte k = 0;
	byte l = 0;
	byte m = 0;
	byte n = 0;
	byte fs = 0;
	byte o = 0;
	for(i = 0;i<16;i++)
	{
		deref(0xE0E0+i) = deref(0xE000+i);
	}
	check_moves();
	if(dir == 1)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				for(k=0;k<4;k++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i,j+1));
						set_tile(i,j+1,get_tile(i,j+2));
						set_tile(i,j+2,get_tile(i,j+3));
						set_tile(i,j+3,0);
					}
				}
			}
			for(j=0;j<4;j++)
			{
				if(get_tile(i,j) == get_tile(i,j+1) && get_tile(i,j) != 0) {
					set_tile(i,j,get_tile(i,j+1)+1);
					set_tile(i,j+1,get_tile(i,j+2));
					set_tile(i,j+2,get_tile(i,j+3));
					set_tile(i,j+3,0);
				}
			}
		}
	} else if(dir == 2)
	{
		for(i=0;i<4;i++)
		{
			for(j=3;j>0;j--)
			{
				for(k=0;k<4;k++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i,j-1));
						set_tile(i,j-1,get_tile(i,j-2));
						set_tile(i,j-2,get_tile(i,j-3));
						set_tile(i,j-3,0);
					}
				}
			}
			for(j=3;j>0;j--)
			{
				if(get_tile(i,j) == get_tile(i,j-1) && get_tile(i,j) != 0) {
					set_tile(i,j,get_tile(i,j-1)+1);
					set_tile(i,j-1,get_tile(i,j-2));
					set_tile(i,j-2,get_tile(i,j-3));
					set_tile(i,j-3,0);
				}
			}
		}
	} else if(dir == 3)
	{
		for(j=0;j<4;j++)
		{
			for(i=0;i<4;i++)
			{
				for(k=0;k<4;k++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i+1,j));
						set_tile(i+1,j,get_tile(i+2,j));
						set_tile(i+2,j,get_tile(i+3,j));
						set_tile(i+3,j,0);
					}
				}
			}
			for(i=0;i<4;i++)
			{
				if(get_tile(i,j) == get_tile(i+1,j) && get_tile(i,j) != 0) {
					set_tile(i,j,get_tile(i+1,j)+1);
					set_tile(i+1,j,get_tile(i+2,j));
					set_tile(i+2,j,get_tile(i+3,j));
					set_tile(i+3,j,0);
				}
			}
		}
	} else if(dir == 4)
	{
		for(j=0;j<4;j++)
		{
			for(i=3;i>0;i--)
			{
				for(k=0;k<4;k++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i-1,j));
						set_tile(i-1,j,get_tile(i-2,j));
						set_tile(i-2,j,get_tile(i-3,j));
						set_tile(i-3,j,0);
					}
				}
			}
			for(i=3;i>0;i--)
			{
				if(get_tile(i,j) == get_tile(i-1,j) && get_tile(i,j) != 0) {
					set_tile(i,j,get_tile(i-1,j)+1);
					set_tile(i-1,j,get_tile(i-2,j));
					set_tile(i-2,j,get_tile(i-3,j));
					set_tile(i-3,j,0);
				}
			}
		}
	} else {
		goto spawn;
	}
	for(i=0;i<16;i++)
	{
		if(deref(0xE000+i) != deref(0xE0E0+i))
		{
			goto spawn;
		}
	}
	goto end;
	spawn:
	while(1)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(get_tile(i,j) == 0)
				{
					if(get_random(15) == 1)
					{
						set_tile(i,j,get_random_tile());
						goto end;
					}
				}
			}
		}
	}

	end:
	draw_tiles();
	check_moves();

	//draw_tiles();
}

void check_moves()
{
	byte i = 0;
	for(i=0;i<15;i++)
	{
		if(deref(0xE000+i) == 11)
		{
			win_screen();
		}
	}
	for(i=0;i<15;i++)
	{
		if((deref(0xE000+i) == 0 || (deref(0xE000+i) == deref(0xE000+i+1)) || deref(0xE000+i) == deref(0xE000+i+4)))
		{
			return;
		}
	}
	death_screen();
}

void render_dark(word addr)
{
    word i = 0;
    word j = 0;
    for(i = 0; i < 0x0600; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0x9800 + j)) = *((word *)(addr + i));
    }
}

void render_light(word addr)
{
    word i = 0;
    word j = 0;
    for(i = 0; i < 0x0600; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0x9000 + j)) = *((word *)(addr + i));
    }
}

void death_screen()
{
	delay(0x4001);
	while(1)
	{
		render_dark(dlose);
		render_light(llose);
		render();

	}
}

void win_screen()
{
	delay(0x4001);
	while(1)
	{
		render_dark(dwin);
		render_light(lwin);
		render();

	}
}

void custom_break()
{

}
