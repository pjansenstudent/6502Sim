#pragma once
#include "Memory.h"

/// <summary>
/// Processor info references that I'm using: 
/// https://www.masswerk.at/6502/6502_instruction_set.html //excellent table view of the instruction set and addressing modes, making it very useful for my purposes, a good chunk of the references I'm using will be from here
/// 
/// </summary>

/// <summary>
/// an enumeration for the different addressing modes, for my own convenience, I'll be using this to avoid re-defining redundant instructions
/// </summary>
enum ADDRESS_MODES {
	ACCUMULATOR, ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, IMMEDIATE, IMPLIED, INDIRECT, INDIRECT_X, INDIRECT_Y, RELATIVE, ZEROPAGE, ZEROPAGE_X, ZEROPAGE_Y, ERR //err is for error, for JAM operations mainly
};

/// <summary>
/// an enumeration for the different instructions, to allow me to have instructions conveniently referred to by type rather than instructing each variant of instruction possible
/// </summary>
enum INSTRUCTIONS {
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA, JAM //JAM is being used with all illegal instructions at the moment, which are not implemented at this point, albeit it may be contemplated, at which point I'll need to make custom instructions for them (for things like LAX and SBX instructions)
};

enum PROCESSOR_STATE {
	FETCH, DECODE, EXECUTE, JAMMED
};


class Processor
{
private:
	//the tables for instructions and address modes (to be completed, these will be 2d arrays), and yes, they did take quite a bit of time to translate to my code (and check for correctness)
	const ADDRESS_MODES address_table[16][16] =
	{
		{IMPLIED,	INDIRECT_X,	ERR,		INDIRECT_X,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,	 ACCUMULATOR,	IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{ABSOLUTE,	INDIRECT_X, ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,	 ACCUMULATOR,	IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMPLIED,	INDIRECT_X, ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  ACCUMULATOR,	IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMPLIED,	INDIRECT_X,	ERR,		INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  ACCUMULATOR,	IMMEDIATE,  INDIRECT,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMMEDIATE,	INDIRECT_X,	IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_Y, ZEROPAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y},
		{IMMEDIATE, INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_Y, ZEROPAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y},
		{IMMEDIATE,	INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,  ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X},
		{IMMEDIATE, INDIRECT_X, IMMEDIATE,	INDIRECT_X, ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	ZEROPAGE,	IMPLIED, IMMEDIATE,  IMPLIED,		IMMEDIATE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE,	ABSOLUTE},
		{RELATIVE,	INDIRECT_Y, ERR,		INDIRECT_Y, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, ZEROPAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,		ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X}
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
			unsigned char n_flag : 1; //negative flag
			unsigned char o_flag : 1; //overflow flag
			unsigned char rsvd : 1; //unused reserved bit
			unsigned char b_flag : 1; //break flag
			unsigned char d_flag : 1; //decimal flag
			unsigned char id_flag : 1; //interrupt disable flag
			unsigned char z_flag : 1; //zero flag
			unsigned char c_flag : 1; //carry flag
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

	//internal functions for fetch, decode, and execute 
	void fetch();
	void decode();
	void execute();
	void increment_pc();

public:
	Processor(); //default constructor, defaults to 2KB RAM/ROM
	Processor(unsigned int ram_size, unsigned int rom_size); //specific constructor for instantiating a different size of RAM/ROM
	~Processor(); // our destructor, which will be used to clear up RAM/ROM pointers
	//finally, the functions that I'll be able to use from outside the class itself, that the interface and controlling apparatus will use
	void step(); // this function will be used to initiate the fetch-decode-execute cycle by the processor
	unsigned char get_output(); //this function will be used to get the resulting output from processor (aka, what would be on the data pins)
	unsigned char get_address_high(); //this function will be used to get the address pins (high bits)
	unsigned char get_address_low(); // same for low bits
	unsigned char get_sflags(); //for getting register status, I can use the unsigned char for this
	unsigned char get_accumulator(); //get accumulator contents
	unsigned char get_x(); //for getting x register contents
	unsigned char get_y(); //same for y
	bool get_readwrite(); //currently unused, but will be used to get the status of reading/writing pin, can be used if design is changed to implement timing and simulate actual processor hardware function
	void reset(); //for resetting the CPU to initial status (should be pretty straightforward)
	const char* get_state(); //will convert the processor state to a string (of some sort, c-style for now, likely will be changed to some Win32 string or something), and return it for the interface

	//functions I'm not sure how to implement yet, but will need
	//void load_rom(//some sort of file input or something); //I will definitely need some sort of function for loading instructions into the ROM
	//void get_rom(); //for getting rom values from CPU, unsure of how I'm going to do this right now, but I'll need some sort of function for getting the ROM contents
	//void get_ram(); //for getting ram values from CPU, unsure of how I'm going to do this right now, but I'll need some sort of function for getting the RAM contents
	
};

