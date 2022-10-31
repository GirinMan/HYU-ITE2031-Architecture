#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;

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
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}

	state.pc = 0;
	for(int i = 0; i < NUMREGS; i++)
		state.reg[i] = 0;

	for(;;){
		printState(&state);
		int op, arg0, arg1, arg2, halted = 0;
		op = (state.mem[state.pc] & (7 << 22)) >> 22;
		arg0 = (state.mem[state.pc] & (7 << 19)) >> 19;
		arg1 = (state.mem[state.pc] & (7 << 16)) >> 16;
		arg2 = state.mem[state.pc] & 0x0000FFFF;

		state.pc++;
		switch(op){
			case 0: // add
				state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
				break;
			case 1: // nor
				state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
				break;
			case 2: // lw
				if((arg2 & (0x8000))){
					arg2 |= 0xFFFF0000;
				}
				state.reg[arg1] = state.mem[state.reg[arg0] + arg2];
				break;
			case 3: // sw
				if((arg2 & (0x8000))){
					arg2 |= 0xFFFF0000;
				}
				state.mem[state.reg[arg0] + arg2] = state.reg[arg1];
				break;
			case 4: // beq
				if(state.reg[arg0] == state.reg[arg1]){
					if((arg2 & (0x8000))){
						arg2 |= 0xFFFF0000;
					}
					state.pc += arg2;
				}
				break;
			case 5: // jalr
				state.reg[arg1] = state.pc;
				state.pc = state.reg[arg0];
				break;
			case 6: // halt
				printf("machine halted\ntotal of %d instructions executed\n", ++instrCnt);
				halted = 1;
				break;
			case 7: // noop
				break;
			default:
				printf("Unknown opcode ");
				printBinary(op, 3);
				printf(" at addres %d\n", state.pc - 1);
				exit(1);
		}

		if(halted)
			break;
		instrCnt++;
	}

	printf("final state of machine:\n");
	printState(&state);

	if(fclose(filePtr) != 0){
		printf("error in closing %s\n", argv[1]);
		exit(1);
	}
	return(0);
}

void printState(stateType *statePtr){
	int i;

	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");

	for (i=0; i<statePtr->numMemory; i++) {
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}

	printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}

	printf("end state\n");
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