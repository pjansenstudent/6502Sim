#pragma once

/// <summary>
/// This is the Memory Class, it will contain our memory, it really only needs a few functions, as it's job is to intialize a block of memory, then access or store memory based on an input binary address, and clear it when necessary
/// I've decided to do address translation in this class, as it will 
/// </summary>
class Memory
{
private:
	char* _memblock; //I'm using a char as it is a 8-bit variable, which we're going to use, since we're using 8 bit memory slots 
	int _memsize; //a variable for storing the size of the memory
	short bytesToArrayOffset(char offsetHigh, char offsetLow); //a function that will take care of address translation based on two 8-bit inputs, will be needed for addressing, since I can't just char/8 as

public:
	Memory(); //default constructor which I will not be using in my case, but there for good practice
	Memory(int memSize); //the actual constructor which we will use, 
	~Memory(); //our decstructor, to deal with our memory block on destruction
	void clearMemory(); // a function for clearing the memory (aka: setting everything to 0x00) 
	char read(char offsetHigh, char offsetLow);
	void write(char offsetHigh, char offsetLow, char value);
};