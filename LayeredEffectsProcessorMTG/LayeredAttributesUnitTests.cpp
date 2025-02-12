#include "LayeredAttributesUnitTests.hpp"
#include "LayeredAttributes_v2.hpp"
#include "LayeredAttributes_v1.hpp"
#include "LayeredAttributes_v3.hpp"
#include <assert.h>
#include <iostream>

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

void LayeredAttributesUnitTests::testOperandConsolidation()
{
	attributes = std::make_unique<Implementation>();
	int basePower = 2;
	attributes->SetBaseAttribute(AttributeKey::AttributeKey_Power, basePower);
	assert(attributes->GetCurrentAttribute(AttributeKey::AttributeKey_Power) == basePower);
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
	std::cout << "testOperandConsolidation passed" << std::endl;
}

void LayeredAttributesUnitTests::testZeroReservation()
{
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