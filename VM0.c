#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500 
#define MAX_LEXI_LEVELS 3

typedef struct{
	int OP;
	int L;
	int M;
} instructions;


void LIT(int stack[], int SP, int *pSP, int M);
void OPR(int stack[], int SP, int *pSP, int BP, int *pBP, int PC, int *pPC, int M);
void LOD(int stack[], int SP, int *pSP, int BP, int L, int M);
void STO(int stack[], int SP, int *pSP, int BP, int L, int M);
void CAL(int stack[], int SP, int BP, int *pBP, int PC, int *pPC, int L, int M);
void INC(int SP, int *pSP, int M);
void JMP(int PC, int *pPC, int M);
void JPC(int stack[], int SP, int *pSP, int PC, int *pPC, int M);
int base(int l, int base, int stack[]);
void PrintCodebase(instructions code_store[], int IC, FILE *ofp);
void printCodeForStacktrace(instructions code_store[], int PC, FILE *ofp);
void printStacktrace(int stack[], instructions code_store[], int PC, int BP, int SP, FILE *ofp);




char *opCodes[] = {"", "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio", "sio"};



int main(){
	int SP = 0; //stack pointer
	int BP = 1; //base pointer
	int PC = 0; //program counter
	int IR = 0; //instruction register

	int stack[MAX_STACK_HEIGHT] = { 0 };
	instructions code_store[MAX_CODE_LENGTH];

	int IC = 0; //instruction counter 
	int OP = 0; //operation code
	int L = 0; //lexicographical level
	int M = 0; //parameter
	FILE * ifp = fopen("/Users/AdamLevy/Documents/Senior_Year/COP3402/Project/mcode.txt", "r");
	while (fscanf(ifp, "%d", &code_store[IC].OP) != EOF){
		
		printf("BEFORE\n");
		//fscanf(ifp, "%d", &code_store[IC].OP);
		fscanf(ifp, "%d", &code_store[IC].L);
		fscanf(ifp, "%d", &code_store[IC].M);
		IC = IC+1;
		printf("AFTER\n");

		
	}

	fclose(ifp);
	FILE * ofp = fopen("/Users/AdamLevy/Documents/Senior_Year/COP3402/Project/stacktrace.txt", "w");

	PrintCodebase(code_store, IC, ofp);

	fprintf(ofp, "\t\t\t\tpc\t bp\t sp\t stack\t\n");
	fprintf(ofp, "Initial values \t\t\t0\t 1\t 0\t\n");

	while(1){
		printCodeForStacktrace(code_store, PC, ofp);
		switch(code_store[PC].OP){
			case 1: //LIT
				LIT(stack, SP, &SP, code_store[PC].M);
				PC++;
				break;
			case 2: //OPR
				OPR(stack, SP, &SP, BP, &BP, PC, &PC, code_store[PC].M);
				PC++;
				break;
			case 3: //LOD
				LOD(stack, SP, &SP, BP, code_store[PC].L, code_store[PC].M);
				PC++;
				break;
			case 4: //STO
				STO(stack, SP, &SP, BP, code_store[PC].L, code_store[PC].M);
				PC++;
				break;
			case 5: //CAL
				CAL(stack, SP, BP, &BP, PC, &PC, code_store[PC].L, code_store[PC].M);
				break;
			case 6: //INC
				INC(SP, &SP, code_store[PC].M);
				PC++;
				break;
			case 7: //JMP
				JMP(PC, &PC, code_store[PC].M);
				break;
			case 8: //JPC
				JPC(stack, SP, &SP, PC, &PC, code_store[PC].M);
				PC++;
				break;
			case 9: //SIO 1
				printf("%d", stack[SP]);
				SP--;
				PC++;
				break;
			case 10: //SIO 2
				SP++;
				PC++;
				break;
			case 11: //SIO 3
				PC = 0;
				printStacktrace(stack, code_store, 0, 0, 0, ofp);
				return 0;
				break;
			default:
				break;
		}

		printStacktrace(stack, code_store, PC, BP, SP, ofp);
	}


	return 0;
}

void LIT(int stack[], int SP, int *pSP, int M){
	SP++;
	stack[SP] = M;
	*pSP = SP;
}

