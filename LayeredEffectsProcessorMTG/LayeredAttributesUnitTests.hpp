#pragma once
#include "LayeredAttributes.hpp"
#include <assert.h>
#include <iostream>

class LayeredAttributesUnitTests
{
public:
	LayeredAttributesUnitTests() = default;
	// obviously, these tests are not comprehensive...
	// they're short and simple for now to ensure the deadline is met
	void runTests()
	{
		testSetAndGet();
		testClear();
	}
private:
	void testSetAndGet()
	{
		LayeredAttributes attributes;
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);
		for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
		{
			if (i == AttributeKey::AttributeKey_Power)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 2);
			}
			else if (i == AttributeKey::AttributeKey_Toughness)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 1);
			}
			else
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 0);
			}
		}
		std::cout << "testSetAndGet passed" << std::endl;
	}
	void testClear()
	{
		LayeredAttributes attributes;
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);

		// TODO: add some layered effects then check the values before clearing

		attributes.ClearLayeredEffects();
		for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
		{
			if (i == AttributeKey::AttributeKey_Power)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 2);
			}
			else if (i == AttributeKey::AttributeKey_Toughness)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 1);
			}
			else
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 0);
			}
		}
		std::cout << "testClear passed" << std::endl;
	}
};