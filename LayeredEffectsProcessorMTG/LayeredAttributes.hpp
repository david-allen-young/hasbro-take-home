#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <array>

class LayeredAttributes : public ILayeredAttributes
{
public:
	LayeredAttributes(bool errorLoggingEnabled = false, bool errorHandlingEnabled = false, int rereservationSize = 10);
	virtual ~LayeredAttributes() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
    int GetCurrentAttribute(AttributeKey attribute) const override;
    void AddLayeredEffect(LayeredEffectDefinition effect) override;
    void ClearLayeredEffects() override;

private:
	bool errorLoggingEnabled;
	bool errorHandlingEnabled;
	int reservationSize;

	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes;
	std::array<int, NumAttributes> currentAttributes;
	//int lowestLayer = std::numeric_limits<int>::max();
	int highestLayer = std::numeric_limits<int>::min();

	// NB: I've chosen a (sorted) map over the (more efficient) unordered_map
	// because I believe the total number of layeredEffects inserted
	// will likely be less than 100 and it is easier to keep the
	// effects sorted by layer during insertion since I am iterating
	// through the map during recalculateCurrentAttributes()
	std::map<int, std::vector<LayeredEffectDefinition>> layeredEffects;

	void recalculateCurrentAttributes();
	void updateCurrentAttributes(const LayeredEffectDefinition& effect);
	void logError(LayeredEffectDefinition effect);
	void logError(AttributeKey attribute) const;
	bool attributeInBounds(AttributeKey attribute) const;
};
