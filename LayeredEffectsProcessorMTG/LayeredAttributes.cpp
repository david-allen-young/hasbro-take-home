#include "LayeredAttributes.hpp"

LayeredAttributes::LayeredAttributes()
{
}

//Set the base value for an attribute on this object. All base values
//default to 0 until set. Note that resetting a base attribute does not
//alter any existing layered effects.
void LayeredAttributes::SetBaseAttribute(AttributeKey attribute, int value)
{
}

//Return the current value for an attribute on this object. Will
//be equal to the base value, modified by any applicable layered
//effects.
int LayeredAttributes::GetCurrentAttribute(AttributeKey attribute) const
{
	return 0;
}

//Applies a new layered effect to this object's attributes. See
//LayeredEffectDefinition for details on how layered effects are
//applied. Note that any number of layered effects may be applied
//at any given time. Also note that layered effects are not necessarily
//applied in the same order they were added. (see LayeredEffectDefinition.Layer)
void LayeredAttributes::AddLayeredEffect(LayeredEffectDefinition effect)
{
}

//Removes all layered effects from this object. After this call,
//all current attributes will be equal to the base attributes.
void LayeredAttributes::ClearLayeredEffects()
{
}
