#define CATCH_CONFIG_RUNNER  // This tells Catch to provide a main() - only do this in one cpp file
#include "Catch/catch.h"
#include "TextView\TextViewTest.h"

int TestMain()
{
	int result = Catch::Session().run();
	return result;
}