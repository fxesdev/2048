#include "base.h"
#include "darkt.h"
#include "lightt.h"
//wadafuk

byte get_random(byte max)
{
	return deref(0xF00C) % max;
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

enum SPECIAL_CHARS
{
	SP_UP = 0x20,
	SP_DOWN = 0x22,
	SP_LEFT = 0x29,
	SP_RIGHT = 0x19
};

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
	for(i = 0;i<16;i++)
	{
		deref(0xE0E0+i) = deref(0xE000+i);
	}
	if(dir == 1)
	{
		for(k=0;k<4;k++)
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i,j+1));
						set_tile(i,j+1,get_tile(i,j+2));
						set_tile(i,j+2,get_tile(i,j+3));
						set_tile(i,j+3,0);
					} else if(get_tile(i,j) == get_tile(i,j+1) && n == 0) {
						set_tile(i,j,get_tile(i,j)+1);
						set_tile(i,j+1,get_tile(i,j+2));
						set_tile(i,j+2,get_tile(i,j+3));
						set_tile(i,j+3,0);
						n = 1;
					}
					//draw_tiles();
				}
				//draw_tiles();
			}
		}
	} else if(dir == 2)
	{
		for(k=0;k<4;k++)
		{
			for(i=0;i<4;i++)
			{
				for(j=3;j>0;j--)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i,j-1));
						set_tile(i,j-1,get_tile(i,j-2));
						set_tile(i,j-2,get_tile(i,j-3));
						set_tile(i,j-3,0);
					} else if(get_tile(i,j) == get_tile(i,j-1) && n == 0) {
						set_tile(i,j,get_tile(i,j)+1);
						set_tile(i,j-1,get_tile(i,j-2));
						set_tile(i,j-2,get_tile(i,j-3));
						set_tile(i,j-3,0);
						n = 1;
					}
					//draw_tiles();
				}
				//draw_tiles();
			}
		}
	} else if(dir == 3)
	{
		for(k=0;k<4;k++)
		{
			for(j=0;j<4;j++)
			{
				for(i=0;i<4;i++)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i+1,j));
						set_tile(i+1,j,get_tile(i+2,j));
						set_tile(i+2,j,get_tile(i+3,j));
						set_tile(i+3,j,0);
					} else if(get_tile(i,j) == get_tile(i+1,j) && n == 0) {
						set_tile(i,j,get_tile(i,j)+1);
						set_tile(i+1,j,get_tile(i+2,j));
						set_tile(i+2,j,get_tile(i+3,j));
						set_tile(i+3,j,0);
						n = 1;
					}
					//draw_tiles();
				}
				//draw_tiles();
			}
		}
	} else if(dir == 4)
	{
		for(k=0;k<4;k++)
		{
			for(j=0;j<4;j++)
			{
				for(i=3;i>0;i--)
				{
					if(get_tile(i,j) == 0)
					{
						set_tile(i,j,get_tile(i-1,j));
						set_tile(i-1,j,get_tile(i-2,j));
						set_tile(i-2,j,get_tile(i-3,j));
						set_tile(i-3,j,0);
					} else if(get_tile(i,j) == get_tile(i-1,j) && n == 0) {
						set_tile(i,j,get_tile(i,j)+1);
						set_tile(i-1,j,get_tile(i-2,j));
						set_tile(i-2,j,get_tile(i-3,j));
						set_tile(i-3,j,0);
						n = 1;
					}
					//draw_tiles();
				}
				//draw_tiles();
			}
		}
	}
	end:
	for(i=0;i<16;i++)
	{
		m = 0;
		for(j=0;j<16;j++)
		{
			if(deref(0xE0E0+j) != deref(0xE000+j))
			{
				m = 1;
			}
		}
		if(m == 0)
		{
			break;
		}
		if(deref(0xE000+i) == 0)
		{
			m = 1;
			break;
		} else {
			m = 0;
		}
	}
	if(m == 1 || dir == 0)
	{
		while(1)
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					if(get_tile(i,j) == 0)
					{
						//deref(0xE0E0) = get_random(2);
						if(get_random(2) == 1)
						{
							if(l < 1)
							{
								set_tile(i,j,get_random(3));
								l++;
							}
						}
					}
				}
			}
			if(l >= 1)
			{
				break;
			}
		}
	}

	draw_tiles();
}

void main() {
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

void custom_break() {
	//break
}
