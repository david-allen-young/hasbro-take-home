#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <unordered_map>


class Effect
{
public:
	Effect(LayeredEffectDefinition effectDef, size_t timestamp)
	{
		std::swap(effectDef, this->effectDef);
		std::swap(timestamp, this->timestamp);
	}
	void nullify() { effectDef.Operation = EffectOperation_Invalid; }
	void updateModification(int modification) { effectDef.Modification = modification; }
	AttributeKey getAttribute() const { return effectDef.Attribute; }
	int getOperation() const { return effectDef.Operation; }
	int getModification() const { return effectDef.Modification; }
	int getLayer() const { return effectDef.Layer; }
	size_t getTimestamp() const { return timestamp; }

private:
	LayeredEffectDefinition effectDef;
	size_t timestamp;
};

struct EffectComparator
{
	bool operator()(const Effect& a, const Effect& b) const
	{
		if (a.getLayer() != b.getLayer())
		{
			return a.getLayer() < b.getLayer();
		}
		return a.getTimestamp() < b.getTimestamp();
	}
};

class LayeredAttributes_v6 : public ILayeredAttributes
{
public:
	LayeredAttributes_v6(bool errorLoggingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LayeredAttributes_v6() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
	int GetCurrentAttribute(AttributeKey attribute) const override;
	void AddLayeredEffect(LayeredEffectDefinition effect) override;
	void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	size_t reservationSize;

	size_t nextTimestamp = 0;
	size_t getNextTimestamp() { return nextTimestamp++; }

	mutable std::unordered_map<AttributeKey, int> baseAttributes;
	mutable std::vector<Effect> effects;
	mutable bool effectsUnsorted = true;
	mutable std::unordered_map<AttributeKey, int> cache;

	int calculateAttribute(AttributeKey attribute) const;
	void updateAttribute(const Effect& effect, int& result) const;

	void updateLayerValidation(const Effect& effect);
	void updateCachedAttribute(const Effect& effect);
	void updateEffectsStorage(const Effect& effect);

	bool flattenOperations(const Effect& effect);

	bool isValidAttributeKey(AttributeKey attribute) const;
	void logError(AttributeKey attribute) const;
};
