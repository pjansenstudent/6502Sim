#include "Memory.h"
/// <summary>
/// Standard destructor class, clean up used memory to prevent memory leaks
/// </summary>

/// <summary>
/// Default Constructor, will be unused in my case, and only throws an error when invoked at the moment
/// </summary>
Memory::Memory() {
	throw 200; //placeholder error value, 
}

/// <summary>
/// 
/// </summary>
/// <param name="memSize">Size of the Memory block to create, must be >= 2048 and <= 65536 </param>
Memory::Memory(unsigned int memSize) {
	if (memSize < 2048 || memSize > 65536) {
		throw 4;
	}
	_memsize = memSize;

	_memblock = new unsigned char[memSize]; //generate a 

	clearMemory(); //clear the memory if it has anything in it
}

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

/// <summary>
/// This Function will convert two characters (representing bits from the addressing pins of the processor)
/// using this method adapted from https://stackoverflow.com/questions/25787349/convert-char-to-short/25787777
/// My version is not using pointers, as it is unnecessary for the operation of this program
/// </summary>
/// <param name="offsetHigh">Offset for the high bytes of the memory address</param>
/// <param name="offsetLow"></param>
/// <returns></returns>
unsigned short Memory::bytesToArrayOffset(unsigned char offsetHigh, unsigned char offsetLow) {
	unsigned short retValue = 0x0000; //ensure that the variable is a clean 
	retValue = offsetHigh;
	retValue << 8; //shift bits left 8 places (for high bits of address)
	retValue = retValue | offsetLow; //bitwise or function to add in the low 

	return retValue;
}

/// <summary>
/// This internal function checks to ensure that an address fed to the memory is within range
/// </summary>
/// <param name="addr">the address being checked</param>
/// <returns></returns>
bool Memory::checkAddress(unsigned short addr) {
	if (addr > _memsize) {
		return false; //if addr is bigger than _memsize, return false
	}
	return true; //otherwise return true
}


unsigned char Memory::read(unsigned char offsetHigh, unsigned char offsetLow) {
	unsigned short addr = bytesToArrayOffset(offsetHigh, offsetLow);
	if (!checkAddress(addr)) {
		throw 5; //error code for invalid memory address
	}
	return _memblock[addr]; //return the data at that address
}


void Memory::write(unsigned char offsetHigh, unsigned char offsetLow, unsigned char value) {
	unsigned short addr = bytesToArrayOffset(offsetHigh, offsetLow);
	if (!checkAddress(addr)) {
		throw 5; //error code for invalid memory address
	}

	_memblock[addr] = value; //write the value to the address
}