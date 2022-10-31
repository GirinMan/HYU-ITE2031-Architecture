#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __DEBUG__ 0
#define __CODE__ 0
#define __BIN__ 0
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isValidReg(int reg);
void translateOp(char *opcode, int *dst);
int translateLabel(int lines, char* label);
int translateSymbol(int lines, char* symbol);
void printBinary(unsigned int n, int min);

int directions[MAXLINELENGTH][2]; // 0: flag, 1: value
char labels[MAXLINELENGTH][7];
int mcCodes[MAXLINELENGTH];

int main(int argc, char *argv[]){
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;

	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
	arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	int curLine = 0, maxLines = 0;
	
	
	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
		argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	// Check if each labels are valid or not, if valid, save address of each labels.
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		if(strlen(label) > 6){
			printf("error: label %s with more than 6 characters\n", label);
			exit(1);
		}
		int line = 0;
		if(strlen(label) > 0 && (line = translateLabel(curLine, label)) != -1){
			printf("error: duplicate label %s in line %d and line %d\n",
			label, line, curLine);
			exit(1);
		}
		if(strlen(label) > 0 && !((label[0] >= 65 && label[0] <= 90) || (label[0] >= 97 && label[0] <= 122))){
			printf("error: label %s must not start with a number\n", label);
			exit(1);
		}
		for(int i = 0; i < strlen(label); i++){
			if(!((label[i] >= 48 && label[i] <= 57) || (label[i] >= 65 && label[i] <= 90) || (label[i] >= 97 && label[i] <= 122))){
			printf("error: label %s should consists of numbers or letters\n", label);
			exit(1);
		}

		}
		strcpy(labels[curLine], label);
		curLine++;
	}
	maxLines = curLine;
	curLine = 0;

	/* this is how to rewind the file ptr so that you start reading from the
	beginning of the file */
	rewind(inFilePtr);

	for(int i = 0; i < maxLines; i++){
		directions[i][0] = directions[i][1] = 0;
	}

	// Apply directions for the .fill instructions
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		int op = -1;
		translateOp(opcode, &op);
		if(op < 0){
			printf("error: opcode %s is unrecognized\n", opcode);
			exit(1);
		}
		else if(op == 8){
			mcCodes[curLine] = translateSymbol(maxLines, arg0);
			directions[curLine][0] = 1;
			directions[curLine][1] = mcCodes[curLine];
		}
		curLine++;
	}
	curLine = 0;

	rewind(inFilePtr);

	// Translate each lines into machine language codes
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		int op = -1;
		translateOp(opcode, &op);
		if(op < 0){
			printf("error: opcode %s is unrecognized\n", opcode);
			exit(1);
		}
		else if (op < 2)
		{
			// R-type 
			if(__DEBUG__) {
				printf("[addr: %d] %s R-%s-", curLine, label, opcode);
				printBinary(op, 3);
				printf(" regA: %s regB: %s destReg: %s\n", arg0, arg1, arg2);
			}

			int regA = translateSymbol(maxLines, arg0);
			int regB = translateSymbol(maxLines, arg1);
			int destReg = translateSymbol(maxLines, arg2);
			if(!(isValidReg(regA) && isValidReg(regB) && isValidReg(destReg))){
				printf("error: line %d have invalid register value\n", curLine);
				exit(1);
			}

			mcCodes[curLine] = (op << 22) | (regA << 19) | (regB << 16) | destReg;
		}
		else if (op < 5)
		{
			// I-type
			if(__DEBUG__) {
				printf("[addr: %d] %s I-%s-", curLine, label, opcode);
				printBinary(op, 3);
				printf(" regA: %s regB: %s offsetField: %s\n", arg0, arg1, arg2);
			}

			int regA = translateSymbol(maxLines, arg0);
			int regB = translateSymbol(maxLines, arg1);
			int offset = translateSymbol(maxLines, arg2);
			if(!(isValidReg(regA) && isValidReg(regB))){
				printf("error: line %d have invalid register value\n", curLine);
				exit(1);
			}

			if(op < 4 || isNumber(arg2)){
				if(((offset > 0) && (offset >= 0x00007FFF)) ||
					(offset < 0) && (offset < 0xFFFF8000)){
					printf("error: line %d invalid offset value ", curLine);
					printBinary(offset, 32);
					printf("\n");
					exit(1);
				}
			}
			else{
				offset -= (curLine + 1);
			}
			mcCodes[curLine] = (op << 22) | (regA << 19) | (regB << 16) | (offset & (0x0000FFFF));
		}
		else if (op < 6){
			// J-type	
			if(__DEBUG__) {		
				printf("[addr: %d] %s J-%s-", curLine, label, opcode);
				printBinary(op, 3);
				printf(" regA: %s regB: %s\n", arg0, arg1);
			}

			int regA = translateSymbol(maxLines, arg0);
			int regB = translateSymbol(maxLines, arg1);
			if(!(isValidReg(regA) && isValidReg(regB))){
				printf("error: line %d have invalid register value\n", curLine);
				exit(1);
			}

			mcCodes[curLine] = (op << 22) | (regA << 19) | (regB << 16);
		}
		else if (op < 8){
			// O-type 
			if(__DEBUG__) {
				printf("[addr: %d] %s O-%s-", curLine, label, opcode);
				printBinary(op, 3);
				printf("\n");
			}

			mcCodes[curLine] = op << 22;
		}
		else{
			//.fill direction
			if(__DEBUG__) {
				printf("[addr: %d] %s .fill", curLine, label);
				printf(" %s\n", arg0);
			}
		}

		if(__CODE__) {
			printf("Machine code: %d [Hex] %x\n", mcCodes[curLine], mcCodes[curLine]);
			if(__BIN__){
				printf("[Bin] ");
				printBinary(mcCodes[curLine], 32);
				printf("\n");	
			}
		}	
		curLine++;
	}

	for(int i = 0; i < maxLines; i++){
		fprintf(outFilePtr, "%d\n", mcCodes[i]);
	}
	if(fclose(inFilePtr) != 0){
		printf("error in closing %s\n", inFileString);
		exit(1);
	}
	if(fclose(outFilePtr) != 0){
		printf("error in closing %s\n", outFileString);
		exit(1);
	}
	exit(0);
}

