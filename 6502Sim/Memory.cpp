#include "Memory.h"
/// <summary>
/// Standard destructor class, clean up used memory to prevent memory leaks
/// </summary>
Memory::~Memory() {
	delete[] _memblock; //de-allocate the memory used in the memory block
}

/// <summary>
/// clearMemory Function, sets the entire block of memory to all zeroes. 
/// </summary>
void Memory::clearMemory() {
	for (int i = 0; i < _memsize; i++) {
		_memblock[i] = 0x00; //set binary value of memblock to 00000000
	}
}