#include "Processor.h"

/// <summary>
/// Default Constructor, initializes variables and creates RAM/ROM
/// </summary>
Processor::Processor() {
	//choose the first options for each, as they will not matter (they'll be updated at fetch anyway)
	addr_mode = IMPLIED;
	inst = BRK;
	//start program counter at 0
	pc_high = 0x00;
	pc_low = 0x00;

	//set the value of all other registers (at 0x00, because they are being initialized)
	a_reg = 0x00;
	x_reg = 0x00;
	y_reg = 0x00;
	sp_reg = 0x00;

	flags.val = 0x00; //set our flag register to all zeroes

	read_write = 0; //set to read, although right now this function is unusued

	//initialize RAM/ROM, casting our values as unsigned ints, just in case

	ram = new Memory((unsigned int) 2048);
	rom = new Memory((unsigned int) 2048);

	//initialize the processor state to FETCH, allowing FETCH State
	state = FETCH;
}

/// <summary>
/// the optional constructor
/// </summary>
/// <param name="ram_size">Size of the RAM, ensure to check that it is between accepted values (2048 and 65536)</param>
/// <param name="rom_size">Size of the ROM, ensure to check that it is between accepted values (2048 and 65536)</param>
Processor::Processor(unsigned int ram_size, unsigned int rom_size) {
	//choose the first options for each, as they will not matter (they'll be updated at fetch anyway)
	addr_mode = IMPLIED;
	inst = BRK;
	//start program counter at 0
	pc_high = 0x00;
	pc_low = 0x00;

	//set the value of all other registers (at 0x00, because they are being initialized)
	a_reg = 0x00;
	x_reg = 0x00;
	y_reg = 0x00;
	sp_reg = 0x00;

	flags.val = 0x00; //set our flag register to all zeroes

	read_write = 0; //set to read, although right now this function is unusued

	//initialize RAM/ROM, using user specified values
	ram = new Memory(ram_size);
	rom = new Memory(rom_size);

	//initialize the processor state to FETCH, allowing FETCH State
	state = FETCH;
}

/// <summary>
/// Standard destructor, will delete any pointers and things for proper memory cleanup
/// </summary>
Processor::~Processor() {
	delete ram;
	delete rom;
}