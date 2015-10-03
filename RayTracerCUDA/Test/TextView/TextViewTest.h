#ifndef _TEXTVIEWTEST_H_
#define _TEXTVIEWTEST_H_

#include "../Catch/catch.h"
#include "../../ViewUtils/FrameTextViewGL.h"
#include "../../ViewUtils/TextViewGL.h"
#include "../../Utils/RandomGenerator.h"
#include <vector>

class TextViewTest{
public:
	enum ViewType {TextView,FrameView};
	void AddView(float xMargin, float yMargin, int number,ViewType type);
private:
	template <class view>
	void addView(float xMargin, float yMargin, int number)
	{
		for (int i = 0; i < number; i++)
		{
			view tv(xMargin, yMargin);
		}
	}
};


void TextViewTest::AddView(float xMargin, float yMargin, int number, ViewType type)
{
		switch (type)
		{
		case TextView:
			addView<TextViewGL>(xMargin, yMargin,number);
			break;
		case FrameView:
			addView<FrameTextViewGL>(xMargin, yMargin, number);
			break;
		}
}

TEST_CASE("TextView test", "[tv]") {
	TextViewTest t;
	SECTION("created number of textviews") {
		int iter = -1;
		int total = 0;
		float yMargin = 0.90f;
		RandomGenerator s;
		while (++iter < 100){
			total += iter;
			int numberOfText = s.getInt(iter);	
			int numberOfFrame = iter - numberOfText;
			t.AddView(0, yMargin, numberOfText, TextViewTest::TextView);
			t.AddView(0, yMargin, numberOfFrame, TextViewTest::FrameView);
			REQUIRE(TextViewGL::GetItemCount() == total);
		}
	}
	printf("TextView tests\n");
}
#endif