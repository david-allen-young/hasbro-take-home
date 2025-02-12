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
		LayeredAttributes attributes;
		int basePower = 2;
		int baseToughness = 1;
		/*
		* Walkthrough:
		* SetBaseAttribute() results in baseAttributes[Power] == 2; recalculateCurrentAttributes() is called but layeredEffects are empty
		* SetBaseAttribute() results in baseAttributes[Toughness] == 1; recalculateCurrentAttributes() is called but layeredEffects are empty
		* At this point baseAttributes and currentAttributes should be the same with Power 2 and Toughness 1
		*/
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
		attributes.SetBaseAttribute(AttributeKey::AttributeKey_Toughness, baseToughness);
		assert(attributes.GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
		assert(attributes.GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == baseToughness);
		/*
		* Walkthrough:
		* AddLayeredEffect(Power, Add, 1, 2) calls updateCurrentAttributes() and results in currentAttributes[Power] == 2 + 1 == 3
		* AddLayeredEffect(Toughness, Add, 1, 2) calls updateCurrentAttributes() and results in currentAttributes[Toughness] == 1 + 1 == 2
		* AddLayeredEffect(Power, Set, 4, 0) calls recalculateCurrentAttributes() and results in [Power] == 4 + 1 == 5
		* AddLayeredEffect(Power, Multiply, 2, 1) calls "eek a BUG!!!"
		* 
		* 
		*/
		std::vector<LayeredEffectDefinition> effects;
		effects.push_back({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		effects.push_back({ AttributeKey_Toughness, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Set, /*modifier*/basePower + 2, /*layer*/0 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });

		for (auto& effect : effects)
		{
			attributes.AddLayeredEffect(effect);
		}
	}
};