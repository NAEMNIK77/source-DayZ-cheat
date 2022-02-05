#include "GColor.h"

GColor color_red(255, 0, 0);
GColor color_green(0, 255, 0);
GColor color_blue(0, 0, 255);
GColor color_yellow(255, 255, 0);
GColor color_orange(255, 100, 0);
GColor color_white(255, 255, 255);
GColor color_black(0, 0, 0);
GColor color_grey(139, 137, 137);
GColor color_pink(255, 0, 255);

GColor color_lightblue(30, 144, 255);
GColor color_darkgreen(0, 180, 0);
GColor color_firebrick(178, 34, 34);
GColor color_deeppink(255, 20, 147);
GColor color_cadetblue(152, 245, 255);
GColor color_maroon(139, 28, 98);
GColor color_chocolate(139, 69, 19);


GColor::GColor(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa) :
	r(rr), g(gg), b(bb), a(aa)
{
	color = 0;
	Set();
}

GColor &GColor::operator =(const GColor& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	Set();

	return *this;
}

void GColor::Set()
{
	color = 0;
	color |= (r << 0);
	color |= (g << 8);
	color |= (b << 16);
	color |= (a << 24);
}

unsigned long GColor::Get()
{
	return color;
}

unsigned char GColor::R()
{
	return r;
}

unsigned char GColor::G()
{
	return g;
}

unsigned char GColor::B()
{
	return b;
}

unsigned char GColor::A()
{
	return a;
}