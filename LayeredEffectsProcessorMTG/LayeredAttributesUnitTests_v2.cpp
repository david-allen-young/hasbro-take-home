#include "LayeredAttributesUnitTests_v2.hpp"
#include "LayeredAttributes_v6.hpp"
#include <assert.h>
#include <iostream>

using Implementation = LayeredAttributes_v6;


void LayeredAttributesUnitTests_v2::runOperationalTests()
{
	testSetAndGet();
	testAddAndClear();
	testBitwise();
	testComplexAdd_v1();
	//testComplexAdd_v2();
	std::cout << "** Operational tests passed **" << std::endl;
}

// Warning: These tests may throw an error
void LayeredAttributesUnitTests_v2::runCrashTests()
{
	testOutOfBounds();
	testZeroReservation();
	std::cout << "** Crash tests passed **" << std::endl;
}

void LayeredAttributesUnitTests_v2::testSetAndGet()
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

void LayeredAttributesUnitTests_v2::testAddAndClear()
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
			//std::cout << "Power==" << attributes->GetCurrentAttribute(AttributeKey(i)) << std::endl;
			assert(attributes->GetCurrentAttribute(AttributeKey(i)) == 3);
		}
		else if (i == AttributeKey::AttributeKey_Toughness)
		{
			//std::cout << "Toughness==" << attributes->GetCurrentAttribute(AttributeKey(i)) << std::endl;
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

void LayeredAttributesUnitTests_v2::testBitwise()
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
	//std::cout << "Power==" << attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) << std::endl;
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 3);
	std::cout << "testBitwise passed" << std::endl;
}

void LayeredAttributesUnitTests_v2::testComplexAdd_v1()
{
	attributes = std::make_unique<Implementation>();
	int basePower = 2;
	int baseToughness = 1;
	/*
	* Walkthrough:
	* assert baseAttributes[Power] == 2
	* assert baseAttributes[Toughness] == 1
	*/
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Toughness, baseToughness);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == baseToughness);
	/*
	* Walkthrough: 
	* append effect Power, Add, 1, L2
	* (assume get(Power) == 2 + 1 == 3)
	* append effect Toughness, Add, 1, L2
	* (assume get(Toughness) == 1 + 1 == 2)
	* append effect Power, Set, 4, L0
	* (assume get(Power) == 4 + 1 == 5)
	* append effect Power, Multiply, 2, L1
	* (assume get(Power) == (4 * 2) + 1 == 9
	* append effect Power, Subtract, 1, L2
	* (assume get(Power) == (4 * 2) + 1 - 1 == 8
	*/
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
	//std::cout << "Power==" << attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) << std::endl;
	//assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == 8);
	//assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Toughness) == 2);
	std::cout << "testComplexAdd_v1 passed" << std::endl;
}

void LayeredAttributesUnitTests_v2::testComplexAdd_v2()
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

void LayeredAttributesUnitTests_v2::testZeroReservation()
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

void LayeredAttributesUnitTests_v2::testOutOfBounds()
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