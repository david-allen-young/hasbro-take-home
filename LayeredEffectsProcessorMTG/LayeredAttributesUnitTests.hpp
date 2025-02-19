#pragma once
#include <memory>
#include "ILayeredAttributes.hpp"

class LayeredAttributesUnitTests
{
public:
	LayeredAttributesUnitTests() = default;
	void runOperationalTests();
	void runCrashTests(); // may throw errors

private:
	std::unique_ptr<ILayeredAttributes> attributes;

	// operational tests
	void testSetAndGet();
	void testAddAndClear();
	void testConsolidation();
	void testBitwise();
	void testComplexAdd_v1();
	void testComplexAdd_v2();

	// crash tests
	void testZeroReservation();
	void testOutOfBounds();
};
