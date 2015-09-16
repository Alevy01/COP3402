#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500 
#define MAX_LEXI_LEVELS 3


//SIO comes from stand input output

typedef struct{
	int OP;
	int L;
	int M;
} instructions;

void LIT(int stack[], int SP, int *pSP, int M);
int OPR(int stack[], int SP, int *pSP, int BP, int *pBP, int PC, int *pPC, int M);
void LOD(int stack[], int SP, int *pSP, int BP, int L, int M);
void STO(int stack[], int SP, int *pSP, int L, int M);


int main(){
	
	int SP = 0; //stack pointer
	int BP = 1; //base pointer
	int PC = 0; //program counter
	int IR = 0; //instruction register

	int stack[MAX_STACK_HEIGHT] = { NULL };
	instructions code_store[MAX_CODE_LENGTH];
	int IC = 0; //instruction counter 
	int OP = 0; //operation code
	int L = 0; //lexicographical level
	int M = 0; //parameter

	FILE * ifp = fopen("/Users/AdamLevy/Documents/Senior_Year/COP3402/Project/mcode.txt", "r");
	
	while (fscanf(ifp,"%d",&OP) != EOF){
		//fscanf(ifp, "%d", &OP);
		fscanf(ifp, "%d", &L);
		fscanf(ifp, "%d", &M);
		//printf("%d %d %d\n", OP, L, M);

		code_store[IC].OP = OP;
		code_store[IC].L = L;
		code_store[IC].M = M;

		printf("%d %d %d\n", code_store[IC].OP, code_store[IC].L, code_store[IC].M);
		IC = IC+1;

		

		if(OP == 11){
			break;
		}
	}

	int pushVal = 0;
	for(int i = 0; i<IC; i++){
		switch(code_store[i].OP){
			case 1: //LIT
				LIT(stack, SP, *SP, code_store[i].M);
				break;
			case 2: //OPR 
				pushVal = OPR(stack, SP, *SP, BP, *BP, PC, *PC, code_store[i].M);
				stack[SP] = pushVal;
				break;
			case 3: //LOD
				LOD(stack, SP, *SP, L, M);
				break;
			case 4: //STO
				break;
			case 5: //CAL
				break;
			case 6: //INC
				break;
			case 7: //JMP
				break;
			case 8: //JMC
				break;
			case 9: //SIO 1
				break;
			case 10: //SIO 2
				break;
			case 11: //SIO 3
				break;
			default:
				break;
		}
	}




	return 0;
}


void LIT(int stack[], int SP, int *pSP, int M){
	*pSP = SP +1;
	stack[SP] = M;
}

int OPR(int stack[], int SP, int *pSP, int BP, int *pBP, int PC, int *pPC, int M){
	int retVal = 0;
	switch(M){
			case 0: //Return from procedure call
				*pSP = BP -1;
				*pPC = stack[SP+4];
				*BP = stack[SP+3];
				break;
			case 1: //NEG
				retVal = stack[SP] * -1;
				*pSP = SP - 1;
				break;
			case 2: //ADD
				retVal = stack[SP] + stack[SP-1];
				*pSP = SP - 1;
				break;
			case 3: //SUB
				retVal = stack[SP] - stack[SP-1];
				*pSP = SP - 1;
				break;
			case 4: //MUL
				retVal = stack[SP] * stack[SP-1];
				*pSP = SP - 1;
				break;
			case 5: //DIV
				retVal = stack[SP]/stack[SP-1];
				*pSP = SP - 1;
				break;
			case 6: //ODD
				if(stack[SP] % 2 == 0){
					retVal = 0;
				}
				else{
					retVal = 1;
				}
				break;
			case 7: //MOD
				retVal = stack[SP] % stack[SP-1];
				*pSP = SP - 1;
				break;
			case 8: //EQL
				if(stack[SP] == stack[SP-1]){
					retVal = 1;
				}
				else{
					retVal = 0;
				}
				*pSP = SP - 1;
				break;
			case 9: //NEQ
				if(stack[SP] == stack[SP-1]){
					retVal = 0;
				}
				else{
					retVal = 1;
				}
				*pSP = SP - 1;
				break;
			case 10: //LSS
				if(stack[SP] > stack[SP-1]){
					retVal = 1;
				}
				else{
					retVal = 0;
				}
				*pSP = SP - 1;
				break;
			case 11: //LEQ
				if(stack[SP] >= stack[SP-1]){
					retVal = 1;
				}
				else{
					retVal = 0;
				}
				*pSP = SP - 1;
				break;
			case 12: //GTR
				if(stack[SP] < stack[SP-1]){
					retVal = 1;
				}
				else{
					retVal = 0;
				}
				*pSP = SP - 1;
				break;
			case 13: //GEQ
				if(stack[SP] <= stack[SP-1]){
					retVal = 1;
				}
				else{
					retVal = 0;
				}
				*pSP = SP - 1;
				break;
			default:
				printf("Hit the default case, somethings wrong.\n");
				break;
		}
	return retVal;
}


void LOD(int stack[], int SP, int *pSP, int BP, int L, int M){
	*pSP = SP + 1;
	int pushVal = stack[base(L, BP) + M];
	stack[SP+1] = pushVal; 
}

void STO(int stack[], int SP, int *pSP, int L, int M){
	stack[base(L) + M] = stack[SP];
	*pSP = SP - 1;
}

int base(int l, int base) {
        int b1 = base; // find base L levels down
        while (l > 0) {
            b1 = stack[b1 + 1];
            l--;
        }
        return b1;
}

