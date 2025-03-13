// Example program demonstrating LayeredAttributes_v2 in a Magic: The Gathering scenario
// Author: David Allen Young | Intended for Technical Review
//
// This simulation highlights how the LayeredAttributes_v2 class correctly applies and resolves game mechanics.
// It tracks power and toughness modifications using a sequence of turns where effects from different layers are added and then processed.

#include <iostream>
#include "../src/LayeredAttributes_v2.hpp"

// Simulated MTG game state
struct Card {
    std::string name;
    LayeredAttributes_v2 attributes;
    Card(const std::string& cardName, int basePower, int baseToughness)
        : name(cardName), attributes(false, 10) // Disable error logging, reserve space for 10 effects
    {
        attributes.SetBaseAttribute(AttributeKey_Power, basePower);
        attributes.SetBaseAttribute(AttributeKey_Toughness, baseToughness);
    }
};

int main() {
    // Initialize a creature with base stats
    Card test_creature("Test Creature", 2, 2); // 2/2 creature

    std::cout << "--- Turn 1: Base stats ---\n";
    std::cout << test_creature.name << " starts as " << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply Giant Growth (+3/+3) to test_creature at layer 4
    LayeredEffectDefinition giantGrowthPower{ AttributeKey_Power, EffectOperation_Add, 3, 4 };
    LayeredEffectDefinition giantGrowthToughness{ AttributeKey_Toughness, EffectOperation_Add, 3, 4 };
    test_creature.attributes.AddLayeredEffect(giantGrowthPower);
    test_creature.attributes.AddLayeredEffect(giantGrowthToughness);

    // Expected result: 2+3 = 5 / 2+3 = 5
    std::cout << "--- Turn 1: Giant Growth Applied (+3/+3) ---\n";
    std::cout << test_creature.name << " after Giant Growth: " << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply a static buff (+1/+1) at layer 5
    LayeredEffectDefinition staticBuffPower{ AttributeKey_Power, EffectOperation_Add, 1, 5 };
    LayeredEffectDefinition staticBuffToughness{ AttributeKey_Toughness, EffectOperation_Add, 1, 5 };
    test_creature.attributes.AddLayeredEffect(staticBuffPower);
    test_creature.attributes.AddLayeredEffect(staticBuffToughness);

    // Expected result: 5+1 = 6 / 5+1 = 6
    std::cout << "--- Turn 2: Static Buff Applied (+1/+1) ---\n";
    std::cout << test_creature.name << " after Static Buff: " << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply Xenagos, God of Revels effect (doubles base power at combat) at layer 3
    LayeredEffectDefinition xenagosEffectPower{ AttributeKey_Power, EffectOperation_Multiply, 2, 3 };
    test_creature.attributes.AddLayeredEffect(xenagosEffectPower);

    // Expected result: (2*2) = 4 first, then Giant Growth (+3) -> 4+3 = 7, then Static Buff (+1) -> 7+1 = 8
    std::cout << "--- Turn 3: Xenagos, God of Revels Effect Applied (base: x2/x1) ---\n";
    std::cout << test_creature.name << " after Xenagos Effect: " << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Clear all layered effects at the end of the turn
    test_creature.attributes.ClearLayeredEffects();

    // Expected result: back to base stats: 2/2
    std::cout << "--- Turn 4: Effects Worn Off ---\n";
    std::cout << test_creature.name << " after all effects expire: " << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    return 0;
}
