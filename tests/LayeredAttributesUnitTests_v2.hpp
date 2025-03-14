#pragma once
#include <memory>
#include "../src/ILayeredAttributes.hpp"

class LayeredAttributesUnitTests_v2
{
public:
	LayeredAttributesUnitTests_v2() = default;
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
