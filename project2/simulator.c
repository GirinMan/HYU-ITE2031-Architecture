#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for this project */
#define HALT 6
#define NOOP 7
#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
int opcode(int instruction);

void printInstruction(int instr);
void printState(stateType *);
void printBinary(unsigned int n, int min);

int main(int argc, char *argv[]){
	char line[MAXLINELENGTH];
	stateType state;
	FILE *filePtr;
	int instrCnt = 0;

	if (argc != 2) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}

	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) {
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}

	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
		if (sscanf(line, "%d", &state.instrMem[state.numMemory]) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		state.dataMem[state.numMemory] = state.instrMem[state.numMemory];
		printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
	}
	printf("%d memory words\n", state.numMemory);
	printf("\tinstruction memory:\n");
	for(int i = 0; i < state.numMemory; i++){
		printf("\t\tinstrMem[ %d ] ", i);
		printInstruction(state.instrMem[i]);
	}

	state.pc = 0;
	state.cycles = 0;
	for(int i = 0; i < NUMREGS; i++)
		state.reg[i] = 0;

	state.IFID.instr = NOOPINSTRUCTION;
	state.IDEX.instr = NOOPINSTRUCTION;
	state.EXMEM.instr = NOOPINSTRUCTION;
	state.MEMWB.instr = NOOPINSTRUCTION;
	state.WBEND.instr = NOOPINSTRUCTION;

    while (1) {
        printState(&state); 
		
        /* check for halt */
        if (opcode(state.MEMWB.instr) == HALT) {
            printf("machine halted\n");
            printf("total of %d cycles executed\n", state.cycles);
            exit(0);
        }

        stateType newState = state;
        newState.cycles++;

        /* --------------------- IF stage --------------------- */
		newState.IFID.instr = state.instrMem[state.pc];
		newState.IFID.pcPlus1 = state.pc + 1;
		newState.pc++;

        /* --------------------- ID stage --------------------- */
		int offsetField = field2(state.IFID.instr);

		newState.IDEX.instr = state.IFID.instr;
		newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
		newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
		newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
		newState.IDEX.offset = offsetField & 0x8000 ? offsetField | 0xFFFF0000 : offsetField;		

        /* --------------------- EX stage --------------------- */
		switch(opcode(state.IDEX.instr)){
			case ADD: // add
				newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.readRegB;
				break;
			case NOR: // nor
				newState.EXMEM.aluResult = ~(state.IDEX.readRegA | state.IDEX.readRegB);
				break;
			case LW: // lw
				newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.offset;
				break;
			case SW: // sw
				newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.offset;
				break;
			case BEQ: // beq
				newState.EXMEM.aluResult = state.IDEX.readRegA - state.IDEX.readRegB;
				break;
			case JALR: // jalr
				break;
			case HALT: // halt
				break;
			case NOOP: // noop
				break;
			default:
				printf("Unknown opcode ");
				printBinary(opcode(state.IDEX.instr), 3);
				printf(" at addres %d\n", state.pc - 1);
				exit(1);
		}

		newState.EXMEM.instr = state.IDEX.instr;
		newState.EXMEM.readRegB = state.IDEX.readRegB;
		newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
		

        /* --------------------- MEM stage --------------------- */
		switch(opcode(state.EXMEM.instr)){
			case ADD: // add
				newState.MEMWB.writeData = state.EXMEM.aluResult;
				break;
			case NOR: // nor
				newState.MEMWB.writeData = state.EXMEM.aluResult;
				break;
			case LW: // lw
				newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
				break;
			case SW: // sw
				newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
				break;
			case BEQ: // beq
				if(!state.EXMEM.aluResult)
					newState.pc = state.EXMEM.branchTarget;
				break;
			case JALR: // jalr
				break;
			case HALT: // halt
				break;
			case NOOP: // noop
				break;
			default:
				printf("Unknown opcode ");
				printBinary(opcode(state.IDEX.instr), 3);
				printf(" at addres %d\n", state.pc - 1);
				exit(1);
		}

		newState.MEMWB.instr = state.EXMEM.instr;

        /* --------------------- WB stage --------------------- */
		switch(opcode(state.MEMWB.instr)){
			case ADD: // add
				newState.reg[field2(state.MEMWB.instr) & 7] = state.MEMWB.writeData;
				break;
			case NOR: // nor
				newState.reg[field2(state.MEMWB.instr) & 7] = state.MEMWB.writeData;
				break;
			case LW: // lw
				newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
				break;
			case SW: // sw
				break;
			case BEQ: // beq
				break;
			case JALR: // jalr
				break;
			case HALT: // halt
				break;
			case NOOP: // noop
				break;
			default:
				printf("Unknown opcode ");
				printBinary(opcode(state.IDEX.instr), 3);
				printf(" at addres %d\n", state.pc - 1);
				exit(1);
		}

		newState.WBEND.instr = state.MEMWB.instr;
		newState.WBEND.writeData = state.MEMWB.writeData;

        state = newState; 
        /* this is the last statement before end of the loop.
        It marks the end of the cycle and updates the
        current state with the values calculated in this
        cycle */
    }

	if(fclose(filePtr) != 0){
		printf("error in closing %s\n", argv[1]);
		exit(1);
	}
	return(0);
}

void
printState(stateType *statePtr)
{
	int i;

	printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
	printf("\tpc %d\n", statePtr->pc);

	printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
		printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}

	printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}

	printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
	printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
	printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
	printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
	printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
	return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
	return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
	return(instruction>>22);
}

void
printInstruction(int instr)
{
	char opcodeString[10];
	if (opcode(instr) == ADD) {
		strcpy(opcodeString, "add");
	} else if (opcode(instr) == NOR) {
		strcpy(opcodeString, "nor");
	} else if (opcode(instr) == LW) {
		strcpy(opcodeString, "lw");
	} else if (opcode(instr) == SW) {
		strcpy(opcodeString, "sw");
	} else if (opcode(instr) == BEQ) {
		strcpy(opcodeString, "beq");
	} else if (opcode(instr) == JALR) {
		strcpy(opcodeString, "jalr");
	} else if (opcode(instr) == HALT) {
		strcpy(opcodeString, "halt");
	} else if (opcode(instr) == NOOP) {
		strcpy(opcodeString, "noop");
	} else {
		strcpy(opcodeString, "data");
	}
	printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
	field2(instr));
}

void printBinary(unsigned int n, int min){
	int arr[32] = {0};
	int i = 31;
	int begin = 0;
	while (n) {
		if (n & 1)
			arr[i] = 1;
		else
			arr[i] = 0;
		n >>= 1;
		i--;
	}

	for(i = 0; i < 32; i++){
		if(arr[i] == 1 || i == 32 - min)
			begin = 1;
		if(begin)
			printf("%d", arr[i]);
	}

}