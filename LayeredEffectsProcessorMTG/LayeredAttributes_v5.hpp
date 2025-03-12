#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <array>

class LayeredAttributes_v5 : public ILayeredAttributes
{
public:
	LayeredAttributes_v5(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LayeredAttributes_v5() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
	int GetCurrentAttribute(AttributeKey attribute) const override;
	void AddLayeredEffect(LayeredEffectDefinition effect) override;
	void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	size_t reservationSize;

	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	mutable std::array<int, NumAttributes> baseAttributes, currentAttributes, highestLayers;

	struct Mod
	{
		EffectOperation operation;
		int modifier;
	};
	using LayerModsMap = std::map</*layer*/int, std::vector<Mod>>;
	mutable std::array<LayerModsMap, NumAttributes> attributeModifiers;
	mutable std::array<bool, NumAttributes> attributeDirty;

	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	bool attributeInBounds(AttributeKey attribute) const;
	void calculateAndCache(AttributeKey attribute) const;
	void updateCache(AttributeKey attribute, const Mod& mod) const;
};
