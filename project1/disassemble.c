#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000
#define LABELNAME 7
#define MAXLABELS 60

int readLine(FILE *, char *);
int convertNum(int);
char *opcodes[8] = {"add", "nor", "lw", "sw", "beq", "jalr", "halt", "noop"};

int main(int argc, char *argv[])
{    
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    int pc, opLength;
    char line[MAXLINELENGTH];
    int op;
    int opcode, arg0, arg1, arg2;

    if (argc != 4) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];
    opLength = atoi(argv[3]);

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

    for (pc = 0; pc < opLength; pc++) {

        op = readLine(inFilePtr, line);
        if (op == -1) break;

        opcode = op >> 22 & 7;
        arg0 = op >> 19 & 7;
        arg1 = op >> 16 & 7;
        arg2 = convertNum(op & 65535);

        fprintf(outFilePtr, "\t%s", opcodes[opcode]);
        if (opcode <= 5) fprintf(outFilePtr, "\t%d\t%d", arg0, arg1);
        if (opcode <= 4) fprintf(outFilePtr, "\t%d", arg2);
        fprintf(outFilePtr, "\n");
    }

    while((op = readLine(inFilePtr, line)) != -1) {

        fprintf(outFilePtr, "\t.fill\t%d\n", op);
    }

    rewind(inFilePtr);
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readLine(FILE *inFilePtr, char *line)
{    
    char *ptr = line;
    /* delete prior values */
    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) != NULL) {
        /* reached end of file */
        return(atoi(line));
    }
    return(-1);
}

int convertNum(int num) {
    if (num & (1<<15) ) num -= (1<<16);
    return(num);
}