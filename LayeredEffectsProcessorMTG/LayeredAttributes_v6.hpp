#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <unordered_map>

class Effect
{
public:
	Effect(LayeredEffectDefinition effectDef, int timestamp)
	{
		std::swap(effectDef, this->effectDef);
		std::swap(timestamp, this->timestamp);
	}
	AttributeKey getAttribute() const { return effectDef.Attribute; }
	int getOperation() const { return effectDef.Operation; }
	int getModification() const { return effectDef.Modification; }
	int getLayer() const { return effectDef.Layer; }
	int getTimestamp() const { return timestamp; }

private:
	LayeredEffectDefinition effectDef;
	int timestamp;
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
	LayeredAttributes_v6(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LayeredAttributes_v6() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
	int GetCurrentAttribute(AttributeKey attribute) const override;
	void AddLayeredEffect(LayeredEffectDefinition effect) override;
	void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	size_t reservationSize;

	mutable std::unordered_map<AttributeKey, int> baseAttributes;
	mutable std::vector<Effect> effects;
	mutable bool effectsUnsorted = true;

	void logError(LayeredEffectDefinition effectDef);
	void logError(AttributeKey attribute) const;

	//bool attributeInBounds(AttributeKey attribute) const;

	mutable std::unordered_map<AttributeKey, int> cache;
	int calculate(AttributeKey attribute) const;
	void update(const Effect& effect, int& result) const;

	//void updateCache(AttributeKey attribute, const Mod& mod) const;

	size_t nextTimestamp = 0;
	size_t getNextTimestamp() { return nextTimestamp++; }
};
