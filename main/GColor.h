#ifndef __GCOLOR__
#define __GCOLOR__

class GColor {
public:
	GColor(unsigned char rr = 0, unsigned char gg = 0, unsigned char bb = 0, unsigned char aa = 255);
	GColor &operator =(const GColor& color);
	unsigned long Get();
	unsigned char R();
	unsigned char G();
	unsigned char B();
	unsigned char A();
private:
	unsigned char r, g, b, a;
	unsigned long color;
protected:
		void Set();
};

extern GColor color_red;
extern GColor color_green;
extern GColor color_blue;
extern GColor color_yellow;
extern GColor color_orange;
extern GColor color_white;
extern GColor color_black;
extern GColor color_grey;
extern GColor color_pink;

extern GColor color_lightblue;
extern GColor color_darkgreen;
extern GColor color_firebrick;
extern GColor color_deeppink;
extern GColor color_cadetblue;
extern GColor color_maroon;
extern GColor color_chocolate;

#endif