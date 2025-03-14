#include "LayeredAttributes_v2.hpp"
#include <stdexcept>

LayeredAttributes_v2::LayeredAttributes_v2(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{
	baseAttributes.fill(0);
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes_v2::SetBaseAttribute(AttributeKey attribute, int value)
{
	if (attributeInBounds(attribute))
	{
		baseAttributes[attribute] = value;
	}
	else
	{
		baseAttributes[attribute] = std::numeric_limits<int>().min();
	}
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v2::GetCurrentAttribute(AttributeKey attribute) const
{
	if (attributeInBounds(attribute) == false)
	{
		return std::numeric_limits<int>::min();
	}
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
	return result;
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v2::AddLayeredEffect(LayeredEffectDefinition effect)
{
	if (attributeModifiers[effect.Attribute].count(effect.Layer) == 0)
	{
		attributeModifiers[effect.Attribute].insert({ effect.Layer, std::vector<Mod>() });
	}
	auto& vecMods = attributeModifiers[effect.Attribute][effect.Layer];
	if(vecMods.capacity() < vecMods.size() + 1)
	{
		vecMods.reserve(attributeModifiers[effect.Attribute][effect.Layer].size() + reservationSize);
	}
	Mod mod = { effect.Operation, effect.Modification };
	consolidateOperands(effect, vecMods, mod);
	attributeModifiers[effect.Attribute][effect.Layer].push_back(mod);
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes_v2::ClearLayeredEffects()
{
	attributeModifiers = {};
}

void LayeredAttributes_v2::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}

void LayeredAttributes_v2::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

bool LayeredAttributes_v2::attributeInBounds(AttributeKey attribute) const
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

void LayeredAttributes_v2::consolidateOperands(const LayeredEffectDefinition& effect, std::vector<Mod>& vecMods, Mod& mod)
{
	if (!vecMods.empty() && vecMods.back().operation == effect.Operation)
	{
		if (mod.operation == EffectOperation_Set)
		{
			vecMods.pop_back();
		}
		else if (mod.operation == EffectOperation_Add || mod.operation == EffectOperation_Subtract)
		{
			mod.modifier += vecMods.back().modifier;
			vecMods.pop_back();
		}
		else if (mod.operation == EffectOperation_Multiply)
		{
			mod.modifier *= vecMods.back().modifier;
			vecMods.pop_back();
		}
	}
}
