# Layered Attributes System

### Overview
This repository contains an implementation of a <b>Layered Attributes System</b>, designed to dynamically modify and manage game entity attributes through a structured layering system. The system allows for efficient calculation of final attribute values by applying various effects such as addition, subtraction, multiplication, and bitwise operations.

### Features
<ul>
<li><b>Base Attribute Management:</b> Set and retrieve base attribute values for an entity.</li>
<li><b>Layered Effects Application:</b> Apply multiple effects to attributes, which can be modified incrementally or in bulk.</li>
<li><b>Incremental Updates:</b> Optimize performance by reducing redundant calculations when modifying existing effects.</li>
<li><b>Error Logging (Optional):</b> Validate attribute modifications and log issues when necessary.</li>
</ul>

### How It Works

#### Setting Base Attributes
<blockquote>Attributes start at a default value (0) and can be explicitly set using SetBaseAttribute(AttributeKey, int).</blockquote>

#### Applying Layered Effects
<blockquote>Effects are applied through AddLayeredEffect(LayeredEffectDefinition), modifying base attributes according to their operation type (e.g., add, multiply, set).</blockquote>

#### Efficient Attribute Retrieval
<blockquote>The system caches computed values for quick access using GetCurrentAttribute(AttributeKey), reducing redundant recalculations.</blockquote>

#### Clearing Effects
<blockquote>Reset all layered effects using ClearLayeredEffects(), reverting attributes to their base values.</blockquote>

#### Example Usage
<pre>
cpp <br>
  
LayeredAttributes_v2 attributes(true, 10); <br>

// Set base attribute <br>
attributes.SetBaseAttribute(AttributeKey_Power, 5); <br>

// Apply a layered effect (e.g., +3 Power) <br>
LayeredEffectDefinition effectDef; <br>
effectDef.Attribute = AttributeKey_Power; <br>
effectDef.Operation = EffectOperation_Add; <br>
effectDef.Modification = 3; <br>
attributes.AddLayeredEffect(effectDef); <br>

// Retrieve final computed value <br>
int power = attributes.GetCurrentAttribute(AttributeKey_Power); // Expected: 8 <br>
</pre>

### Installation & Integration

#### Include the Header and Source Files
Ensure LayeredAttributes_v2.hpp and LayeredAttributes_v2.cpp are added to your project.

#### Compile with Your Project
This system is designed to be lightweight and does not require external dependencies.

### Potential Use Cases

<b>Game Mechanics:</b> Adjust player stats dynamically (e.g., buffs, debuffs). <br>
<b>Card-Based Games:</b> Implement effects similar to Magic: The Gathering layer-based calculations. <br>
<b>Simulation & AI:</b> Modify agent properties over time with multiple influences. <br>

### Future Improvements
#### Support for time-based effect expiration.
#### Optimized storage for large-scale entity handling.
#### Customizable conflict resolution strategies for layered effects.
