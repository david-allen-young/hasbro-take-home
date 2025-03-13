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

<blockquote>
<b>Setting Base Attributes</b>
<ul><li>Attributes start at a default value (0) and can be explicitly set using SetBaseAttribute(AttributeKey, int).</li></ul>
</blockquote>

<blockquote>
<b>Applying Layered Effects</b>
<ul><li>Effects are applied through AddLayeredEffect(LayeredEffectDefinition), modifying base attributes according to their operation type (e.g., add, multiply, set).</li></ul>
</blockquote>

<blockquote>
<b>Efficient Attribute Retrieval</b>
<ul><li>The system caches computed values for quick access using GetCurrentAttribute(AttributeKey), reducing redundant recalculations.</li></ul>
</blockquote>

<blockquote>
<b>Clearing Effects</b>
<ul><li>Reset all layered effects using ClearLayeredEffects(), reverting attributes to their base values.</li></ul>
</blockquote>

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

<blockquote>
<b>Include the Header and Source Files</b>
<ul><li>Ensure LayeredAttributes_v2.hpp and LayeredAttributes_v2.cpp are added to your project.</li></ul>
</blockquote>

<blockquote>
<b>Compile with Your Project</b>
<ul><li>This system is designed to be lightweight and does not require external dependencies.</li></ul>
</blockquote>

### Potential Use Cases
<ul>
<li><b>Game Mechanics:</b> Adjust player stats dynamically (e.g., buffs, debuffs). </li>
<li><b>Card-Based Games:</b> Implement effects similar to Magic: The Gathering layer-based calculations. </li>
<li><b>Simulation & AI:</b> Modify agent properties over time with multiple influences. </li>
</ul>

### Future Improvements
<ul>
<li>Support for time-based effect expiration.</li>
<li>Optimized storage for large-scale entity handling.</li>
<li>Customizable conflict resolution strategies for layered effects.</li>
</ul>
