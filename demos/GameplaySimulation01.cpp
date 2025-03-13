// Example program demonstrating LayeredAttributes_v2 in a Magic: The Gathering scenario
// Author: David Allen Young | Intended for Technical Review
//
// This simulation highlights how the LayeredAttributes_v2 class correctly applies and resolves game mechanics.
// It tracks power and toughness modifications using a sequence of turns between two players.

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
    // Initialize two creatures with base stats
    Card creature1("Serra Angel", 4, 4); // 4/4 Flying, Vigilance
    Card creature2("Test Creature", 2, 2); // 2/2 creature

    std::cout << "--- Turn 1: Base stats ---\n";
    std::cout << creature1.name << " starts as " << creature1.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature1.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";
    std::cout << creature2.name << " starts as " << creature2.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature2.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply Giant Growth (+3/+3) to creature2 at layer 4
    LayeredEffectDefinition giantGrowthPower{ AttributeKey_Power, EffectOperation_Add, 3, 4 };
    LayeredEffectDefinition giantGrowthToughness{ AttributeKey_Toughness, EffectOperation_Add, 3, 4 };
    creature2.attributes.AddLayeredEffect(giantGrowthPower);
    creature2.attributes.AddLayeredEffect(giantGrowthToughness);

    // Expected result: 2+3 = 5 / 2+3 = 5
    std::cout << "--- Turn 1: Giant Growth Applied ---\n";
    std::cout << creature2.name << " after Giant Growth: " << creature2.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature2.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply a static buff (+1/+1) at layer 5
    LayeredEffectDefinition staticBuffPower{ AttributeKey_Power, EffectOperation_Add, 1, 5 };
    LayeredEffectDefinition staticBuffToughness{ AttributeKey_Toughness, EffectOperation_Add, 1, 5 };
    creature2.attributes.AddLayeredEffect(staticBuffPower);
    creature2.attributes.AddLayeredEffect(staticBuffToughness);

    // Expected result: 5+1 = 6 / 5+1 = 6
    std::cout << "--- Turn 2: Static Buff Applied ---\n";
    std::cout << creature2.name << " after Static Buff: " << creature2.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature2.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply Xenagos, God of Revels effect (doubles base power at combat) at layer 3
    LayeredEffectDefinition xenagosEffectPower{ AttributeKey_Power, EffectOperation_Multiply, 2, 3 };
    creature2.attributes.AddLayeredEffect(xenagosEffectPower);

    // Expected result: (2*2) = 4 first, then Giant Growth (+3) -> 4+3 = 7, then Static Buff (+1) -> 7+1 = 8
    std::cout << "--- Turn 3: Xenagos, God of Revels Effect Applied ---\n";
    std::cout << creature2.name << " after Xenagos Effect: " << creature2.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature2.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Clear all layered effects at the end of the turn
    creature2.attributes.ClearLayeredEffects();

    // Expected result: back to base stats: 2/2
    std::cout << "--- Turn 4: Effects Worn Off ---\n";
    std::cout << creature2.name << " after all effects expire: " << creature2.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
        << creature2.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    return 0;
}
