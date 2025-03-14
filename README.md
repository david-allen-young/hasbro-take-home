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


* **Data Structures**
    ```
	mutable std::unordered_map<AttributeKey, int> baseAttributes;
	mutable std::unordered_map<AttributeKey, std::vector<Effect>> effects;
	mutable std::unordered_map<AttributeKey, bool> attributeDirty;
	mutable std::unordered_map<AttributeKey, int> cache;
    ```
    * LayeredAttributes_v2::**baseAttributes** stores the base attributes as they arrive (refer to **::SetBaseAttribute()** method.)
    * LayeredAttributes_v2::**effects** stores the layered effects as they arrive (refer to **::AddLayeredEffect()** method.)
    * LayeredAttributes_v2::**attributeDirty** tracks the validity of the cached (current) attribute value.
    * LayeredAttributes_v2::**cache** stores the cached result of applying the **std::vector<Effect>** to a given attribute.
    * Each container is preceded by the **mutable** keyword to allow for lazy evaluation during calls to **::GetCurrentAttribute()** from the client.
    * This is necessary because the pure **virtual** method **ILayeredAttributes::GetCurrentAttribute()** is specified as **const** in the interface file.
 
* **Initialization**
   ```cpp
   LayeredAttributes_v2::LayeredAttributes_v2(bool errorLoggingEnabled, size_t reservationSize)
   	: errorLoggingEnabled(errorLoggingEnabled), reservationSize(std::max(1ULL, reservationSize))
   {
   	baseAttributes.reserve(reservationSize);
   	cache.reserve(reservationSize);
   }
   ```
   * The constructor allows for optional error logging and **pre-allocates memory** according to a configurable **reservation** block size.
      
* **Setting Base Attributes**
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
   * Attributes start at a default value (0) and can be explicitly set using the **::SetBaseAttribute()** method.
   * If error logging is enabled and the **AttributeKey** fails validation, write an error to the log.
   * Set **baseAttributes[attribute]** to the given value.
   * Update **attributeDirty[attribute]** to indicate that this attribute's cached value is no longer valid.
      
* **Applying Layered Effects**
   ```cpp
   void LayeredAttributes_v2::AddLayeredEffect(LayeredEffectDefinition effectDef)
   {
   	if (errorLoggingEnabled && !isValidAttributeKey(effectDef.Attribute))
   	{
   		logError(effectDef.Attribute);
   	}
   	size_t timestamp = getNextTimestamp();
   	auto effect = Effect(effectDef, timestamp);
   	AttributeKey attribute = effect.getAttribute();
   	if (updateIncrementally(effect))
   	{
   		updateAttribute(effect, cache[attribute]);
   	}
   	else
   	{
   		if (effects[attribute].size() + 1 > reservationSize)
   		{
   			effects[attribute].reserve(effects.size() + reservationSize);
   		}
   		auto it = std::lower_bound(effects[attribute].begin(), effects[attribute].end(), effect, EffectComparator());
   		effects[attribute].insert(it, effect);
   		attributeDirty[attribute] = true;
   	}
   }
   ```
    * Effects are applied through **::AddLayeredEffect()**, modifying base attributes according to their operation type (e.g., add, multiply, set).
    * If error logging is enabled and the **AttributeKey** fails validation, write an error to the log.
    * Generate a unique **timestamp** for this effect.
    * Attempt to update the cache while **avoiding a full** recalculation.
    * If the **incremental update** cannot proceed, insert this effect in priority order **{layer, timestamp}** using the **custom comparator**.
    * Mark the attribute **dirty** so that it will be **recalculated** upon retrieval.
 
* **Custom Comparator**
    ```cpp
	struct EffectComparator
	{
		bool operator()(const Effect& a, const Effect& b) const
		{
			if (a.getLayer() != b.getLayer())
			{
				return a.getLayer() < b.getLayer();
			}
			return a.getTimestamp() < b.getTimestamp();
		}
	};
    ```
    * LayeredAttributes_v2::**EffectComparator** is used to keep the effects container sorted by **{layer, timestamp}** during insertions.
      
