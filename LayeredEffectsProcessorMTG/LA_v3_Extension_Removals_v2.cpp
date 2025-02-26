#include "LA_v3_Extension_Removals_v2.hpp"
#include <stdexcept>

LA_v3_Extension_Removals_v2::LA_v3_Extension_Removals_v2(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{
	baseAttributes.fill(0);
	currentAttributes.fill(0);
	highestLayers.fill(std::numeric_limits<int>::min());
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LA_v3_Extension_Removals_v2::SetBaseAttribute(AttributeKey attribute, int value)
{
	if (attributeInBounds(attribute))
	{
		baseAttributes[attribute] = value;
		calculateAndCache(attribute);
	}
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LA_v3_Extension_Removals_v2::GetCurrentAttribute(AttributeKey attribute) const
{
	if (attributeInBounds(attribute) == false)
	{
		return std::numeric_limits<int>::min();
	}
	return currentAttributes[attribute];
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LA_v3_Extension_Removals_v2::AddLayeredEffect(LayeredEffectDefinition effect)
{
	if (attributeInBounds(effect.Attribute) == false)
	{
		return;
	}
	if (attributeModifiers[effect.Attribute].count(effect.Layer) == 0)
	{
		attributeModifiers[effect.Attribute].insert({ effect.Layer, std::vector<Mod>() });
	}
	auto& vecMods = attributeModifiers[effect.Attribute][effect.Layer];
	if (vecMods.capacity() < vecMods.size() + 1)
	{
		vecMods.reserve(attributeModifiers[effect.Attribute][effect.Layer].size() + reservationSize);
	}
	Mod mod = { effect.Operation, effect.Modification };
	if (!vecMods.empty() && vecMods.back().operation == effect.Operation)
	{
		// consolidate operands where possible
		// so there are less mods to iterate through
		// during calls to ::calculateAndCache()
		if (mod.operation == EffectOperation_Set)
		{
			vecMods.back().modifier = mod.modifier;
		}
		else if (mod.operation == EffectOperation_Add || mod.operation == EffectOperation_Subtract)
		{
			vecMods.back().modifier += mod.modifier;
		}
		else if (mod.operation == EffectOperation_Multiply)
		{
			vecMods.back().modifier *= mod.modifier;
		}
		else if (mod.operation == EffectOperation_BitwiseOr)
		{
			vecMods.back().modifier |= mod.modifier;
		}
		else if (mod.operation == EffectOperation_BitwiseAnd)
		{
			vecMods.back().modifier &= mod.modifier;
		}
		// NB: decided against flattening EffectOperation_BitwiseXor (for now)
		// because I was getting different results depending on input (even vs. odd)
	}
	else
	{
		vecMods.push_back(mod);
	}
	highestLayers[effect.Attribute] = std::max(highestLayers[effect.Attribute], effect.Layer);
	if (effect.Layer < highestLayers[effect.Attribute])
	{
		calculateAndCache(effect.Attribute);
	}
	else
	{
		updateCache(effect.Attribute, mod);
	}
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LA_v3_Extension_Removals_v2::ClearLayeredEffects()
{
	attributeModifiers = {};
	currentAttributes = baseAttributes;
	highestLayers.fill(std::numeric_limits<int>::min());
}

void LA_v3_Extension_Removals_v2::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}

void LA_v3_Extension_Removals_v2::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

bool LA_v3_Extension_Removals_v2::attributeInBounds(AttributeKey attribute) const
{
	bool outOfBounds = attribute < 0 || attribute >= NumAttributes;
	if (outOfBounds && errorLoggingEnabled)
	{
		logError(attribute);
	}
	if (outOfBounds && errorHandlingEnabled)
	{
		throw std::out_of_range("Attribute out of range");
	}
	return !outOfBounds;
}

void LA_v3_Extension_Removals_v2::calculateAndCache(AttributeKey attribute)
{
	int result = baseAttributes[attribute];
	for (const auto& [layer, mods] : attributeModifiers[attribute])
	{
		for (const auto& mod : mods)
		{
			if (mod.operation == EffectOperation::EffectOperation_Set)
			{
				result = mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_Add)
			{
				result += mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_Subtract)
			{
				result -= mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_Multiply)
			{
				result *= mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_BitwiseOr)
			{
				result |= mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_BitwiseAnd)
			{
				result &= mod.modifier;
			}
			else if (mod.operation == EffectOperation::EffectOperation_BitwiseXor)
			{
				result ^= mod.modifier;
			}
		}
	}
	currentAttributes[attribute] = result;
}

void LA_v3_Extension_Removals_v2::updateCache(AttributeKey attribute, const Mod& mod)
{
	if (mod.operation == EffectOperation::EffectOperation_Set)
	{
		currentAttributes[attribute] = mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_Add)
	{
		currentAttributes[attribute] += mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_Subtract)
	{
		currentAttributes[attribute] -= mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_Multiply)
	{
		currentAttributes[attribute] *= mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_BitwiseOr)
	{
		currentAttributes[attribute] |= mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_BitwiseAnd)
	{
		currentAttributes[attribute] &= mod.modifier;
	}
	else if (mod.operation == EffectOperation::EffectOperation_BitwiseXor)
	{
		currentAttributes[attribute] ^= mod.modifier;
	}
}
