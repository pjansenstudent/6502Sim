#pragma once
#include "Memory.h"
#include <fstream> //file input/output for c++, I'm going to use this for 


/// <summary>
/// Processor info references that I'm using: 
/// https://www.masswerk.at/6502/6502_instruction_set.html //excellent table view of the instruction set and addressing modes, making it very useful for my purposes, a good chunk of the references I'm using will be from here
/// https://www.atarimagazines.com/compute/issue53/047_1_All_About_The_Status_Register.php
/// http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html //learning the logic behind the 6502 overflow flag, which is signed rather than simply testing to see if result exceeds 0xFF
/// http://6502.org/tutorials/ //for a whole bunch of primers on all of the details of operations and components of the 6502
/// </summary>

/// <summary>
/// an enumeration for the different addressing modes, for my own convenience, I'll be using this to avoid re-defining redundant instructions
/// </summary>
enum ADDRESS_MODES {
	ACCUMULATOR, ABSOLUT, ABSOLUTE_X, ABSOLUTE_Y, IMMEDIATE, IMPLIED, INDIRECT, INDIRECT_X, INDIRECT_Y, RELATIV, ZEROPAGE, ZEROPAGE_X, ZEROPAGE_Y, ERR //err is for error, for JAM operations mainly
};

/// <summary>
/// an enumeration for the different instructions, to allow me to have instructions conveniently referred to by type rather than instructing each variant of instruction possible
/// </summary>
enum INSTRUCTIONS {
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA, JAM //JAM is being used with all illegal instructions at the moment, which are not implemented at this point, albeit it may be contemplated, at which point I'll need to make custom instructions for them (for things like LAX and SBX instructions)
};

/// <summary>
/// Enum for processor state, 
/// </summary>
enum PROCESSOR_STATE {
	FETCH, DECODE, EXECUTE, JAMMED
};


