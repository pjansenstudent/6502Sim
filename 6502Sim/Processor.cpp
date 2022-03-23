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

/// <summary>
/// Internal Fetch command, a bit crude for now as there is no try-catch for failure of address resolution, however that can be resolved fairly easily once this is operational
/// </summary>
void Processor::fetch() {
	if (state == FETCH) {
		curr_instruction.val = rom->read(pc_high, pc_low);

		state = DECODE;
	}
	else {
		//I need to come up with some crashing logic or some sort of error handling here ideally
	}
}

/// <summary>
/// Internal Decode function, this uses the INSTRUCTIONS and ADDRESS_MODES table to parse the binary to an instruction and addressing mode to be used in the execution
/// </summary>
void Processor::decode() {
	if (state == DECODE) {
		//parse the function
		inst = instruction_table[curr_instruction.nib_high][curr_instruction.nib_low];
		addr_mode = address_table[curr_instruction.nib_high][curr_instruction.nib_low];

		state = EXECUTE;
	}
	else {
		//need to put some error handling for JAM state or wrong state here.
	}
}

/// <summary>
/// The major function of the Processor object, this is where instructions are executed and work is actually done
/// </summary>
void Processor::execute() {
	if (state == EXECUTE) {
		
		
		/*
		Quick Explanation of the logic for the switch,
		each operation will do it's respective work, however, depending on addressing mode, the way the rom is read may be changed
		Thus, I'll have to break up each operation that uses multiple addressing modes and set any operands according to the instruction
		I'll try to reduce redundancy here by separating memory interactions from the actual work
		*/
		switch (inst) {
		case ADC: {
			unsigned char operand = 0x00; //this will be the number that will be used in the addtion

			switch (addr_mode) {
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}

			if (flags.d_flag == 0) {
				//the 6502 has an interesting overflow flag, and it's relationship with the other flags. Basically, it checks to see if overflow occurs on SIGNED math, rather than unsigned, meaning I can't just check for a normal overflow for 
				//using the formula from http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html, we can implement it in C++ rather easily, this should allow the addition to take place
				if (!(a_reg ^ operand) && (a_reg ^ (a_reg + operand))) {

				}
				//As far as I'm aware, the carry bit should be pretty simple, we just need to factor in the current carry bit, and determine whether we got a carry bit, did it in ternary because it's nice and clean
				if (a_reg + operand + ((flags.c_flag == 0b1) ? 0x01 : 0x00) > 0xFF) {
					flags.c_flag = 0b1; //set carry
				}
				else {
					flags.c_flag = 0b0; //clear carry, I think this makes sense, as carry should always be the carry bit
				}
				a_reg += operand; //do the addition for the actual value in the accumulator

				//checks for n and z flags
				if (a_reg = 0x00) {
					flags.z_flag = 0b1;
				}
				if ((a_reg & 0x80) > 0x00) {
					flags.n_flag = 0b1;
				}

				//finally, increment the pc
				increment_pc();
			}
			else {
				//decimal mode addition and arithmetic not yet implemented
			}

		}
			break;
		case AND:
		{
			unsigned char operand = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			a_reg = a_reg & operand; //bitwise and the operand and the accumulator

			//calculate whether flags are set: Negative and Zero flags specifically
			if (a_reg = 0x00) {
				flags.z_flag = 0b1;
			}
			if ((a_reg & 0x80) > 0x00) {
				flags.n_flag = 0b1;
			}
			increment_pc(); //finally, increment program counter for next instruction

		}
			break;
		case ASL: {
			switch (addr_mode) {
			case ACCUMULATOR:
			{
				
				
				//perform bit-shift
				a_reg = a_reg << 1;
				
				//check result for zero and negative results
				if (a_reg == 0x00) {
					flags.z_flag = 0b1;
				}
				if ((a_reg & 0x80) > 0) {
					flags.n_flag = 0b1;
				}
			}
				break;
			case ABSOLUT:
			{
				unsigned char op_high = 0x00;
				unsigned char op_low = 0x00;
				unsigned char result = 0x00;
				increment_pc();
				op_low = rom->read(pc_high, pc_low);
				increment_pc();
				op_high = rom->read(pc_high, pc_low);
				result = ram->read(op_high, op_low);
				if ((result & 0x80) > 0x00) {
					flags.c_flag = 0b1;
				}
				else {
					flags.c_flag = 0b0;
				}

				result = result << 1;

				if (result == 0x00) {
					flags.z_flag = 0b1;
				}
				if (result & 0x80 > 0) {
					flags.n_flag = 0b1;
				}

				//finally, apply the operation to the RAM

				ram->write(op_high, op_low, result);
			}
				break;
			case ABSOLUTE_X:
			{
				unsigned char op_high;
				unsigned char op_low;
				increment_pc();
				op_low = rom->read(pc_high, pc_low);
				increment_pc();
				op_high = rom->read(pc_high, pc_low);

				if (op_low + x_reg > 0xFF) {
					if (op_high + 0x01 > 0xFF) {
						op_high = 0x00;
					}
					else {
						op_high += 0x01;
					}
					op_low == 0x00;
					flags.c_flag = 0b1;
				}
				else {
					flags.c_flag = 0b0;
				}
				op_low += x_reg;

				unsigned char result = ram->read(op_high, op_low);
				if ((result & 0x80) > 0) {
					flags.c_flag = 0b1;
				}
				else {
					flags.c_flag = 0b0;
				}

				result = result << 1;

				if (result == 0x00) {
					flags.z_flag = 0b1;
				}
				if (result & 0x80 > 0) {
					flags.n_flag = 0b1;
				}

				ram->write(op_high, op_high, result);

				increment_pc();
			}
				break;
			case ZEROPAGE: {
				increment_pc();
				unsigned char operand = rom->read(pc_high, pc_low);
				unsigned char result = ram->read(0x00, operand);
				if ((result & 0x80) > 0) {
					flags.c_flag = 0b1;
				}
				result = result << 1;
				
				if (result == 0x00) {
					flags.z_flag = 0b1;
				}
				if (result & 0x80 > 0) {
					flags.n_flag = 0b1;
				}

				ram->write(0x00, operand, result);

				increment_pc();
			}
				break;
			case ZEROPAGE_X: {
				increment_pc();
				unsigned char operand = rom->read(pc_high, pc_low);
				operand += x_reg;
				unsigned char result = ram->read(0x00, operand);
				if ((result & 0x80) > 0) {
					flags.c_flag = 0b1;
				}
				result = result << 1;

				if (result == 0x00) {
					flags.z_flag = 0b1;
				}
				if ((result & 0x80) > 0) {
					flags.n_flag = 0b1;
				}

				ram->write(0x00, operand, result);

				increment_pc();
			}
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			increment_pc(); //increment the PC
		}
			break;
		case BCC:
			
			increment_pc();

			//branch on carry flag clear
			if (flags.c_flag == 0b0){
				unsigned char operand = rom->read(pc_high, pc_low);
				if ((operand & 0x80) > 0) {
					//subtraction case, I'll need to convert from signed negative to something I can subtract with
					operand = ~operand; //flip all of the bits in operand
					operand += 1; //do the two's coplement conversion
					if ((pc_low - operand) < 0x00) {
						pc_high -= 0x01;
						pc_low -= operand;
					}
					else {
						pc_low -= operand;
					}
				}
				else {
					//I'm not sure how the signed bit operation works with the program counter, and if carrying occurs, but for now I'll assume it does
					if ((pc_low + operand) > 0xFF) {
						pc_high += 0x01;
						pc_low += operand;
					}
					else {
						pc_low += operand;
					}
				}
			}
			else {
				increment_pc();
			}
			break;
		case BCS:
			increment_pc();

			//branch on carry flag clear
			if (flags.c_flag == 0b1) {
				unsigned char operand = rom->read(pc_high, pc_low);
				if ((operand & 0x80) > 0) {
					//subtraction case, I'll need to convert from signed negative to something I can subtract with
					operand = ~operand; //flip all of the bits in operand
					operand += 1; //do the two's coplement conversion
					if ((pc_low - operand) < 0x00) {
						pc_high -= 0x01;
						pc_low -= operand;
					}
					else {
						pc_low -= operand;
					}
				}
				else {
					//I'm not sure how the signed bit operation works with the program counter, and if carrying occurs, but for now I'll assume it does
					if ((pc_low + operand) > 0xFF) {
						pc_high += 0x01;
						pc_low += operand;
					}
					else {
						pc_low += operand;
					}
				}
			}
			else {
				increment_pc();
			}
			break;
		case BEQ:
			break;
		case BIT:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case BMI:

			break;
		case BNE:
			break;
		case BPL:
			break;
		case BRK:
			break;
		case BVC:
			break;
		case BVS:
			break;
		case CLC:
			break;
		case CLD:
			break;
		case CLI:
			break;
		case CLV:
			break;
		case CMP:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case CPX:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case CPY:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case DEC:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case DEX:
			break;
		case DEY:
			break;
		case EOR:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case INC:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case INX:
			break;
		case INY:
			break;
		case JMP:
			break;
		case JSR:
			break;
		case LDA:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case LDX:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case LDY:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case LSR:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case NOP:
			break;
		case ORA:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case PHA:
			break;
		case PHP:
			break;
		case PLA:
			break;
		case PLP:
			break;
		case ROL:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case ROR:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case RTI:
			break;
		case RTS:
			break;
		case SBC:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case SEC:
			break;
		case SED:
			break;
		case SEI:
			break;
		case STA:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case STX:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case STY:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				break;
			case ABSOLUTE_X:
				break;
			case ABSOLUTE_Y:
				break;
			case IMMEDIATE:
				break;
			case IMPLIED:
				break;
			case INDIRECT:
				break;
			case INDIRECT_X:
				break;
			case INDIRECT_Y:
				break;
			case RELATIV:
				break;
			case ZEROPAGE:
				break;
			case ZEROPAGE_X:
				break;
			case ZEROPAGE_Y:
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			break;
		case TAX:
			break;
		case TAY:
			break;
		case TSX:
			break;
		case TXA:
			break;
		case TXS:
			break;
		case TYA:
			break;
		case JAM:
			state = JAMMED; //jam the processor state
			break;
		}
		if (state != JAMMED) {
			state = FETCH;
		}
		increment_pc(); //increase the pc for the next instruction
	} else {
		//state error correcting here, 
	}
}

/// <summary>
/// This function is used to increment the Program Counter registers, it also handles any cases for overflow
/// I think it could be possible to just use ++ for the increment, as it should overflow, however I've decided to play it safe for now
/// </summary>
void Processor::increment_pc() {
	if (pc_low + 0x01 > 0xFF) {
		pc_low = 0x00;
		if (pc_high + 0x01 > 0xFF) {
			pc_high = 0x00;
		}
		else {
			pc_high += 0x01;
		}
	}
	else {
		pc_low += 0x01;
	}
}

/// <summary>
/// reset function, clears the memory and resets the processor to initial status
/// </summary>
void Processor::reset() {
	ram->clearMemory();
	rom->clearMemory();
	flags.val = 0x00;
	a_reg = 0x00;
	x_reg = 0x00;
	y_reg = 0x00;
	sp_reg = 0x00;
	pc_high = 0x00;
	pc_low = 0x00;
}
