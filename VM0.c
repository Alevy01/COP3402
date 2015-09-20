
#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000  // static height of stack
#define MAX_CODE_LENGTH 500   // static size of code store
#define MAX_LEXI_LEVELS 3

typedef struct{
	int OP;
	int L;
	int M;
} instructions;

char *opCodes[] = {"", "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio", "sio"};

int SP = 0; //stack pointer
int BP = 1; //base pointer
int PC = 0; //program counter
instructions IR; //instruction register
int IC = 0; //instruction counter

int stack[MAX_STACK_HEIGHT] = { 0 };
instructions code_store[MAX_CODE_LENGTH];
int pipe_flags[MAX_CODE_LENGTH] = { 0 };

int base(int l, int base);
void allOperations(instructions IR, FILE *ofp);
void readFile();
void PrintCodebase(FILE *ofp);
void printCodeForStacktrace(FILE *ofp);
void printStacktrace(FILE *ofp);

int main(){

	readFile();
	FILE * ofp = fopen("stacktrace.txt", "w");
	PrintCodebase(ofp);
	fprintf(ofp, "\t\t\t\tpc\t bp\t sp\t stack\t\n");
	fprintf(ofp, "Initial values \t\t\t0\t 1\t 0\t\n");

	while(IC > 0){
		printCodeForStacktrace(ofp);
		if(code_store[PC].OP == 11){
			allOperations(code_store[PC], ofp);
			printStacktrace(ofp);
			break;
		}
		//printf("%d %d %d\n", code_store[PC].OP, code_store[PC].L, code_store[PC].M);
		allOperations(code_store[PC], ofp);
		IC--;
		printStacktrace(ofp);
		
	}

	return 0;
}



void PrintCodebase(FILE *ofp){
	fprintf(ofp, "Line\t OP\t L\t M\t\n");
	int i = 0;
	for(i = 0; i<IC; i++){
		fprintf(ofp, "%d\t %s\t %d\t %d\t\n", i, opCodes[code_store[i].OP], code_store[i].L, code_store[i].M);
	}
	fprintf(ofp, "\n");
}

void printCodeForStacktrace(FILE *ofp){
	fprintf(ofp, "%d\t %s\t %d\t %d\t", PC, opCodes[code_store[PC].OP], code_store[PC].L, code_store[PC].M);
}

void printStacktrace(FILE *ofp){
	//printf("\nLINE:%d\nPC:%d\n", line, PC);
	
	fprintf(ofp, "%d\t %d\t %d\t ", PC, BP, SP);
	int i = 1;
	for(i= 1; i<=SP; i++){
		if(pipe_flags[i] == 1){
			fprintf(ofp, "| ");
		}
		fprintf(ofp, "%d ", stack[i]);
	}
	fprintf(ofp, "\n");
	
}



void readFile(){
	FILE *ifp;
	ifp = fopen("mcode.txt", "r");

	while(!feof(ifp)){
		fscanf(ifp, "%d %d %d ", &code_store[IC].OP, &code_store[IC].L, &code_store[IC].M);
		IC++;
	}
	fclose(ifp);
}




int base(int l, int base) {
    int b1 = base; // find base L levels down
    while (l > 0) {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}


void allOperations(instructions IR, FILE *ofp){
	switch(IR.OP){
		case 1: //LIT
			SP += 1;
			stack[SP] = IR.M;
			PC += 1;
			break;
		case 2:
			switch(IR.M){
				case 0:
					SP = BP -1;
					PC = stack[SP + 4];
					BP = stack[SP + 3];
					pipe_flags[SP + 1] = 0;
					break;
				case 1: //NEG
					stack[SP] = -stack[SP];
					//PC += 1;
					break;
				case 2: //ADD
					SP-=1;
					stack[SP] = stack[SP] + stack[SP + 1];
					break;
				case 3: //SUB
					SP -= 1;
					stack[SP] = stack[SP] - stack[SP +1];
					break;
				case 4: //MULT
					SP -= 1;
					stack[SP] = stack[SP] * stack[SP + 1];
					//PC += 1;
					break;
				case 5: //DIV
					SP -= 1;
					stack[SP] = stack[SP] / stack[SP + 1];
					break;
				case 6: //ODD
					if(stack[SP] % 2 == 0){
						stack[SP] = 0;
					}
					else{
						stack[SP] = 1;
					}
					break;
				case 7:
					SP -= 1;
					stack[SP] = stack[SP] % stack[SP+1];
					break;
				case 8:
					SP -= 1;
					if(stack[SP] == stack[SP + 1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC += 1;
					break;
				case 9:
					SP -= 1;
					if(stack[SP] != stack[SP+1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC += 1;
					break;
				case 10:
					SP -= 1;
					if(stack[SP] < stack[SP+1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC += 1;
					break;
				case 11:
					SP -= 1;
					if(stack[SP] <= stack[SP+1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC += 1;
					break;
				case 12:
					SP -= 1;
					//printf("%d %d\n", stack[SP], stack[SP+1]);
					if(stack[SP] > stack[SP+1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC+= 1;
					break;
				case 13:
					SP -= 1;
					if(stack[SP] >= stack[SP+1]){
						stack[SP] = 1;
					}
					else{
						stack[SP] = 0;
					}
					//PC += 1;
					break;
				default:
					break;
			}

			PC+=1;
			break;
		case 3: //LOD
			SP += 1;
			stack[SP] = stack[base(IR.L, BP)+ IR.M];
			PC++;
			break;
		case 4: //STO
			stack[base(IR.L, BP) +IR.M] = stack[SP];
			SP--;
			PC++;
			break;
		case 5: //CAL
			stack[SP+1] = 0;
			stack[SP+2] = base(IR.L, BP);
			stack[SP+3] = BP;
			stack[SP+4] = PC + 1;
			BP = SP + 1;
			PC = IR.M;
			pipe_flags[SP+1] = 1;
			break;
		case 6: //INC
			SP += IR.M;
			PC += 1;
			break;
		case 7: //JMP
			PC = IR.M;
			break;
		case 8: //JPC
			if(stack[SP] == 0){
				PC = IR.M;
				break;
			}
			SP -= 1;
			PC += 1;
			break;
		case 9: //SIO 1
			printf("%d", stack[SP]);
			SP -= 1;
			PC += 1;
			break;
		case 10: //SIO 2
			scanf("%d", &stack[SP]);
			SP += 1;
			PC += 1;
			break;
		case 11: //SIO 3 
			SP = 0;
			PC = 0;
			BP = 0;
			//exit(0);
			break;
		default:
			break;
	}


}