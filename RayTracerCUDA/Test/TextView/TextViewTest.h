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
	
	~TextViewTest();
	void ZeroItem_TotalCountTest();
	void OneTextViewAdd_TotalCountTest();
	void OneFrameViewAdd_TotalCountTest();
	void RandomNumberItem_RandomTypeTVAdd_TotalCountTest();

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

TextViewTest::~TextViewTest()
{
	TextViewGL::ResetItemCount();
}

void TextViewTest::ZeroItem_TotalCountTest()
{
	REQUIRE(TextViewGL::GetItemCount() == 0);
}

void TextViewTest::OneTextViewAdd_TotalCountTest()
{
	float yMargin = 0.90f;
	AddView(0, yMargin, 1, TextViewTest::TextView);
	REQUIRE(TextViewGL::GetItemCount() == 1);
}

void TextViewTest::OneFrameViewAdd_TotalCountTest()
{
	float yMargin = 0.90f;
	AddView(0, yMargin, 1, TextViewTest::FrameView);
	REQUIRE(TextViewGL::GetItemCount() == 1);
}

void TextViewTest::RandomNumberItem_RandomTypeTVAdd_TotalCountTest()
{
	RandomGenerator s;
	float yMargin = 0.90f;
	int totalNumberOfTV = s.getInt();
	int numberOfText = s.getInt(totalNumberOfTV);
	int numberOfFrame = totalNumberOfTV - numberOfText;
	AddView(0, yMargin, numberOfText, TextViewTest::TextView);
	AddView(0, yMargin, numberOfFrame, TextViewTest::FrameView);

	REQUIRE(TextViewGL::GetItemCount() == totalNumberOfTV);
}


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

TEST_CASE("TextView test (totalCount)", "[tv]") {
	SECTION("created zero textviews") {
		TextViewTest t;
		t.ZeroItem_TotalCountTest();
	}
	SECTION("created one textview") {
		TextViewTest t;
		t.OneTextViewAdd_TotalCountTest();
	}
	SECTION("created one frame text view") {
		TextViewTest t;
		t.OneFrameViewAdd_TotalCountTest();
	}
	SECTION("created random number of textviews") {
		TextViewTest t;
		t.RandomNumberItem_RandomTypeTVAdd_TotalCountTest();
	}
	printf("TextView tests\n");
}
#endif