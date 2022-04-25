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
	sp_reg = 0xFF; //set to FF as per Stack Pointer operation (page 2 FF to 00) https://www.cs.jhu.edu/~phi/csf/slides/lecture-6502-stack.pdf

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
	sp_reg = 0xFF; //set to FF as per Stack Pointer operation (page 2 FF to 00) https://www.cs.jhu.edu/~phi/csf/slides/lecture-6502-stack.pdf

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
/// Standard Get Function for Accumulator
/// </summary>
/// <returns></returns>
unsigned char Processor::get_accumulator() {
	return a_reg;
}


unsigned char Processor::get_x() {
	return x_reg;
}


unsigned char Processor::get_y() {
	return y_reg;
}


unsigned char Processor::get_pc_high() {
	return pc_high;
}


unsigned char Processor::get_pc_low() {
	return pc_low;
}


unsigned char Processor::get_output() {
	return output;
}


unsigned char Processor::get_sflags() {
	return flags.val;
}

unsigned char Processor::get_sp() {
	return sp_reg;
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
		inst = instruction_table[curr_instruction.nib_low][curr_instruction.nib_high];
		addr_mode = address_table[curr_instruction.nib_low][curr_instruction.nib_high];

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
			{
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
				break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
				break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
				break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
				break;
			case INDIRECT_Y: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
				break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
				break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
				break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}

			if (flags.d_flag == 0) {
				//the 6502 has an interesting overflow flag, and it's relationship with the other flags. Basically, it checks to see if overflow occurs on SIGNED math, rather than unsigned, meaning I can't just check for a normal overflow for 
				//using the formula from http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html, we can implement it in C++ rather easily, this should allow the addition to take place
				/*if (!(a_reg ^ operand) && (a_reg ^ (a_reg + operand))) {
					
				}
				//As far as I'm aware, the carry bit should be pretty simple, we just need to factor in the current carry bit, and determine whether we got a carry bit, did it in ternary because it's nice and clean
				if ((int)(a_reg + operand + ((flags.c_flag == 0b1) ? 0x01 : 0x00) > 0xFF)) {
					flags.c_flag = 0b1; //set carry
				}
				else {
					flags.c_flag = 0b0; //clear carry, I think this makes sense, as carry should always be the carry bit
				}*/

				//doing a simpler version of it for now
				if ((operand & 0x80) > 0) {
					if ((a_reg & 0x80) > 0) {
						//both negative
						if (((a_reg - operand) & 0x80) > 0) {
							flags.o_flag = 0b1;
						}
						else {
							flags.o_flag = 0b0;
						}
					}
				}
				else {
					if (a_reg < 0x80) { //accomplishes the same thing as & 0x80 > 0, but 
						if (((int)a_reg + operand) > 0xFF) {
							flags.c_flag = 0b1;
							flags.o_flag = 0b1;

						}
						else {
							flags.c_flag = 0b0;
							flags.o_flag = 0b0;
						}
					}
				}
				a_reg += operand; //do the addition for the actual value in the accumulator

				//checks for n and z flags
				if (a_reg == 0x00) {
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
			case ABSOLUT:
			{
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
			break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case INDIRECT_Y: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
						 break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
						   break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
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
			if (flags.c_flag == 0b0) {
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
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);

			if (flags.z_flag == 0b1) {
				//branch on flag being set, do a relative address mode 
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
				increment_pc(); //no need to branch, it just needs to continue on
			}
		}
		break;
		case BIT:
		{
			increment_pc();
			unsigned char operand;
			switch (addr_mode) {
			case ABSOLUT:
			{
				unsigned char offset_l = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char offset_h = rom->read(pc_high, pc_low);
				operand = rom->read(offset_h, offset_l);
			}
			break;
			case ZEROPAGE:
				operand = rom->read(0x00, rom->read(pc_high, pc_low));
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}

			//regardless of addressing mode, do the operation
			if (operand & 0x80 > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag = 0b0;
			}
			if (operand & 0x40 > 0) {
				flags.o_flag = 0b1;
			}
			else {
				flags.o_flag = 0b0;
			}
			if (a_reg & operand > 0) {
				flags.z_flag = 0b1;
			}
			else {

			}
			increment_pc(); //increment the pc
		}
		break;
		case BMI:
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);
			if (flags.n_flag == 0b1) {
				//branch on flag being set, do a relative address mode 
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
		}
		break;
		case BNE:
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);

			if (flags.z_flag == 0b0) {
				//branch on flag being set, do a relative address mode 
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
				increment_pc(); //no need to branch, it just needs to continue on
			}
		}
		break;
		case BPL:
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);
			if (flags.n_flag == 0b0) {
				//branch on flag being set, do a relative address mode 
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
		}
		break;
		case BRK:
			break;
		case BVC:
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);
			if (flags.o_flag == 0b0) {
				//branch on flag being set, do a relative address mode 
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
		}
		break;
		case BVS:
		{
			increment_pc();
			unsigned char operand = rom->read(pc_high, pc_low);
			if (flags.o_flag == 0b1) {
				//branch on flag being set, do a relative address mode 
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
		}
		break;
		case CLC:
			flags.c_flag = 0b0;
			increment_pc();
			break;
		case CLD:
			increment_pc();
			flags.d_flag == 0b0;
			break;
		case CLI:
			increment_pc();
			flags.id_flag == 0b0;
			break;
		case CLV:
			increment_pc();
			flags.o_flag == 0b0;
			break;
		case CMP: {
			increment_pc();
			unsigned char operand;
			switch (addr_mode) {
			case ABSOLUT:
			{
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
			break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case INDIRECT_Y: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
						 break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
						   break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
						   break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			unsigned char result = a_reg - operand;
			if ((result & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag = 0b0;
			}
			if (result == 0) {
				flags.z_flag = 0b1;
				flags.c_flag = 0b1;
			}
			if (operand > a_reg) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b0;
			}
			if (a_reg > operand) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b1;
			}
		}
				increment_pc();
				break;
		case CPX: {
			increment_pc();
			unsigned char operand;
			switch (addr_mode) {
			case ABSOLUT: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			unsigned char result = x_reg - operand;
			if ((result & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag = 0b0;
			}
			if (result == 0) {
				flags.z_flag = 0b1;
				flags.c_flag = 0b1;
			}
			if (operand > x_reg) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b0;
			}
			if (x_reg > operand) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b1;
			}
		}
				increment_pc();
				break;
		case CPY:{
			increment_pc();
			unsigned char operand;
			switch (addr_mode) {
			case ABSOLUT: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			unsigned char result = y_reg - operand;
			if ((result & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag = 0b0;
			}
			if (result == 0) {
				flags.z_flag = 0b1;
				flags.c_flag = 0b1;
			}
			if (operand > y_reg) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b0;
			}
			if (y_reg > operand) {
				flags.z_flag = 0b0;
				flags.c_flag = 0b1;
			}
			increment_pc();
		}
			break;
		case DEC: {
			unsigned char addr_high = 0x00;
			unsigned char addr_low = 0x00;
			switch (addr_mode) {
			case ABSOLUT: {
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
			}
				break;
			case ABSOLUTE_X: {
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
			}
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = ram->read(0x00, rom->read(pc_high, pc_low));
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			ram->write(addr_high, addr_low, ram->read(addr_high, addr_low) - 1);

			increment_pc();
		}
			break;
		case DEX:
			x_reg--;
			increment_pc();
			break;
		case DEY:
			y_reg--;
			increment_pc();
			break;
		case EOR: {
			increment_pc();
			unsigned char operand();
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
			//code goes here
		}
			increment_pc();
			break;
		case INC: {
			unsigned char operand = 0x00;
			unsigned char addr_high = 0x00;
			unsigned char addr_low = 0x00;
			switch (addr_mode) {
			case ABSOLUT: {
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
			}
						break;
			case ABSOLUTE_X: {
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
			}
						   break;
			case ZEROPAGE:
				increment_pc();
				addr_low = ram->read(0x00, rom->read(pc_high, pc_low));
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			operand--;
			ram->write(addr_high, addr_low, operand);
			if (operand == 0x00) {
				flags.z_flag = 0b1;
			}
			if ((operand & 0x80) > 1) {
				flags.n_flag = 0b1;
			}
		}
			increment_pc();
			break;
		case INX:
			x_reg++;
			if (x_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			if ((x_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			increment_pc();
			break;
		case INY:
			y_reg++;
			if (y_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			if ((y_reg & 0x80) > 0) {

			}
			break;
		case JMP: {
			increment_pc();
			unsigned char tmpAdd = rom->read(pc_high, pc_low);
			increment_pc();
			pc_high = rom->read(pc_high, pc_low);
			pc_low = tmpAdd;
			//no increment needed here, because it's manually setting the address
		}
				break;
		case JSR: {
			increment_pc();
			ram->write(0x01, sp_reg, pc_high);
			sp_reg--;
			ram->write(0x01, sp_reg, pc_low);
			sp_reg--;
		}
				break;
		case LDA:
		{
			unsigned char operand;
			switch (addr_mode) {
			case ACCUMULATOR:
				operand = a_reg;
				break;
			case ABSOLUT: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT: {
				//basically the same as absolute
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						 break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case INDIRECT_Y:
			{
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
			break;
			case RELATIV: {
				unsigned char addr_high = pc_high;
				unsigned char addr_low = pc_low;
				increment_pc();
				unsigned char addr_mod = rom->read(pc_high, pc_low);
				if (addr_mod >= 0x80) {
					if (((int)addr_low - ((addr_mod & 0x7F) + 1)) < 0) {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
						addr_high--;
					}
					else {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
					}
				}
				else {
					if (((int)addr_low + addr_mod) > 0xFF) {
						addr_low += addr_mod;
						addr_high++;
					}
					else {
						addr_low += addr_mod;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
						 break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
						   break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
						   break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}

			a_reg = operand; //load the accumulator with the value
			increment_pc();
		}
		break;
		case LDX: {
			unsigned char operand;
			switch (addr_mode) {
			case ACCUMULATOR:
				operand = a_reg;
				break;
			case ABSOLUT: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT: {
				//basically the same as absolute
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						 break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case INDIRECT_Y:
			{
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
			break;
			case RELATIV: {
				unsigned char addr_high = pc_high;
				unsigned char addr_low = pc_low;
				increment_pc();
				unsigned char addr_mod = rom->read(pc_high, pc_low);
				if (addr_mod >= 0x80) {
					if (((int)addr_low - ((addr_mod & 0x7F) + 1)) < 0) {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
						addr_high--;
					}
					else {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
					}
				}
				else {
					if (((int)addr_low + addr_mod) > 0xFF) {
						addr_low += addr_mod;
						addr_high++;
					}
					else {
						addr_low += addr_mod;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
						 break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
						   break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
						   break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			x_reg = operand;
		}
			increment_pc();
			break;
		case LDY: {
			unsigned char operand;
			switch (addr_mode) {
			case ACCUMULATOR:
				operand = a_reg;
				break;
			case ABSOLUT: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ABSOLUTE_X: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case ABSOLUTE_Y: {
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						   break;
			case IMMEDIATE:
				increment_pc();
				operand = rom->read(pc_high, pc_low);
				break;
			case INDIRECT: {
				//basically the same as absolute
				increment_pc();
				unsigned char addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				unsigned char addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
			}
						 break;
			case INDIRECT_X: {
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr += x_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
						   break;
			case INDIRECT_Y:
			{
				increment_pc();
				unsigned char addr = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr += y_reg;
				}
				operand = ram->read(0x00, addr); //this could be wrong, according to source ,but the source is a bit confusing, I'll leave it as this for now, as it's 
			}
			break;
			case RELATIV: {
				unsigned char addr_high = pc_high;
				unsigned char addr_low = pc_low;
				increment_pc();
				unsigned char addr_mod = rom->read(pc_high, pc_low);
				if (addr_mod >= 0x80) {
					if (((int)addr_low - ((addr_mod & 0x7F) + 1)) < 0) {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
						addr_high--;
					}
					else {
						addr_low = addr_low - ((addr_mod & 0x7F) + 1);
					}
				}
				else {
					if (((int)addr_low + addr_mod) > 0xFF) {
						addr_low += addr_mod;
						addr_high++;
					}
					else {
						addr_low += addr_mod;
					}
				}
				operand = ram->read(addr_high, addr_low);
			}
						break;
			case ZEROPAGE: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low));
			}
						 break;
			case ZEROPAGE_X: {
				increment_pc();
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + x_reg);
			}
						   break;
			case ZEROPAGE_Y: {
				operand = ram->read(0x00, rom->read(pc_high, pc_low) + y_reg);
			}
						   break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			y_reg = operand;
		}
			increment_pc();
			break;
		case LSR: {
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
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
			increment_pc();
		}
			break;
		case NOP:
			increment_pc();
			break;
		case ORA: {
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
					break;
			case ABSOLUTE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case IMMEDIATE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(pc_high, pc_low);
				break;
			case INDIRECT_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr_low -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr_low += x_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case INDIRECT_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr_low -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr_low += y_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low + x_reg);
				break;
			case ZEROPAGE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low + y_reg);
				operand = ram->read(0x00, addr_low);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}

		}
			break;
		case PHA:
			ram->write(0x01, sp_reg, a_reg);
			sp_reg--;
			break;
		case PHP:
			flags.b_flag = 0b1;
			flags.rsvd = 0b1;
			ram->write(0x01, sp_reg, flags.val);
			sp_reg--;
			break;
		case PLA:
			a_reg = ram->read(0x01, sp_reg);
			sp_reg++;
			increment_pc();
			break;
		case PLP:
			flags.val = ram->read(0x01, sp_reg) & 0xCF;
			sp_reg++;
			increment_pc();
			break;
		case ROL:{
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case IMMEDIATE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(pc_high, pc_low);
				break;
			case INDIRECT_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr_low -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr_low += x_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case INDIRECT_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr_low -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr_low += y_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low + x_reg);
				break;
			case ZEROPAGE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low + y_reg);
				operand = ram->read(0x00, addr_low);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			increment_pc();
		}
			break;
		case ROR: {
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case IMMEDIATE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(pc_high, pc_low);
				break;
			case INDIRECT_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr_low -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr_low += x_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case INDIRECT_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr_low -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr_low += y_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low + x_reg);
				break;
			case ZEROPAGE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low + y_reg);
				operand = ram->read(0x00, addr_low);
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			increment_pc();
		}
			break;
		case RTI:
			flags.val = ram->read(0x01, sp_reg) & 0xCF;
			sp_reg++;
			pc_low = ram->read(0x01, sp_reg);
			sp_reg++;
			pc_high = ram->read(0x01, sp_reg);
			sp_reg++;
			break;
		case RTS:
			pc_low = ram->read(0x01, sp_reg);
			sp_reg++;
			pc_high = ram->read(0x01, sp_reg);
			sp_reg++;
			break;
		case SBC: {
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
			//do the subtraction...I may be missing something slightly here, but I think it should work
			if ((operand & 0x80) > 0) {
				if ((a_reg & 0x80) > 0) {
					a_reg = a_reg + (operand & 0x7F);
				}
				else {
					if ((int)a_reg + (operand & 0x7F) > 0xFF) {
						flags.o_flag = 0b1;
					}
					else {
						flags.o_flag = 0b0;
					}
				}
			}
			else {
				if ((a_reg & 0x80) > 0){
					
				}
			}
			if (a_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((a_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag = 0b0;
			}
		}
			break;
		case SEC:
			flags.c_flag = 0b1;
			increment_pc();
			break;
		case SED:
			flags.d_flag = 0b1;
			increment_pc();
			break;
		case SEI:
			flags.id_flag = 0b1;
			increment_pc();
			break;
		case STA: {
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case IMMEDIATE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(pc_high, pc_low);
				break;
			case INDIRECT_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr_low -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr_low += x_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case INDIRECT_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr_low -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr_low += y_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand;
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand + x_reg;
				break;
			case ZEROPAGE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand + y_reg;
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			increment_pc();
			ram->write(addr_high, addr_low, a_reg);
		}
			break; {
				unsigned char operand = 0x00;
				unsigned char addr_low = 0x00;
				unsigned char addr_high = 0x00;
				switch (addr_mode) {
				case ACCUMULATOR:
					break;
				case ABSOLUT:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					increment_pc();
					addr_high = rom->read(pc_high, pc_low);
					operand = ram->read(addr_high, addr_low);
					break;
				case ABSOLUTE_X:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					increment_pc();
					addr_high = rom->read(pc_high, pc_low);
					if (x_reg >= 0x80) {
						if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
							addr_high--;
							addr_low = addr_low - ((x_reg & 0x7F) + 1);
						}
						else {
							addr_low = addr_low - ((x_reg & 0x7F) + 1);
						}
					}
					else {
						if ((int)addr_low + x_reg > 0xFF) {
							addr_high++;
							addr_low = addr_low + x_reg;
						}
						else {
							addr_low = addr_low + x_reg;
						}
					}
					operand = ram->read(addr_high, addr_low);
					break;
				case ABSOLUTE_Y:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					increment_pc();
					addr_high = rom->read(pc_high, pc_low);
					if (x_reg >= 0x80) {
						if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
							addr_high--;
							addr_low = addr_low - ((y_reg & 0x7F) + 1);
						}
						else {
							addr_low = addr_low - ((y_reg & 0x7F) + 1);
						}
					}
					else {
						if ((int)addr_low + y_reg > 0xFF) {
							addr_high++;
							addr_low = addr_low + y_reg;
						}
						else {
							addr_low = addr_low + y_reg;
						}
					}
					operand = ram->read(addr_high, addr_low);
					break;
				case IMMEDIATE:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					increment_pc();
					addr_high = rom->read(pc_high, pc_low);
					operand = ram->read(pc_high, pc_low);
					break;
				case INDIRECT_X:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					if (x_reg >= 0x80) {
						addr_low -= ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low += x_reg;
					}
					operand = ram->read(0x00, addr_low);
					break;
				case INDIRECT_Y:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					if (y_reg >= 0x80) {
						addr_low -= ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low += y_reg;
					}
					operand = ram->read(0x00, addr_low);
					break;
				case ZEROPAGE:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					operand = ram->read(0x00, addr_low);
					addr_low = operand;
					break;
				case ZEROPAGE_X:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					operand = ram->read(0x00, addr_low);
					addr_low = operand + x_reg;
					break;
				case ZEROPAGE_Y:
					increment_pc();
					addr_low = rom->read(pc_high, pc_low);
					operand = ram->read(0x00, addr_low);
					addr_low = operand + y_reg;
					break;
				case ERR:
					state = JAMMED; //jam the processor
					break;
				}
				increment_pc();
				ram->write(addr_high, addr_low, x_reg);
			}
			break;
		case STY: {
			unsigned char operand = 0x00;
			unsigned char addr_low = 0x00;
			unsigned char addr_high = 0x00;
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUT:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((x_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((x_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + x_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + x_reg;
					}
					else {
						addr_low = addr_low + x_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case ABSOLUTE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					if ((int)(addr_low - ((y_reg & 0x7F) + 0x01)) < 0) {
						addr_high--;
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
					else {
						addr_low = addr_low - ((y_reg & 0x7F) + 1);
					}
				}
				else {
					if ((int)addr_low + y_reg > 0xFF) {
						addr_high++;
						addr_low = addr_low + y_reg;
					}
					else {
						addr_low = addr_low + y_reg;
					}
				}
				operand = ram->read(addr_high, addr_low);
				break;
			case IMMEDIATE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				increment_pc();
				addr_high = rom->read(pc_high, pc_low);
				operand = ram->read(pc_high, pc_low);
				break;
			case INDIRECT_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (x_reg >= 0x80) {
					addr_low -= ((x_reg & 0x7F) + 1);
				}
				else {
					addr_low += x_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case INDIRECT_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				if (y_reg >= 0x80) {
					addr_low -= ((y_reg & 0x7F) + 1);
				}
				else {
					addr_low += y_reg;
				}
				operand = ram->read(0x00, addr_low);
				break;
			case ZEROPAGE:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand;
				break;
			case ZEROPAGE_X:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand + x_reg;
				break;
			case ZEROPAGE_Y:
				increment_pc();
				addr_low = rom->read(pc_high, pc_low);
				operand = ram->read(0x00, addr_low);
				addr_low = operand + y_reg;
				break;
			case ERR:
				state = JAMMED; //jam the processor
				break;
			}
			increment_pc();
			ram->write(addr_high, addr_low, y_reg);
		}
			break;
		case TAX:
			x_reg = a_reg;
			if (x_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((x_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag - 0b0;
			}
			increment_pc();
			break;
		case TAY:
			y_reg = a_reg;
			if (y_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((y_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag - 0b0;
			}
			increment_pc();
			break;
		case TSX:
			x_reg = sp_reg;
			if (x_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((x_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag - 0b0;
			}
			increment_pc();
			break;
		case TXA:
			a_reg = x_reg;
			if (a_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((a_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag - 0b0;
			}
			increment_pc();
			break;
		case TXS:
			sp_reg = x_reg;
			increment_pc();
			break;
		case TYA:
			a_reg = y_reg;
			if (a_reg == 0x00) {
				flags.z_flag = 0b1;
			}
			else {
				flags.z_flag = 0b0;
			}
			if ((a_reg & 0x80) > 0) {
				flags.n_flag = 0b1;
			}
			else {
				flags.n_flag - 0b0;
			}
			increment_pc();
			break;
		case JAM:
			state = JAMMED; //jam the processor state
			break;
		}
		if (state != JAMMED) {
			state = FETCH;
		}
		//increment_pc(); //increase the pc for the next instruction //not sure if needed at the moment, as I'm currently accounting for this manually in instructions, it's inefficient, but it allows me to avoid decrementing pc on jumps and branches
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

/// <summary>
/// The load_progam function loads a program into the rom, it is a critical part of the entire application, as this is the part where 
/// </summary>
/// <param name="filepath"></param>
void Processor::load_program(const char* filepath) {
	union rom_iterator {
		struct {
			unsigned char high : 8;
			unsigned char low : 8;
		};
		unsigned short full;
	};

	rom_iterator itr;
	itr.full = 0x0000;

	//initialize an input stream
	unsigned char byte_read = 0x00;
	std::ifstream input_file_stream;
	input_file_stream.open(filepath); //open the file from the resulting filepalth, I'll likely put this in a try-catch block at some point
	//byte_read = input_file_stream.get();

	//read each byte and input it into 
	while (!input_file_stream.eof()) {
	//while (byte_read != 0x11){
		byte_read = input_file_stream.get();
		rom->write(itr.high, itr.low, byte_read);
		//byte_read = input_file_stream.get();
		itr.full++;
	}
}

/// <summary>
/// This function is necessary since binary files for the 6502 are in little endian format
/// this function is probaby very inefficient, but it's a quick and dirty fix that will work, I can always replace this algorithm with a more efficient one later
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
unsigned char Processor::little_to_big_endian(unsigned char input) {
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

	byte inputbyte;
	inputbyte.val = input;
	byte outputbyte;

	//flip the bytes
	outputbyte.b0 = inputbyte.b7;
	outputbyte.b1 = inputbyte.b6;
	outputbyte.b2 = inputbyte.b5;
	outputbyte.b3 = inputbyte.b4;
	outputbyte.b4 = inputbyte.b3;
	outputbyte.b5 = inputbyte.b2;
	outputbyte.b6 = inputbyte.b1;
	outputbyte.b7 = inputbyte.b0;
	return outputbyte.val; //returns the now flipped byte
}

void Processor::step() {
	if (state == FETCH) {
		fetch();
		decode();
		execute();
	}
	else {
		state = JAMMED;
	}
}


unsigned int Processor::get_rom_size() {
	return rom->get_size();
}


unsigned int Processor::get_ram_size() {
	return ram->get_size();
}

unsigned char Processor::get_rom_value(unsigned char offsetHigh, unsigned char offsetLow) {
	return rom->read(offsetHigh, offsetLow);
}


unsigned char Processor::get_ram_value(unsigned char offsetHigh, unsigned char offsetLow) {
	return ram->read(offsetHigh, offsetLow);
}