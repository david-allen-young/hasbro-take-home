#pragma once
#include "ILayeredAttributes.hpp"
#include <vector>
#include <map>
#include <array>

class LayeredAttributes : public ILayeredAttributes
{
public:
	LayeredAttributes(bool shouldLogErrors = false, bool shouldThrowErrors = false);
	virtual ~LayeredAttributes() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
    int GetCurrentAttribute(AttributeKey attribute) const override;
    void AddLayeredEffect(LayeredEffectDefinition effect) override;
    void ClearLayeredEffects() override;
	bool errorLoggingEnabled = false;
	bool errorHandlingEnabled = false;
private:
	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes;
	std::array<int, NumAttributes> currentAttributes;
	std::map<int, std::vector<LayeredEffectDefinition>> layeredEffects;
	int lowestLayer = std::numeric_limits<int>::max();
	void recalculateCurrentAttributes();
	void updateCurrentAttributes(const LayeredEffectDefinition& effect);
	void logError(LayeredEffectDefinition effect);
};
