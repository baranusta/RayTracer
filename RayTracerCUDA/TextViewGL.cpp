#include "TextViewGL.h"
#include <iostream>


int TextViewGL::_itemCount = 0;

TextViewGL::TextViewGL()
{
	text = "FPS: 1";
	this->InitObject(Color(0,0,0));
}

void TextViewGL::SetColor(Color col)
{
	this->col = col;
}

TextViewGL::TextViewGL(std::string t)
{
	text = t;
	this->InitObject(Color(0, 0, 0));
}

TextViewGL::~TextViewGL()
{
	std::cout << "lol";
}

void TextViewGL::InitObject(Color col)
{
	xPos = -0.95f;
	yPos = +0.88f - 0.1f*_itemCount;
	_itemCount ++;
	this->col = col;
}

void TextViewGL::SetText(char* c)
{
	std::string str(c);
	text = str;
}

void TextViewGL::draw()
{
	glColor3f(col.getX(), col.getY(), col.getZ());
	glRasterPos2f(xPos, yPos);

	for (char k: text)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, k);
	}
}