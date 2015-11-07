#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
     nulsym = 1, identsym, numbersym, plussym, minussym,
     multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
     gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
     periodsym, becomessym, beginsym, endsym, ifsym, thensym,
     whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
     readsym, elsesym
} token_type;


#define MAX_STRING_LEN 11

void getToken(FILE *ifp);
void error(int errNumber);
void block (FILE *ifp);
void const_dec(FILE *ifp); 
void var_dec(FILE *ifp); 
void proc_dec(FILE *ifp); 
void statement(FILE *ifp);
//Global token
typedef struct{
	char string[MAX_STRING_LEN];
	token_type type;
	int numeric;
} token;


token curr_token;
int tokenNum = 0;

int main(){
	FILE *ifp = fopen("input.txt", "r");
	while(1){
	getToken(ifp);
}
	return 0;
}

void getToken(FILE *ifp){
  
  if(feof(ifp)){
  	curr_token.type = nulsym;
    return;
  }
 

  fscanf(ifp, "%d ", (int*)(&curr_token.type));
 
  if(curr_token.type == identsym){
    fscanf(ifp, "%s ", curr_token.string);
  }
 
  if(curr_token.type == numbersym){
    fscanf(ifp, "%d ", &curr_token.numeric);
  }

  tokenNum++;

  printf("%d\n", curr_token.type);
  printf("%s\n", curr_token.string);
  printf("%d\n", curr_token.numeric);


}

void error(int errNumber){

	switch(int errNumber){
		case 1: 
			printf("Use = instead of :=");
		case 2: 
			printf("= must be followed by a number");
		case 3:
			printf("Identifier must be folowed by :=");
		case 4: 
			printf("Const,Var,Procedure, must be followed by identifier");
		case 5:
			printf("Semicolon or comma missing.");
		case 6: 
			printf("Incorrect symbol after procedure declaration.");
		case 7:
			printf("Statement Expected.");
		case 8:
			printf("Incorrect symbol after statement part in block.");
		case 9:
			printf("Period expected.");
		case 10:
			printf("Semicolon between statements missing.");
		case 11: 
			printf("Undeclared Identifier");
		case 12:
			printf("Assignment to constatn or procedure is not allowed.");
		case 13: 
			printf("Assignment operator expected");
		case 14:
			printf("Call must be followed by an identifier.");
		case 15:
			printf("Call of a constant or variable is meaningless.");
		case 16: 
			printf("then expcted.");
		case 17:
			printf("Semicolon or } expected.");
		case 18: 
			printf("do expected.");
		case 19:
			printf("Incorrect symbol following statement.");
		case 20:
			printf("Relational operator expected."); 
		case 21:
			printf("Expression must not contain a procedure identifier.");
		case 22:
			printf("Right Parenthesis is missing.");
		case 23:
			printf("The preceding factor cannot begin with this symbol.");
		case 24: 
			printf("An expression canot begin with this symbol.");
		case 25: 
			printf("This number is too large.");	
		default:
			printf("Error. Not a valid code Error value"	
	}

}

void block(FILE *ifp){

		if(curr_token.type == constsym)
		{
			const_dec();
		}
		else if(curr_token.type == var_dec)
		{
			var_dec(); 
		}
		else if(curr_token.type == procsym){
			proc_dec(); 
		}
		else {
			statement();
		}
}

void const_dec(FILE *ifp){
	getToken(ifp);
	do{
		if(curr_token.type != identsym){
			error(PUT NUMBER); 
		}
		getToken(ifp); 
		if(curr_token.type != eqsym){
			error(PUT NUMBER); 
		}
		getToken(ifp);
		if(curr_token.type = numbersym)
		{
			error(2);
		}
		getToken(ifp);
	}while(curr_token.type == commasym);
	if(curr_token.type != semicolonsym){
		error(); 
	}
}

void var_dec(FILE *ifp){
	do{
		getToken(ifp);
		if(curr_token.type != identsym){
			error();
		}
		getToken(ifp);
	}while(curr_token.type == commasym);

	if(curr_token.type != semicolonsym){
		error()
	}

	getToken();
}

void proc_dec(FILE *ifp){
	while(curr_token.type == procsym){
		getToken(ifp);
		if(curr_token.type != identsym){
			error();
		}
		if(curr_token.type != identsym){
			error();
		}
		if(curr_token.type != semicolonsym){
			error();
		}
		getToken(ifp);
		block();
		if(curr_token.type != semicolonsym)
		{
			error();
		}
		getToken(ifp);
	}
	statement();
}

void statement(FILE *ifp){

}

