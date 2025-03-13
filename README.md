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
* **Error Logging (Optional)**
    * Validate attribute modifications and log issues when necessary.


### **How It Works**


* **Initialization**
    * The constructor allows for optional error logging and pre-allocates memory according to a configurable reservation block size.
   ```cpp
   LayeredAttributes_v2::LayeredAttributes_v2(bool errorLoggingEnabled, size_t reservationSize)
   	: errorLoggingEnabled(errorLoggingEnabled), reservationSize(std::max(1ULL, reservationSize))
   {
   	baseAttributes.reserve(reservationSize);
   	cache.reserve(reservationSize);
   }
   ```
      
* **Setting Base Attributes**
    * Attributes start at a default value (0) and can be explicitly set using SetBaseAttribute(AttributeKey, int).
       * If error logging is enabled and the AttributeKey fails validation, write an error to the log.
       * Set baseAttributes[attribute] to the given value.
       * Update attributeDirty[attribute] to indicate that this attribute's cached value is no longer valid.
   ```cpp
   void LayeredAttributes_v2::SetBaseAttribute(AttributeKey attribute, int value)
   {
      if (errorLoggingEnabled && !isValidAttributeKey(attribute))
      {
         logError(attribute);
      }
      baseAttributes[attribute] = value;
      attributeDirty[attribute] = true;
   }
   ```
      
* **Applying Layered Effects**
    * Effects are applied through AddLayeredEffect(LayeredEffectDefinition), modifying base attributes according to their operation type (e.g., add, multiply, set).
* **Efficient Attribute Retrieval**
    * The system caches computed values for quick access using GetCurrentAttribute(AttributeKey), reducing redundant recalculations.
* **Clearing Effects**
    * Reset all layered effects using ClearLayeredEffects(), reverting attributes to their base values.


#### 


### **Example Usage**

* GameplaySimulation01.cpp
```cpp
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
```

* Output
```
--- Turn 1: Base stats ---
Test Creature starts as 2/2
--- Turn 1: Giant Growth Applied (+3/+3) ---
Test Creature after Giant Growth: 5/5
--- Turn 2: Static Buff Applied (+1/+1) ---
Test Creature after Static Buff: 6/6
--- Turn 3: Xenagos, God of Revels Effect Applied (base: x2/x1) ---
Test Creature after Xenagos Effect: 8/6
--- Turn 4: Effects Worn Off ---
Test Creature after all effects expire: 2/2
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