/*
* Read and parse a line of the assembly-language file. Fields are returned
* in label, opcode, arg0, arg1, arg2 (these strings must have memory already
* allocated to them).
*
* Return values:
* 0 if reached end of file
* 1 if all went well
*
* exit(1) if line is too long.
*/
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, 
char *arg1, char *arg2) {
	char line[MAXLINELENGTH];
	char *ptr = line;
	
	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	* Parse the rest of the line. Would be nice to have real regular
	* expressions, but scanf will suffice.
	*/
	sscanf(ptr, 
	"%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", 
	opcode, arg0, arg1, arg2);

	return(1);
}

/* return 1 if string is a number */
int isNumber(char *string){ 
	int i;
	return((sscanf(string, "%d", &i)) == 1);
}

/* Check if given register value is valid or not */
int isValidReg(int reg){
	return (reg < 0 || reg > 7) ? 0 : 1;
}

/* translate opcode into digits */
void translateOp(char *opcode, int *dst){
	if (!strcmp(opcode, "add")){
		*dst = 0;
	}
	else if(!strcmp(opcode, "nor")){
		*dst = 1;
	}
	else if(!strcmp(opcode, "lw")){
		*dst = 2;
	}
	else if(!strcmp(opcode, "sw")){
		*dst = 3;
	}
	else if(!strcmp(opcode, "beq")){
		*dst = 4;
	}
	else if(!strcmp(opcode, "jalr")){
		*dst = 5;
	}
	else if(!strcmp(opcode, "halt")){
		*dst = 6;
	}
	else if(!strcmp(opcode, "noop")){
		*dst = 7;
	}
	else if(!strcmp(opcode, ".fill")){
		*dst = 8;
	}
	else{
		*dst = -1;
	}
}

/* Translate label into memory address.
Return -1 if the label is undefined.
Assume that there aren't any duplicate labels. */
int translateLabel(int lines, char* label){
	int i, found = -1;
	for(i = 0; i < lines; i++){
		if(!strcmp(labels[i], label)){
			found = i;
			break;
		}
	}
	return found;
}

/* Translate a symbol string to integer value.
If it is a number, translate into int.
Else(it is a label), search coresponding address of the label.
If not found, exit with error. */
int translateSymbol(int lines, char* symbol){
	int search;
	if(isNumber(symbol)){
		return(atoi(symbol));
	}
	else{
		search = translateLabel(lines, symbol);
		if(search == -1){
			printf("error: label %s is undefined\n", symbol);
			exit(1);
		}
	}
	return search;

	if(directions[search][0]){
		return directions[search][1];
	}
	else{
		return search;
	}
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