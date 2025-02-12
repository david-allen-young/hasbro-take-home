#include "LayeredAttributes.hpp"
#include <stdexcept>

LayeredAttributes::LayeredAttributes(bool shouldLogErrors, bool shouldThrowErrors)
{
	errorLoggingEnabled = shouldLogErrors;
	errorHandlingEnabled = shouldThrowErrors;
	baseAttributes.fill(0);
	currentAttributes.fill(0);
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes::SetBaseAttribute(AttributeKey attribute, int value)
{
	baseAttributes[attribute] = value;
	recalculateCurrentAttributes();
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes::GetCurrentAttribute(AttributeKey attribute) const
{
	return currentAttributes[attribute];
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes::AddLayeredEffect(LayeredEffectDefinition effect)
{
	//bool shouldRecalculate = effect.Layer < lowestLayer;
	//lowestLayer = std::min(lowestLayer, effect.Layer);
	bool shouldRecalculate = effect.Layer < highestLayer;
	highestLayer = std::max(highestLayer, effect.Layer);
	layeredEffects[effect.Layer].push_back(effect);
	if (shouldRecalculate)
	{
		recalculateCurrentAttributes();
	}
	else
	{
		updateCurrentAttributes(effect);
	}
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes::ClearLayeredEffects()
{
	layeredEffects.clear();
	currentAttributes = baseAttributes;
}

void LayeredAttributes::recalculateCurrentAttributes()
{
	currentAttributes = baseAttributes;
	for (auto& [layer, effects] : layeredEffects)
	{
		for (auto& effect : effects)
		{
			updateCurrentAttributes(effect);
		}
	}
}

void LayeredAttributes::updateCurrentAttributes(const LayeredEffectDefinition& effect)
{
	if (effect.Operation == EffectOperation::EffectOperation_Set)
	{
		currentAttributes[effect.Attribute] = effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_Add)
	{
		currentAttributes[effect.Attribute] += effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_Subtract)
	{
		currentAttributes[effect.Attribute] -= effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_Multiply)
	{
		currentAttributes[effect.Attribute] *= effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_BitwiseOr)
	{
		currentAttributes[effect.Attribute] |= effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_BitwiseAnd)
	{
		currentAttributes[effect.Attribute] &= effect.Modification;
	}
	else if (effect.Operation == EffectOperation::EffectOperation_BitwiseXor)
	{
		currentAttributes[effect.Attribute] ^= effect.Modification;
	}
	else //if (effect.Operation == EffectOperation::EffectOperation_Invalid)
	{
		if (errorLoggingEnabled)
		{
			logError(effect);
		}
		if (errorHandlingEnabled)
		{
			throw std::runtime_error("Invalid effect operation");
		}
	}
}

void LayeredAttributes::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}
