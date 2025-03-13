# **Layered Effects Processor**


### **Overview**

This repository contains an implementation of a Layered Effects Processor, designed to dynamically modify and manage game entity attributes through a structured layering system. The system allows for efficient calculation of final attribute values by applying various effects such as addition, subtraction, multiplication, and bitwise operations.


### **Features**



* **Base Attribute Management**
    * Set and retrieve base attribute values for an entity.
* **Layered Effects Application**
    * Apply multiple effects to attributes, which can be modified incrementally or in bulk.
* **Incremental Updates**
    * Optimize performance by reducing redundant calculations when modifying existing effects.
* **Error Logging (Optional)	**
    * Validate attribute modifications and log issues when necessary.


### **How It Works**



* **Setting Base Attributes**
    * Attributes start at a default value (0) and can be explicitly set using SetBaseAttribute(AttributeKey, int).
* **Applying Layered Effects**
    * Effects are applied through AddLayeredEffect(LayeredEffectDefinition), modifying base attributes according to their operation type (e.g., add, multiply, set).
* **Efficient Attribute Retrieval**
    * The system caches computed values for quick access using GetCurrentAttribute(AttributeKey), reducing redundant recalculations.
* **Clearing Effects**
    * Reset all layered effects using ClearLayeredEffects(), reverting attributes to their base values.


#### 


### **Example Usage**

GameplaySimulation01.cpp
```cpp
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
    std::cout << test_creature.name << " starts as " 
              << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
              << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    // Apply Giant Growth (+3/+3) to test_creature at layer 4
    LayeredEffectDefinition giantGrowthPower{ AttributeKey_Power, EffectOperation_Add, 3, 4 };
    LayeredEffectDefinition giantGrowthToughness{ AttributeKey_Toughness, EffectOperation_Add, 3, 4 };
    test_creature.attributes.AddLayeredEffect(giantGrowthPower);
    test_creature.attributes.AddLayeredEffect(giantGrowthToughness);

    // Expected result: 2+3 = 5 / 2+3 = 5
    std::cout << "--- Turn 1: Giant Growth Applied (+3/+3) ---\n";
    std::cout << test_creature.name << " after Giant Growth: " 
              << test_creature.attributes.GetCurrentAttribute(AttributeKey_Power) << "/"
              << test_creature.attributes.GetCurrentAttribute(AttributeKey_Toughness) << "\n";

    return 0;
}
```

### 


### **Installation & Integration**



* **Include the Header and Source Files**
    * Ensure LayeredAttributes_v2.hpp and LayeredAttributes_v2.cpp are added to your project.
* **Compile with Your Project**
    * This system is designed to be lightweight and does not require external dependencies.


### **Potential Use Cases**



* **Game Mechanics**
    * Adjust player stats dynamically (e.g., buffs, debuffs).
* **Card-Based Games**
    * Implement effects similar to Magic: The Gathering layer-based calculations.
* **Simulation & AI**
    * Modify agent properties over time with multiple influences.


### **Future Improvements**



* Support for time-based effect expiration.
* Optimized storage for large-scale entity handling.
* Customizable conflict resolution strategies for layered effects.