* **Incremental Update**
    ```cpp
	bool LayeredAttributes_v2::updateIncrementally(const Effect& effect)
	{
		AttributeKey attribute = effect.getAttribute();
		if (effects[attribute].empty())
		{
			return false;
		}
		auto& oldEffect = effects[attribute].back();
		if (oldEffect.getLayer() != effect.getLayer())
		{
			return false;
		}
		auto oldOperation = oldEffect.getOperation();
		if (oldOperation == EffectOperation::EffectOperation_BitwiseXor)
		{
			return false;
		}
		auto operation = effect.getOperation();
		if (operation == oldOperation)
		{
			int updatedModification = oldEffect.getModification();
			if (operation == EffectOperation_Set)
			{
				updatedModification = effect.getModification();
			}
			else if (operation == EffectOperation_Add || operation == EffectOperation_Subtract)
			{
				updatedModification += effect.getModification();
			}
			else if (operation == EffectOperation_Multiply)
			{
				updatedModification *= effect.getModification();
			}
			else if (operation == EffectOperation_BitwiseOr)
			{
				updatedModification |= effect.getModification();
			}
			else if (operation == EffectOperation_BitwiseAnd)
			{
				updatedModification &= effect.getModification();
			}
			else
			{
			}
			oldEffect.updateModification(updatedModification);
		}
		else
		{
			effects[attribute].push_back(effect);
		}
		return true;
	}
    ```
    * LayeredAttributes_v2::**updateIncrementally()** attempts to **merge** or **append** the most recent effect addition.
    * It does this to **avoid a recalculation** of all the modifications during attribute retrieval.
    * First it determines if the incoming effect is in the **same layer** as the last effect added for the given attribute.
    * Second it determines if the incoming effect has the **same operation** as the last effect.
    * If the layers and operations are **both the same**, the system **merges** the modifications and updates the last effect.
    * If **only the layers** are the same, the system **appends** this affect to the end of the container.
    * If **neither** of the above are true, the function **returns false** so the caller knows to mark the attribute **dirty**.
 
* **Efficient Attribute Retrieval**
   ```cpp
   int LayeredAttributes_v2::GetCurrentAttribute(AttributeKey attribute) const
   {
   	if (errorLoggingEnabled && !isValidAttributeKey(attribute))
   	{
   		logError(attribute);
   	}
   	auto it = cache.find(attribute);
   	if (it == cache.end())
   	{
   		it = cache.insert({ attribute, calculateAttribute(attribute) }).first;
   	}
   	else if (attributeDirty[attribute])
   	{
   		it->second = calculateAttribute(attribute);
   		attributeDirty[attribute] = false;
   	}
   	else
   	{
   		// do nothing
   	}
   	return it->second;
   }
   ```
    * The system caches computed values for quick access using **::GetCurrentAttribute()**, reducing redundant recalculations.
    * If error logging is enabled and the **AttributeKey** fails validation, write an error to the log.
    * If the attribute key has not been added to the cache yet, create it.
    * If the attribute is newly created or was marked dirty, perform a full calculation.
    * If a calculation was performed, store the result as the new cached value.
    * Return the cached value.
      
* **Full Calculation**
    ```cpp
	int LayeredAttributes_v2::calculateAttribute(AttributeKey attribute) const
	{
		// the map defaults to zero if no key is present
		int result = baseAttributes[attribute];
	
		for (auto& effect : effects[attribute])
		{
			updateAttribute(effect, result);
		}
	
		return result;
	}
	
	void LayeredAttributes_v2::updateAttribute(const Effect& effect, int& result) const
	{
		if (effect.getOperation() == EffectOperation_Set)
		{
			result = effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_Add)
		{
			result += effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_Subtract)
		{
			result -= effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_Multiply)
		{
			result *= effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_BitwiseOr)
		{
			result |= effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_BitwiseAnd)
		{
			result &= effect.getModification();
		}
		else if (effect.getOperation() == EffectOperation_BitwiseXor)
		{
			result ^= effect.getModification();
		}
		else
		{
			// do nothing
		}
	}
    ```
    * LayeredAttributes_v2::**calculateAttribute()** is called by **::GetCurrentAttribute()** if the cache is **dirty** or does **not exist**.
    * LayeredAttributes_v2::**updateAttribute()** is called by **::calculateAttribute()** for each effect, **applying** the modifications to the **base**.
 
* **Clearing Effects**
   ```cpp
   void LayeredAttributes_v2::ClearLayeredEffects()
   {
   	effects = {};
   	cache = {};
   	attributeDirty = {};
   }
   ```
   * Reset all layered effects using **::ClearLayeredEffects()**, reverting attributes to their base values.


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
    * Ensure **LayeredAttributes_v2.hpp** and **LayeredAttributes_v2.cpp** are added to your project.
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
