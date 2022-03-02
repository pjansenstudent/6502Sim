#pragma once

/// <summary>
/// This is the Memory Class, it will contain our memory, it really only needs a few functions, as it's job is to intialize a block of memory, then access or store memory based on an input binary address, and clear it when necessary
/// I've decided to do address translation in this class, as it will 
/// note the liberal use of unsigned, as we're dealing with raw binary (or in my case, hex, cause it's easier to work with)
/// </summary>
class Memory
{
private:
	unsigned char* _memblock; //I'm using a char as it is a 8-bit variable, which we're going to use, since we're using 8 bit memory slots 
	unsigned int _memsize; //a variable for storing the size of the memory
	unsigned short bytesToArrayOffset(unsigned char offsetHigh, unsigned char offsetLow); //a function that will take care of address translation based on two 8-bit inputs, will be needed for addressing, since I can't just char/8 as
	bool checkAddress(unsigned short addr); //to ensure the address provided is valid given the size of the Memory

public:
	Memory(); //default constructor which I will not be using in my case, but there for good practice
	Memory(unsigned int memSize); //the actual constructor which we will use, 
	~Memory(); //our decstructor, to deal with our memory block on destruction
	void clearMemory(); // a function for clearing the memory (aka: setting everything to 0x00) 
	unsigned char read(unsigned char offsetHigh, unsigned char offsetLow);
	void write(unsigned char offsetHigh, unsigned char offsetLow, unsigned char value);
};