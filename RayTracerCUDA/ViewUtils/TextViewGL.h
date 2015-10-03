#ifndef _TEXTVIEWGL_H_
#define _TEXTVIEWGL_H_

#include "..\Utils\Vec3.h"
#include "GL\freeglut.h"
#include <string>

class TextViewGL{
public:
	TextViewGL(float xMargin, float yMargin);
	TextViewGL(float xMargin, float yMargin,std::string t);
	~TextViewGL();
	virtual void draw();
	void SetColor(Color col);
	void SetText(char* text);
	static int GetItemCount();
private:
	void InitObject(float xMargin, float yMargin,Color c);
	std::string text;
	float xPos;
	float yPos;
	Color col;
	static int _itemCount;
};

#endif