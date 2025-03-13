#include "LayeredAttributes_v7.hpp"
#include <stdexcept>
#include <algorithm>

LayeredAttributes_v7::LayeredAttributes_v7(bool errorLoggingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), reservationSize(std::max(1ULL, reservationSize))
{
	baseAttributes.reserve(reservationSize);
	cache.reserve(reservationSize);
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes_v7::SetBaseAttribute(AttributeKey attribute, int value)
{
	if (errorLoggingEnabled && !isValidAttributeKey(attribute))
	{
		logError(attribute);
	}
	baseAttributes[attribute] = value;
	attributeDirty[attribute] = true;
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v7::GetCurrentAttribute(AttributeKey attribute) const
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

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v7::AddLayeredEffect(LayeredEffectDefinition effectDef)
{
	if (errorLoggingEnabled && !isValidAttributeKey(effectDef.Attribute))
	{
		logError(effectDef.Attribute);
	}
	size_t timestamp = getNextTimestamp();
	auto effect = Effect(effectDef, timestamp);
	AttributeKey attribute = effect.getAttribute();
	updateAttribute(effect, cache[attribute]);
	attributeDirty[attribute] = true;
	if (effects[attribute].size() + 1 > reservationSize)
	{
		effects[attribute].reserve(effects.size() + reservationSize);
	}
	if (!flattenOperations(effect))
	{
		auto it = std::lower_bound(effects[attribute].begin(), effects[attribute].end(), effect, EffectComparator());
		effects[attribute].insert(it, effect);
	}
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes_v7::ClearLayeredEffects()
{
	effects = {};
	cache = {};
	attributeDirty = {};
}

bool LayeredAttributes_v7::isValidAttributeKey(AttributeKey attribute) const
{
	// as defined in ILayeredAttributes.hpp
	// at the time of this implementation
	/*
		enum AttributeKey : int
		{
			AttributeKey_NotAssessed = 0,
			AttributeKey_Power,
			AttributeKey_Toughness,
			AttributeKey_Loyalty,
			AttributeKey_Color,
			AttributeKey_Types,
			AttributeKey_Subtypes,
			AttributeKey_Supertypes,
			AttributeKey_ManaValue,
			AttributeKey_Controller
		};
	*/
	if (attribute >= AttributeKey_Power && attribute <= AttributeKey_Controller)
	{
		return true;
	}
	return false;
}

void LayeredAttributes_v7::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

int LayeredAttributes_v7::calculateAttribute(AttributeKey attribute) const
{
	// the map defaults to zero if no key is present
	int result = baseAttributes[attribute];

	for (auto& effect : effects[attribute])
	{
		updateAttribute(effect, result);
	}

	return result;
}

void LayeredAttributes_v7::updateAttribute(const Effect& effect, int& result) const
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
}

bool LayeredAttributes_v7::flattenOperations(const Effect& effect)
{
	bool flattened = false;
	auto attribute = effect.getAttribute();
	if (!effects[attribute].empty())
	{
		auto& oldEffect = effects[attribute].back();
		auto operation = oldEffect.getOperation();
		if (operation == effect.getOperation() && oldEffect.getLayer() == effect.getLayer())
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
			flattened = true;
		}
	}
	return flattened;
}


