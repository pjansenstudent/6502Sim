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
		curr_instruction = rom->read(pc_high, pc_low);

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

		state == EXECUTE;
	}
	else {
		//need to put some error handling for JAM state or wrong state here.
	}
}

/// <summary>
/// The major function of the Processor object, this is where instructions are executed and work is actually done
/// </summary>
void Processor::execute() {
	if (state == EXECUTE)[
		switch (inst) {
		case ADC:
			switch (addr_mode) {
				case ACCUMULATOR:
					break;
				case ABSOLUTE:
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
				case RELATIVE:
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
		case AND:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUTE:
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
			case RELATIVE:
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
		case ASL:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUTE:
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
			case RELATIVE:
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
		case BCC:
			break;
		case BCS:
			break;
		case BEQ:
			break;
		case BIT:
			switch (addr_mode) {
			case ACCUMULATOR:
				break;
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			case ABSOLUTE:
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
			case RELATIVE:
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
			state == JAMMED; //jam the processor state
			break;
		}
		if (state != JAMMED) {
			state = FETCH;
		}
		increment_pc(); //increase the pc for the next instruction
	] else {
		//state error correcting here, 
	}
}


void Processor::increment_pc() {

}