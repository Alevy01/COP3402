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


int main(){
	
	int SP = 0; //stack pointer
	int BP = 1; //base pointer
	int PC = 0; //program counter
	int IR = 0; //instruction register

	int stack[MAX_STACK_HEIGHT] = { NULL };
	instructions code_store[MAX_CODE_LENGTH];
	int IC = 0; //instruction counter 
	int OP = 0; //operation code
	int L = 0; //
	int M = 0; //

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


	for(int i = 0; i<IC; i++){
		switch(code_store[i]){
			case 1: //LIT
				break;
			case 2: //OPR 
				break;
			case 3: //LOD
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


void LIT(int stack[], int SP, int M){
	SP = SP +1;
	stack[SP] = M;
}

