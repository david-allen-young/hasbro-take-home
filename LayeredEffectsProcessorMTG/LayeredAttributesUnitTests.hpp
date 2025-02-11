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
		testAddAndClear();
		testComplexAdd();
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
	void testAddAndClear()
	{
		LayeredAttributes attributes;
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);
		std::vector<LayeredEffectDefinition> effects;
		int baseModifier = 0;
		int baseLayer = 0;
		effects.push_back({ AttributeKey_Power, EffectOperation_Add, baseModifier + 1, baseLayer + 1 });
		effects.push_back({ AttributeKey_Toughness, EffectOperation_Add, baseModifier + 1, baseLayer + 1 });
		for (auto& effect : effects)
		{
			attributes.AddLayeredEffect(effect);
		}
		for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
		{
			if (i == AttributeKey::AttributeKey_Power)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 3);
			}
			else if (i == AttributeKey::AttributeKey_Toughness)
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 2);
			}
			else
			{
				assert(attributes.GetCurrentAttribute(AttributeKey(i)) == 0);
			}
		}
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
		std::cout << "testAddAndClear passed" << std::endl;
	}
	void testComplexAdd()
	{
		// TODO: add several effects with at least one layer vs. timestamp conflict to ensure recalculation works right
	}
};