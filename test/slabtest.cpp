#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <mem/slab.h>
#include <stdint.h>
#include <set>

TEST_CASE( "Slab allocator works" ) {
	struct slab_page slb;
	
	uintptr_t slab_start = (uintptr_t)&slb;
	uintptr_t slab_end = slab_start + sizeof(slb);
	
	slabp_init(&slb, 1);
	
	uint32_t* ptr = (uint32_t*)slabp_alloc(&slb);
	
	REQUIRE( slab_start < (uintptr_t)ptr );
	
	REQUIRE( slab_end > (uintptr_t)ptr );
	
	REQUIRE( ((uintptr_t)ptr & 3) == 0 );
	
	*ptr = 0x12345678;
	
	REQUIRE( *ptr == 0x12345678 );
	
	slabp_free(&slb, ptr);
}

TEST_CASE( "Slab allocator allocates all unique elements" ) {
	std::set<uintptr_t> elements;
	
	struct slab_page slb;
	
	uintptr_t slab_start = (uintptr_t)&slb;
	uintptr_t slab_end = slab_start + sizeof(slb);
	
	slabp_init(&slb, 1);
	
	uintptr_t elem = 0;
	
	while (!slabp_full(&slb)) {
		elem = (uintptr_t)slabp_alloc(&slb);
		
		REQUIRE( slab_start < elem );
		REQUIRE( elem < slab_end );
				
		REQUIRE( elements.count(elem) == 0 );
		
		elements.insert( elem );
	}
	
	REQUIRE(elements.size() > ( sizeof(slb) / (2 * sizeof(uint32_t)) ));
}

TEST_CASE( "Slab allocator can alloc and free" ) {
	struct slab_page slb;
	
	slabp_init(&slb, 1);
	
	for (uint32_t i = 0; i < 0x100000; i++) {
		slabp_free(&slb, slabp_alloc(&slb));
	}
	
	REQUIRE( !slabp_full(&slb) );
}

TEST_CASE( "Slab allocator works with NPO2 sizes" ) {
	struct slab_page slb;
	
	for (int i = 1; i < SLABP_SIZE; i++) {
		slabp_init(&slb, i);
		while (!slabp_full(&slb)) {
			slabp_alloc(&slb);
		}
	}
}
