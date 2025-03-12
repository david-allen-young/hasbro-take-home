#include "LayeredAttributes_v6.hpp"
#include <stdexcept>
#include <algorithm>

LayeredAttributes_v6::LayeredAttributes_v6(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{

}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes_v6::SetBaseAttribute(AttributeKey attribute, int value)
{
	baseAttributes[attribute] = value;
	cache.erase(attribute);
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v6::GetCurrentAttribute(AttributeKey attribute) const
{
	if (cache.count(attribute) == 0)
	{
		int result = calculate(attribute);
		cache[attribute] = result;
	}
	return cache[attribute];
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v6::AddLayeredEffect(LayeredEffectDefinition effectDef)
{
	int timestamp = getNextTimestamp();
	auto effect = Effect(effectDef, timestamp);
	if (!effects.empty())
	{
		AttributeKey attribute = effect.getAttribute();
		if (effects.back().getLayer() > effect.getLayer())
		{
			effectsUnsorted = true;
			cache.erase(attribute);
		}
		else
		{
			if (cache.count(attribute) == 0)
			{
				cache[attribute] = baseAttributes[attribute];
			}
			update(effect, cache[attribute]);
		}
	}
	if (effects.size() + 1 > reservationSize)
	{
		effects.reserve(effects.size() + reservationSize);
	}
	effects.push_back(effect);
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes_v6::ClearLayeredEffects()
{
	effects.clear();
	cache.clear();
}

void LayeredAttributes_v6::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}

void LayeredAttributes_v6::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

int LayeredAttributes_v6::calculate(AttributeKey attribute) const
{
	// the map defaults to zero if no key is present
	int result = baseAttributes[attribute];

	if (effectsUnsorted)
	{
		std::sort(effects.begin(), effects.end(), EffectComparator());
		effectsUnsorted = false;
	}

	for (auto& effect : effects)
	{
		if (effect.getAttribute() != attribute)
		{
			continue;
		}
		update(effect, result);
	}

	return result;
}

void LayeredAttributes_v6::update(const Effect& effect, int& result) const
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


