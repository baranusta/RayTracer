#ifndef _FRAMETEXTVIEWGL_H_
#define _FRAMETEXTVIEWGL_H_

#include "Vec3.h"
#include "TextViewGL.h"
#include <stdio.h>


typedef Vec3 Color;

class FrameTextViewGL :public TextViewGL
{
public:
	void draw()
	{
		this->setFrameText();
		TextViewGL::draw();
	}
private:

	void setFrameText()
	{
		frame++;
		timeK = glutGet(GLUT_ELAPSED_TIME);
		char frameString[50];
		if (timeK - timebase > 1000) {
			sprintf_s(frameString, "FPS:%4.2f",
				frame*1000.0 / (timeK - timebase));
			timebase = timeK;
			frame = 0;
			TextViewGL::SetText(frameString);
		}
	}
	int frame = 0;
	int timeK;
	int timebase = 0;
};


#endif