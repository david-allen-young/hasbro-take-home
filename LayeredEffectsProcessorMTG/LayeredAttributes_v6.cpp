#include "LayeredAttributes_v6.hpp"
#include <stdexcept>

LayeredAttributes_v6::LayeredAttributes_v6(bool errorLoggingEnabled, bool errorHandlingEnabled, size_t reservationSize)
	: errorLoggingEnabled(errorLoggingEnabled), errorHandlingEnabled(errorHandlingEnabled), reservationSize(std::max(1ULL, reservationSize))
{

}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes_v6::SetBaseAttribute(AttributeKey attribute, int value)
{

}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes_v6::GetCurrentAttribute(AttributeKey attribute) const
{
	return 0;
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes_v6::AddLayeredEffect(LayeredEffectDefinition effect)
{

}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes_v6::ClearLayeredEffects()
{

}

void LayeredAttributes_v6::logError([[maybe_unused]] LayeredEffectDefinition effect)
{
	// Imagine that this method writes something useful to glog or similar logging service
}

void LayeredAttributes_v6::logError([[maybe_unused]] AttributeKey attribute) const
{
	// Imagine that this method writes something useful to glog or similar logging service
}

bool LayeredAttributes_v6::attributeInBounds([[maybe_unused]]AttributeKey attribute) const
{
	return false;
}

void LayeredAttributes_v6::calculateAndCache(AttributeKey attribute) const
{

}

void LayeredAttributes_v6::updateCache(AttributeKey attribute, const Mod& mod) const
{

}
