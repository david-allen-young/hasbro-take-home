#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <array>

class LA_v3_Extension_Removals : public ILayeredAttributes
{
public:
	LA_v3_Extension_Removals(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LA_v3_Extension_Removals() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
	int GetCurrentAttribute(AttributeKey attribute) const override;
	void AddLayeredEffect(LayeredEffectDefinition effect) override;
	void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	size_t reservationSize;

	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes, currentAttributes, highestLayers;

	struct Mod
	{
		EffectOperation operation;
		int modifier;
	};
	using LayerModsMap = std::map</*layer*/int, std::vector<Mod>>;
	std::array<LayerModsMap, NumAttributes> attributeModifiers;

	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	bool attributeInBounds(AttributeKey attribute) const;
	void calculateAndCache(AttributeKey attribute);
	void updateCache(AttributeKey attribute, const Mod& mod);
};
