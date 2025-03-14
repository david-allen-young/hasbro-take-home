#include "LayeredAttributes_v5.hpp"
#include <stdexcept>

/*NB: Do not use this implementation. Aborting in favor a new apprach. See v6...*/

LayeredAttributes_v5::LayeredAttributes_v5(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{
	//baseAttributes.fill(0);
	//currentAttributes.fill(0);
	//attributeDirty.fill(false);
	//highestLayers.fill(std::numeric_limits<int>::min());
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes_v5::SetBaseAttribute(AttributeKey attribute, int value)
{
	//if (attributeInBounds(attribute))
	{
		baseAttributes[attribute] = value;
		attributeDirty[attribute] = true;
	}
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v5::GetCurrentAttribute(AttributeKey attribute) const
{
	//if (attributeInBounds(attribute) == false)
	//{
	//	return std::numeric_limits<int>::min();
	//}
	//if(attributeDirty[attribute])
	auto itDirty = attributeDirty.find(attribute);
	if (itDirty != attributeDirty.end() && itDirty->second)
	{
		calculateAndCache(attribute);
		attributeDirty[attribute] = false;
	}
	//return currentAttributes[attribute];
	auto itCurrent = currentAttributes.find(attribute);
	return itCurrent != currentAttributes.end() ? itCurrent->second : std::numeric_limits<int>::min();
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v5::AddLayeredEffect(LayeredEffectDefinition effect)
{
	//if (attributeInBounds(effect.Attribute) == false)
	if(attributeModifiers.find(effect.Attribute) == attributeModifiers.end())
	{
		return;
	}
	if (attributeModifiers[effect.Attribute].first.count(effect.Layer) == 0)
	{
		attributeModifiers[effect.Attribute].first.insert({ effect.Layer, std::vector<Mod>() });
	}
	auto& vecMods = attributeModifiers[effect.Attribute].first[effect.Layer];
	if (vecMods.capacity() < vecMods.size() + 1)
	{
		vecMods.reserve(attributeModifiers[effect.Attribute].first[effect.Layer].size() + reservationSize);
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
	//if (effect.Layer < highestLayers[effect.Attribute])
	//{
	//	calculateAndCache(effect.Attribute);
	//}
	//else
	//{
	//	updateCache(effect.Attribute, mod);
	//}
	if (effect.Layer < highestLayers[effect.Attribute])
	{
		attributeDirty[effect.Attribute] = true;
	}
	else
	{
		updateCache(effect.Attribute, mod);
	}
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes_v5::ClearLayeredEffects()
{
	//attributeModifiers = {};
	attributeModifiers.clear();
	currentAttributes = baseAttributes;
	//highestLayers.fill(std::numeric_limits<int>::min());
	highestLayers.clear();
	//for (int attribute = 0; attribute < NumAttributes; attribute++)
	//{
	//	attributeDirty[attribute] = false;
	//}
	attributeDirty.clear();
}

void LayeredAttributes_v5::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}

void LayeredAttributes_v5::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

bool LayeredAttributes_v5::attributeInBounds([[maybe_unused]]AttributeKey attribute) const
{
	//bool outOfBounds = attribute < 0 || attribute >= NumAttributes;
	//if (outOfBounds && errorLoggingEnabled)
	//{
	//	logError(attribute);
	//}
	//if (outOfBounds && errorHandlingEnabled)
	//{
	//	throw std::out_of_range("Attribute out of range");
	//}
	//return !outOfBounds;
	return false;
}

void LayeredAttributes_v5::calculateAndCache(AttributeKey attribute) const
{
	int result = baseAttributes[attribute];
	for (const auto& [layer, mods] : attributeModifiers[attribute].first)
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

void LayeredAttributes_v5::updateCache(AttributeKey attribute, const Mod& mod) const
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