void OPR(int stack[], int SP, int *pSP, int BP, int *pBP, int PC, int *pPC, int M){
	switch(M){
		case 0:
			SP = BP -1;
			PC = stack[SP+4];
			BP = stack[SP+3];
			break;
		case 1:
			stack[SP] = -1 * stack[SP];
			break;
		case 2:
			stack[SP-1] = stack[SP] + stack[SP -1];
			SP--;
			break;
		case 3:
			stack[SP-1] = stack[SP] - stack[SP-1];
			SP--;
			break;
		case 4:
			stack[SP-1] = stack[SP] * stack[SP-1];
			SP--;
			break;
		case 5:
			stack[SP-1] = stack[SP] / stack[SP-1];
			SP--;
		case 6:
			if(stack[SP] % 2 == 0){
				stack[SP] = 0;
			}
			else{
				stack[SP] = 1;
			}
			break;
		case 7:
			stack[SP-1] = stack[SP] % stack[SP-1];
			SP--;
			break;
		case 8:
			if(stack[SP] == stack[SP-1]){
				stack[SP -1] = 1;
			}
			else{
				stack[SP-1] = 0;
			}
			SP--;
			break;
		case 9:
			if(stack[SP] == stack[SP-1]){
				stack[SP -1] = 0;
			}
			else{
				stack[SP-1] = 1;
			}
			SP--;
			break;
		case 10:
			if(stack[SP] > stack[SP-1]){
				stack[SP -1] = 1;
			}
			else{
				stack[SP-1] = 0;
			}
			SP--;
			break;
		case 11:
			if(stack[SP] >= stack[SP-1]){
				stack[SP -1] = 1;
			}
			else{
				stack[SP-1] = 0;
			}
			SP--;
			break;
		case 12:
			if(stack[SP] < stack[SP-1]){
				stack[SP -1] = 1;
			}
			else{
				stack[SP-1] = 0;
			}
			SP--;
			break;
		case 13:
			if(stack[SP] <= stack[SP-1]){
				stack[SP -1] = 1;
			}
			else{
				stack[SP-1] = 0;
			}
			SP--;
			break;
		default:
			printf("DEFAULT CASE IN ALU OPS.\n");
			break;
	}

	*pSP = SP;
	*pPC = PC;
	*pBP = BP;
}

void LOD(int stack[], int SP, int *pSP, int BP, int L, int M){
	SP++;
	stack[SP] = stack[base(L, BP, stack) + M];
	*pSP = SP;
}

void STO(int stack[], int SP, int *pSP, int BP, int L, int M){
	stack[base(L, BP, stack) + M] = stack[SP];
	SP--;
	*pSP = SP;
}

void CAL(int stack[], int SP, int BP, int *pBP, int PC, int *pPC, int L, int M){
	stack[SP+1] = 0;
	stack[SP+2] = base(L, BP, stack);
	stack[SP+3] = BP;
	stack[SP+4] = PC;
	BP = SP + 1;
	PC = M;

	*pBP = BP;
	*pPC = PC;

}

void INC(int SP, int *pSP, int M){
	SP = SP + M;
	*pSP = SP;
}

void JMP(int PC, int *pPC, int M){
	PC = M;
	*pPC = M;
}

void JPC(int stack[], int SP, int *pSP, int PC, int *pPC, int M){
	if(stack[SP] == 0){
		PC = M;
	}
	SP--;
	*pPC = PC;
	*pSP = SP;
}


int base(int l, int base, int stack[]){
        int b1 = base; // find base L levels down
        while (l > 0) {
            b1 = stack[b1 + 1];
            l--;
        }
        return b1;
}





void PrintCodebase(instructions code_store[], int IC, FILE *ofp){
	fprintf(ofp, "Line\t OP\t L\t M\t\n");
	int i = 0;
	for(i = 0; i<IC; i++){
		fprintf(ofp, "%d\t %s\t %d\t %d\t\n", i, opCodes[code_store[i].OP], code_store[i].L, code_store[i].M);
	}
	fprintf(ofp, "\n");
}


void printCodeForStacktrace(instructions code_store[], int PC, FILE *ofp){
	fprintf(ofp, "%d\t %s\t %d\t %d\t", PC, opCodes[code_store[PC].OP], code_store[PC].L, code_store[PC].M);
}

void printStacktrace(int stack[], instructions code_store[], int PC, int BP, int SP, FILE *ofp){
	//printf("\nLINE:%d\nPC:%d\n", line, PC);
	
	fprintf(ofp, "%d\t %d\t %d\t ", PC, BP, SP);
	int i = 1;
	for(i= 1; i<=SP; i++){
		fprintf(ofp, "%d ", stack[i]);
	}
	fprintf(ofp, "\n");
	
}











