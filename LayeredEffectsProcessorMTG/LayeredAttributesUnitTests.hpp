#pragma once
#include "LayeredAttributes_v1.hpp"
#include "LayeredAttributes_v2.hpp"
#include "LayeredAttributes_v3.hpp"
#include <assert.h>
#include <iostream>

class LayeredAttributesUnitTests
{
public:
	//using Implementation = LayeredAttributes_v1;
	//using Implementation = LayeredAttributes_v2;
	using Implementation = LayeredAttributes_v3;

	LayeredAttributesUnitTests() = default;

	// These tests should not throw an error
	// (assertions are expected to return true)
	void runOperationalTests()
	{
		testSetAndGet();
		testAddAndClear();
		testOperandConsolidation();
		testComplexAdd1();
		testComplexAdd2();
	}

	// Warning: These tests are expected to throw an error
	void runCrashTests()
	{
		testOutOfBounds();
		testZeroReservation();
	}

private:
	std::unique_ptr<ILayeredAttributes> attributes;
	void testSetAndGet();
	void testAddAndClear();
	void testOperandConsolidation();
	void testComplexAdd1()
	{
		attributes = std::make_unique<Implementation>();
		int basePower = 2;
		int baseToughness = 1;
		/*
		* Walkthrough:
		* SetBaseAttribute() results in baseAttributes[Power] == 2; recalculateCurrentAttributes() is called but layeredEffects are empty
		* SetBaseAttribute() results in baseAttributes[Toughness] == 1; recalculateCurrentAttributes() is called but layeredEffects are empty
		* At this point baseAttributes and currentAttributes should be the same with Power 2 and Toughness 1
		*/
		attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
		attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, baseToughness);
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == baseToughness);
		/*
		* Walkthrough:
		* AddLayeredEffect(Power, Add, 1, 2) calls updateCurrentAttributes() and results in currentAttributes[Power] == 2 + 1 == 3
		* AddLayeredEffect(Toughness, Add, 1, 2) calls updateCurrentAttributes() and results in currentAttributes[Toughness] == 1 + 1 == 2
		* AddLayeredEffect(Power, Set, 4, 0) calls recalculateCurrentAttributes() and results in [Power] == 4 + 1 == 5
		* AddLayeredEffect(Power, Multiply, 2, 1) calls recalculateCurrentAttributes() and results in [Power] == (4 * 2) + 1 == 9
		* AddLayeredEffect(Power, Subtract, 1, 2) call updateCurrentAttributes() and results in [Power] == 9 - 1 == 8
		* At this point Power should be 8 and Toughness should be 2
		*/
		std::vector<LayeredEffectDefinition> effects;
		effects.push_back({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		effects.push_back({ AttributeKey_Toughness, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Set, /*modifier*/basePower + 2, /*layer*/0 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
		effects.push_back({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
		for (auto& effect : effects)
		{
			attributes->AddLayeredEffect(effect);
		}
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 8);
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == 2);
		std::cout << "testComplexAdd1 passed" << std::endl;
	}
	void testComplexAdd2()
	{
		attributes = std::make_unique<Implementation>();
		int basePower = 2;
		int baseToughness = 1;
		attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
		attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, baseToughness);
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == baseToughness);
		attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 3);
		attributes->AddLayeredEffect({ AttributeKey_Toughness, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == 2);
		attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Set, /*modifier*/basePower + 2, /*layer*/0 });
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 5);
		attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 9);
		attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
		assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 8);
		for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
		{
			if (i == AttributeKey::AttributeKey_Power)
			{
				assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 8);
			}
			else if (i == AttributeKey::AttributeKey_Toughness)
			{
				assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 2);
			}
			else
			{
				assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 0);
			}
		}
		std::cout << "testComplexAdd2 passed" << std::endl;
	}

	void testZeroReservation();
	void testOutOfBounds();
};
