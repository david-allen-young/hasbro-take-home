//#include "LayeredAttributesUnitTests.hpp"
#include "LayeredAttributesUnitTests_v2.hpp"

//
#include <vector>
#include <algorithm>
#include <iostream>

int main()
{
	LayeredAttributesUnitTests_v2 tests;
	tests.runOperationalTests();
	tests.runCrashTests(); 
	return 0;
}


