#include "LayeredAttributesUnitTests.hpp"
#include "LayeredAttributes_v2.hpp"
#include "LayeredAttributes_v1.hpp"
#include "LayeredAttributes_v3.hpp"
#include "LayeredAttributes_v4.hpp"
//#include "LayeredAttributes_v5.hpp" // bad
#include "LayeredAttributes_v6.hpp"
#include <assert.h>
#include <iostream>

//using Implementation = LayeredAttributes_v1;
//using Implementation = LayeredAttributes_v2;
//using Implementation = LayeredAttributes_v3;
//using Implementation = LayeredAttributes_v4;
//using Implementation = LayeredAttributes_v5; // bad
using Implementation = LayeredAttributes_v6;


void LayeredAttributesUnitTests::runOperationalTests()
{
	testSetAndGet();
	testAddAndClear();
	testConsolidation();
	testBitwise();
	testComplexAdd_v1();
	testComplexAdd_v2();
	std::cout << "** Operational tests passed **" << std::endl;
}

// Warning: These tests may throw an error
void LayeredAttributesUnitTests::runCrashTests()
{
	testOutOfBounds();
	testZeroReservation();
	std::cout << "** Crash tests passed **" << std::endl;
}

void LayeredAttributesUnitTests::testSetAndGet()
{
	attributes = std::make_unique<Implementation>();
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);
	for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
	{
		if (i == AttributeKey::AttributeKey_Power)
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 2);
		}
		else if (i == AttributeKey::AttributeKey_Toughness)
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 1);
		}
		else
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 0);
		}
	}
	std::cout << "testSetAndGet passed" << std::endl;
}

void LayeredAttributesUnitTests::testAddAndClear()
{
	attributes = std::make_unique<Implementation>();
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);
	std::vector<LayeredEffectDefinition> effects;
	int baseModifier = 0;
	int baseLayer = 0;
	effects.push_back({ AttributeKey_Power, EffectOperation_Add, baseModifier + 1, baseLayer + 1 });
	effects.push_back({ AttributeKey_Toughness, EffectOperation_Add, baseModifier + 1, baseLayer + 1 });
	for (auto& effect : effects)
	{
		attributes->AddLayeredEffect(effect);
	}
	for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
	{
		if (i == AttributeKey::AttributeKey_Power)
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 3);
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
	attributes->ClearLayeredEffects();
	for (int i = AttributeKey::AttributeKey_NotAssessed; i <= AttributeKey::AttributeKey_Controller; ++i)
	{
		if (i == AttributeKey::AttributeKey_Power)
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 2);
		}
		else if (i == AttributeKey::AttributeKey_Toughness)
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 1);
		}
		else
		{
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 0);
		}
	}
	std::cout << "testAddAndClear passed" << std::endl;
}

void LayeredAttributesUnitTests::testConsolidation()
{
	attributes = std::make_unique<Implementation>();
	int basePower = 2;
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower - 3);
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower + 3);
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower * 4 + 3);
	basePower = 5;
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) != basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower * 4 + 3);
	std::cout << "testConsolidation passed" << std::endl;
}

void LayeredAttributesUnitTests::testBitwise()
{
	attributes = std::make_unique<Implementation>();
	int basePower = 3;
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseAnd, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseAnd, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseAnd, /*modifier*/1, /*layer*/2 });
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 1);
	attributes->ClearLayeredEffects();
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseOr, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseOr, /*modifier*/1, /*layer*/2 });
	attributes->AddLayeredEffect({ AttributeKey_Power, EffectOperation_BitwiseOr, /*modifier*/1, /*layer*/2 });
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 3);
	std::cout << "testBitwise passed" << std::endl;
}

void LayeredAttributesUnitTests::testComplexAdd_v1()
{
	attributes = std::make_unique<Implementation>();
	int basePower = 2;
	int baseToughness = 1;
	/*
	* Walkthrough: (assumes Implementation = _v1)
	* SetBaseAttribute() results in baseAttributes[Power] == 2; recalculateCurrentAttributes() is called but layeredEffects are empty
	* SetBaseAttribute() results in baseAttributes[Toughness] == 1; recalculateCurrentAttributes() is called but layeredEffects are empty
	* At this point baseAttributes and currentAttributes should be the same with Power 2 and Toughness 1
	*/
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, baseToughness);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == baseToughness);
	/*
	* Walkthrough: (assumes Implementation = _v1)
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
	std::cout << "testComplexAdd_v1 passed" << std::endl;
}

void LayeredAttributesUnitTests::testComplexAdd_v2()
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
	std::cout << "testComplexAdd_v2 passed" << std::endl;
}

void LayeredAttributesUnitTests::testZeroReservation()
{
	std::cout << "testZeroReservation now expects to NOT throw an error..." << std::endl;
	// this block will throw an error without proper implementation
	{
		size_t initialVectorCapacity = 0;
		attributes = std::make_unique<Implementation>(false, false, initialVectorCapacity);
	}

	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, 2);
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, 1);
	std::vector<LayeredEffectDefinition> effects;
	effects.push_back({ AttributeKey_Power, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	effects.push_back({ AttributeKey_Toughness, EffectOperation_Add, /*modifier*/1, /*layer*/2 });
	effects.push_back({ AttributeKey_Power, EffectOperation_Set, /*modifier*/4, /*layer*/0 });
	effects.push_back({ AttributeKey_Power, EffectOperation_Multiply, /*modifier*/2, /*layer*/1 });
	effects.push_back({ AttributeKey_Power, EffectOperation_Subtract, /*modifier*/1, /*layer*/2 });
	for (auto& effect : effects)
	{
		attributes->AddLayeredEffect(effect);
	}

	// One must step through the call stack using the debugger
	// to verify that the capacity handling is correctly implemented
	std::cout << "testZeroReservation did not throw an error" << std::endl;
}

void LayeredAttributesUnitTests::testOutOfBounds()
{
	std::cout << "testOutOfBounds expects to throw an error..." << std::endl;
	attributes = std::make_unique<Implementation>(true, true);
	try
	{
		attributes->SetBaseAttribute(AttributeKey(-1), 2);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Expected exception caught: " << e.what() << '\n';
	}
	try
	{
		attributes->SetBaseAttribute(AttributeKey(100), 1);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Expected exception caught: " << e.what() << '\n';
	}
}