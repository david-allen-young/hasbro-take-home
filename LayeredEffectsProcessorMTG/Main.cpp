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

	//sandbox stuff
	//std::vector<std::tuple<int, int, int, int>> effects;
	//effects.push_back({ /*layer*/7, /*timestamp*/1, /*attribute*/ 1, /*modifier*/ 1});
	//effects.push_back({ /*layer*/6, /*timestamp*/2, /*attribute*/ 1, /*modifier*/ 1});
	//std::sort(effects.begin(), effects.end());
	//int layer = std::get<0>(effects[0]);
	//std::cout << "first layer == " << layer << std::endl;

	return 0;
}


