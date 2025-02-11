#pragma once
#include "ILayeredAttributes.hpp"
//#include <queue>
#include <vector>
#include <map>
#include <array>

class LayeredAttributes : public ILayeredAttributes
{
public:
	LayeredAttributes();
	virtual ~LayeredAttributes() = default;
	void SetBaseAttribute(AttributeKey attribute, int value) override;
    int GetCurrentAttribute(AttributeKey attribute) const override;
    void AddLayeredEffect(LayeredEffectDefinition effect) override;
    void ClearLayeredEffects() override;
private:
	static const size_t NumAttributes = AttributeKey::AttributeKey_Controller + 1;
	std::array<int, NumAttributes> baseAttributes;
	std::array<int, NumAttributes> currentAttributes;
	using Effect = LayeredEffectDefinition;
	//struct EffectCmp
	//{
	//	bool operator()(std::pair<Effect, int> lhs, std::pair<Effect, int> rhs) const
	//	{
	//		bool sameLayer = (lhs.first.Layer == rhs.first.Layer);
	//		bool timestampPriority = (lhs.second > rhs.second);
	//		bool layerPriority = (lhs.first.Layer > rhs.first.Layer);
	//		return sameLayer ? timestampPriority : layerPriority;
	//	}
	//};
	//std::priority_queue<std::pair<Effect, int>, std::vector<std::pair<Effect, int>>, EffectCmp> minHeap;
	std::map<int, std::vector<Effect>> layeredEffects;
};
