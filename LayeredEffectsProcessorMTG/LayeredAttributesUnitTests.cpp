#include "LayeredAttributesUnitTests.hpp"
#include "LayeredAttributes_v2.hpp"
#include "LayeredAttributes_v1.hpp"
#include "LayeredAttributes_v3.hpp"
#include <assert.h>
#include <iostream>

void LayeredAttributesUnitTests::testZeroReservation()
{
	attributes = std::make_unique<Implementation>(false, false, 0);
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
	std::cout << "testZeroReservation passed" << std::endl;
}
