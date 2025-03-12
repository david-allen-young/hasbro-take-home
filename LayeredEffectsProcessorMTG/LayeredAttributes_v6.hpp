#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
//#include <array>
#include <unordered_map>

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

	mutable std::vector<std::tuple</*layer*/int, /*timestamp*/int, /*attribute*/int, /*modifier*/int>> effects;

	//struct Mod
	//{
	//	EffectOperation operation;
	//	int modifier;
	//};
	//using LayerModsMap = std::map</*layer*/int, std::vector<Mod>>;
	//mutable std::unordered_map<AttributeKey, std::pair<LayerModsMap, /*modifier*/int>> attributeModifiers;
	//mutable std::unordered_map<AttributeKey, /*dirty*/bool> attributeDirty;

	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	//bool attributeInBounds(AttributeKey attribute) const;
	//void calculateAndCache(AttributeKey attribute) const;
	//void updateCache(AttributeKey attribute, const Mod& mod) const;
};
