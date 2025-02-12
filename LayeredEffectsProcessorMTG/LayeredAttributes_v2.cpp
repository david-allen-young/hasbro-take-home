#include "LayeredAttributes_v2.hpp"
#include <stdexcept>

LayeredAttributes_v2::LayeredAttributes_v2(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{
	baseAttributes.fill(0);
	currentAttributes.fill(0);
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
		// NB: I suppose this is the nuclear option
		// but I would prefer an obvious bad outcome
		// from a bad input during runtime
		baseAttributes.fill(std::numeric_limits<int>().min());
	}
	// NB: This has the disadvantage of recalculating
	// each time SetBaseAttribute is called, however
	// I see no way to implement a batch set of all attributes
	// while maintaining the sanctity of the given interface
	recalculateCurrentAttributes();
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v2::GetCurrentAttribute(AttributeKey attribute) const
{
	if (attributeInBounds(attribute))
	{
		return currentAttributes[attribute];
	}
	// make it obvious during runtime that we received bad input
	return std::numeric_limits<int>::min();
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v2::AddLayeredEffect(LayeredEffectDefinition effect)
{
	bool shouldRecalculate = effect.Layer < highestLayer;
	highestLayer = std::max(highestLayer, effect.Layer);
	if (layeredEffects.count(effect.Layer) == 0)
	{
		// NB: Here my intention is to make subsequent pushes more efficient by preallocating memory
		// The default value (10) seemed like a reasonable number of slots to reserve given the mechanics of the card game
		// (the interface and instructions are ambiguous as to the likely volume of calls)
		layeredEffects[effect.Layer] = std::vector<LayeredEffectDefinition>();
		layeredEffects[effect.Layer].reserve(reservationSize);
	}
	else if (layeredEffects[effect.Layer].capacity() < layeredEffects[effect.Layer].size() + 1)
	{
		// NB: And here we grow by chunks when the capacity is exceeded
		layeredEffects[effect.Layer].reserve(layeredEffects[effect.Layer].size() + reservationSize);
	}
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
void LayeredAttributes_v2::ClearLayeredEffects()
{
	layeredEffects.clear();
	currentAttributes = baseAttributes;
}

void LayeredAttributes_v2::recalculateCurrentAttributes()
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

void LayeredAttributes_v2::updateCurrentAttributes(const LayeredEffectDefinition& effect)
{
	if (attributeInBounds(effect.Attribute) == false)
	{
		// NB: I suppose this is the nuclear option
		// but I would prefer an obvious bad outcome
		// from a bad input during runtime
		currentAttributes.fill(std::numeric_limits<int>().min());
		return;
	}
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
		// NB: In this case I am not using the nuclear option
		// because the index is in range and the consequence
		// of receiving an Invalid operation is ambiguous
		// with only the interface and instructions provided
		// (refer to ILayeredAttributes.hpp)
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
