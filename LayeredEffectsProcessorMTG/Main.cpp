#include "LayeredAttributesUnitTests.hpp"

int main()
{
	LayeredAttributesUnitTests tests;
	tests.runOperationalTests();
	tests.runCrashTests(); 
	return 0;
}


