#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <array>

class LayeredAttributes_v2 : public ILayeredAttributes
{
public:
	LayeredAttributes_v2(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LayeredAttributes_v2() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
    int GetCurrentAttribute(AttributeKey attribute) const override;
    void AddLayeredEffect(LayeredEffectDefinition effect) override;
    void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	size_t reservationSize;

	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes;

	struct Mod
	{
		EffectOperation operation;
		int modifier;
	};
	using LayerModsMap = std::map</*layer*/int, std::vector<Mod>>;
	std::array<LayerModsMap, NumAttributes> attributeModifiers;
	std::array<int, NumAttributes> highestLayers;

	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	bool attributeInBounds(AttributeKey attribute) const;
	void consolidateOperands(const LayeredEffectDefinition& effect, std::vector<Mod>& vecMods, Mod& mod);
};
