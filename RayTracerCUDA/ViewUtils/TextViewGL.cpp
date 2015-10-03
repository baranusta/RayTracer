#include "TextViewGL.h"
#include <iostream>


int TextViewGL::_itemCount = 0;

int TextViewGL::GetItemCount(){
	return _itemCount;
}

TextViewGL::TextViewGL(float xMargin,float yMargin)
{
	text = "FPS: 1";
	this->InitObject(xMargin,yMargin,Color(0,0,0));
}

void TextViewGL::SetColor(Color col)
{
	this->col = col;
}

TextViewGL::TextViewGL(float xMargin, float yMargin, std::string t)
{
	text = t;
	this->InitObject(xMargin, yMargin, Color(0, 0, 0));
}

TextViewGL::~TextViewGL()
{
}

void TextViewGL::InitObject(float xMargin, float yMargin, Color col)
{
	xPos = xMargin;
	yPos = yMargin - 0.1f*_itemCount;
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