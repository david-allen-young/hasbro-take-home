#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <array>

class LA_v3_Extension_Removals : public ILayeredAttributes
{
public:
	struct Mod
	{
		EffectOperation operation;
		int modifier;
		size_t uid;
	};

	LA_v3_Extension_Removals(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, size_t rereservationSize = 10ULL);
	virtual ~LA_v3_Extension_Removals() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
	int GetCurrentAttribute(AttributeKey attribute) const override;
	void AddLayeredEffect(LayeredEffectDefinition effect) override;
	void ClearLayeredEffects() override;

	bool RemoveLayeredEffect(size_t uid);

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	size_t reservationSize;

	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes, currentAttributes, highestLayers;

	struct ModIndex
	{
		int layer = -1;
		size_t key = -1;
		AttributeKey attribute;
	};
	std::unordered_map<size_t/*uid*/, ModIndex> modIndexes;
	size_t currentUID = 0;
	size_t createUID();

	//using LayerModsMap = std::map</*layer*/int, std::vector<Mod>>;
	using LayerModsMap = std::map</*layer*/int, std::map</*key*/int, Mod>>;
	std::array<LayerModsMap, NumAttributes> attributeModifiers;

	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	bool attributeInBounds(AttributeKey attribute) const;
	void calculateAndCache(AttributeKey attribute);
	void updateCache(AttributeKey attribute, const Mod& mod);
};