class Processor
{
private:
	//the tables for instructions and address modes (to be completed, these will be 2d arrays), and yes, they did take quite a bit of time to translate to my code (and check for correctness)
	const ADDRESS_MODES address_table[16][16] =
	{
		{IMPLIED,	INDIRECT_X,	ERR,		INDIRECT_X,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,	 ACCUMULATOR,	IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{ABSOLUT,	INDIRECT_X, ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,	 ACCUMULATOR,	IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMPLIED,	INDIRECT_X, ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  ACCUMULATOR,	IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMPLIED,	INDIRECT_X,	ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  ACCUMULATOR,	IMMEDIATE,  INDIRECT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMMEDIATE,	INDIRECT_X,	IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_Y, ZEROPAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y},
		{IMMEDIATE, INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_Y, ZEROPAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y},
		{IMMEDIATE,	INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMMEDIATE, INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,	ABSOLUT,	ABSOLUT,	ABSOLUT,	ABSOLUT},
		{RELATIV,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X}
	};
	
	const INSTRUCTIONS instruction_table[16][16] = 
	{
		{BRK, ORA, JAM, JAM, JAM, ORA, ASL, JAM, PHP, ORA, ASL, JAM, JAM, ORA, ASL, JAM},
		{BPL, ORA, JAM, JAM, JAM, ORA, ASL, JAM, CLC, ORA, JAM, JAM, JAM, ORA, ASL, JAM},
		{JSR, AND, JAM, JAM, BIT, AND, ROL, JAM, PLP, AND, ROL, JAM, BIT, AND, ROL, JAM},
		{BMI, AND, JAM, JAM, JAM, AND, ROL, JAM, SEC, AND, JAM, JAM, JAM, AND, ROL, JAM},
		{RTI, EOR, JAM, JAM, JAM, EOR, LSR, JAM, PHA, EOR, LSR, JAM, JMP, EOR, LSR, JAM},
		{BVC, EOR, JAM, JAM, JAM, EOR, LSR, JAM, CLI, EOR, JAM, JAM, JAM, EOR, LSR, JAM},
		{RTS, ADC, JAM, JAM, JAM, ADC, ROR, JAM, PLA, ADC, ROR, JAM, JMP, ADC, ROR, JAM},
		{BVS, ADC, JAM, JAM, JAM, ADC, ROR, JAM, SEI, ADC, JAM, JAM, JAM, ADC, ROR, JAM},
		{JAM, STA, JAM, JAM, STY, STA, STX, JAM, DEY, JAM, TXA, JAM, STY, STA, STX, JAM},
		{BCC, STA, JAM, JAM, STY, STA, STX, JAM, TYA, STA, TXS, JAM, JAM, STA, JAM, JAM},
		{LDY, LDA, LDX, JAM, LDY, LDA, LDX, JAM, TAY, LDA, TAX, JAM, LDY, LDA, LDX, JAM},
		{BCS, LDA, JAM, JAM, LDY, LDA, LDX, JAM, CLV, LDA, TSX, JAM, LDY, LDA, LDX, JAM},
		{CPY, CMP, JAM, JAM, CPY, CMP, DEC, JAM, INY, CMP, DEX, JAM, CPY, CMP, DEC, JAM},
		{BNE, CMP, JAM, JAM, JAM, CMP, DEC, JAM, CLD, CMP, JAM, JAM, JAM, CMP, DEC, JAM},
		{CPX, SBC, JAM, JAM, CPX, SBC, INC, JAM, INX, SBC, NOP, JAM, CPX, SBC, INC, JAM},
		{BEQ, SBC, JAM, JAM, JAM, SBC, INC, JAM, SED, SBC, JAM, JAM, JAM, SBC, INC, JAM}
	};
	//both are const so that the values cannot be changed
	
	//instantiations of the enums above to be used for executing instructions in my model 
	ADDRESS_MODES addr_mode;
	INSTRUCTIONS inst;
	
	/// <summary>
	/// breakdown of processor flag register into bitwise union/struct, so that I can address the bits of the status flag register individually
	/// I learned of this trick, as well as got some ideas regarding the "structure" of my simulator (more or less future structure, if I decide to connect the modules to a "bus" with timing), from this video: https://www.youtube.com/watch?v=F8kx56OZQhg
	/// note: I am not actually using any of the code from this video series (as far as I'm aware, although it may be similar as both involve 6502 emulation), I've actually only seen the first video, it's part of what gave me the idea of doing this project, and I did feel like sourcing where I learned the union/struct trick
	/// </summary>
	union sflag_reg {
		struct {
			//breakdown of flag register will go here
			unsigned char n_flag : 1; //negative flag, common flag, it determines whether operation results in negative number (bit 7 of resultant operation's register is 1)
			unsigned char o_flag : 1; //overflow flag, detects when a signed overflow has occured (so result is > 127 or < -127, I think is the range), has some interesting logic behind it
			unsigned char rsvd : 1; //unused reserved bit, it will likely not be used here
			unsigned char b_flag : 1; //break flag, it likely will not be that necessary for my purposes, as it is essentially used to determine software breaks, but I'll implement the instruction for it, so it does matter
			unsigned char d_flag : 1; //decimal flag, used for determining whether the processor will operate in decimal mode, also called BCD mode, where operations are done with BCD numbers, not implemented specifically at the moment, but will likely be added after regular binary mode is complete
			unsigned char id_flag : 1; //interrupt disable flag
			unsigned char z_flag : 1; //zero flag, very useful flag, determines
			unsigned char c_flag : 1; //carry flag, used when doing addition/subtraction to ensure that proper results are obtained 
		};
		unsigned char val;
	};

	/// <summary>
	/// this union will be useful if I need to check individual bits of an operand
	/// </summary>
	union byte {
		struct {
			unsigned char b7 : 1;
			unsigned char b6 : 1;
			unsigned char b5 : 1;
			unsigned char b4 : 1;
			unsigned char b3 : 1;
			unsigned char b2 : 1;
			unsigned char b1 : 1;
			unsigned char b0 : 1;
		};
		unsigned char val;
	};

	//stack pointer registers (6502 uses 16-bit stack pointer, just like it's addressing capabilities)
	unsigned char pc_high;
	unsigned char pc_low;
	
	// other registers: 
	unsigned char a_reg; //accumulator
	unsigned char x_reg; //index x
	unsigned char y_reg; //index y
	unsigned char sp_reg; //stack pointer
	sflag_reg flags;
	
	
						  
	/// <summary>
	/// Another union, this one is for the data lines for an instruction (note that this is a theoretical thing, the actual processor uses data pins for both input and output on the same bus, I may or may not include a io_data line, but this instruction structure will be used for quickly parsing commands, no instantiation needed here, I'll use it in the function for decoding
	/// </summary>
	union instruction {
		struct {
			unsigned char nib_high : 4;
			unsigned char nib_low : 4;
		};
		unsigned char val; 
	};

	//value for the current instruction, (not actually present in physical processor, but useful for my purposes as stated above
	instruction curr_instruction;
	
	bool read_write; //a boolean value for reading and writing (0 = read, 1 = write...maybe, I'm not matching it directly to the hardware specifications), not sure if I'm going to need this yet, but I've got it here just in case

	//RAM and ROM variables (Pointers of course)
	Memory* ram;
	Memory* rom;

	//instantion of our processor state

	PROCESSOR_STATE state;

	//the output lines, which is the result of an operation
	unsigned char output;

	//internal functions for fetch, decode, and execute 
	void fetch();
	void decode();
	void execute();
	void increment_pc();
	unsigned char little_to_big_endian(unsigned char input);

public:
	Processor(); //default constructor, defaults to 2KB RAM/ROM
	Processor(unsigned int ram_size, unsigned int rom_size); //specific constructor for instantiating a different size of RAM/ROM
	~Processor(); // our destructor, which will be used to clear up RAM/ROM pointers
	//finally, the functions that I'll be able to use from outside the class itself, that the interface and controlling apparatus will use
	void step(); // this function will be used to initiate the fetch-decode-execute cycle by the processor
	unsigned char get_output(); //this function will be used to get the resulting output from processor (aka, what would be on the data pins)
	unsigned char get_pc_high(); //this function will be used to get the address pins (high bits)
	unsigned char get_pc_low(); // same for low bits
	unsigned char get_sflags(); //for getting register status, I can use the unsigned char for this
	unsigned char get_accumulator(); //get accumulator contents
	unsigned char get_x(); //for getting x register contents
	unsigned char get_y(); //same for y
	unsigned char get_sp(); //get stack pointer register
	bool get_readwrite(); //currently unused, but will be used to get the status of reading/writing pin, can be used if design is changed to implement timing and simulate actual processor hardware function
	void reset(); //for resetting the CPU to initial status (should be pretty straightforward)
	const char* get_state(); //will convert the processor state to a string (of some sort, c-style for now, likely will be changed to some Win32 string or something), and return it for the interface
	void load_program(const char* filepath);
	unsigned char get_rom_value(unsigned char address_high, unsigned char address_low);
	unsigned char get_ram_value(unsigned char address_high, unsigned char address_low);
	unsigned int get_rom_size();
	unsigned int get_ram_size();

	//functions I'm not sure how to implement yet, but will need
	//void load_rom(//some sort of file input or something); //I will definitely need some sort of function for loading instructions into the ROM
	
};

