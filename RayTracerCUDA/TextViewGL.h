#ifndef _TEXTVIEWGL_H_
#define _TEXTVIEWGL_H_

#include "Vec3.h"
#include "GL\freeglut.h"
#include <string>

class TextViewGL{
public:
	TextViewGL();
	TextViewGL(std::string t);
	~TextViewGL();
	virtual void draw();
	void SetColor(Color col);
	void SetText(char* text);
private:
	void InitObject(Color c);
	std::string text;
	int textLength;
	float xPos;
	float yPos;
	Color col;
	static int _itemCount;
};

#endif