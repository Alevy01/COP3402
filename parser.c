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
#define MAX_SYMBOL_TABLE_SIZE 100
/* For constants, store kind, name and val   For variables, store kind, name, L and M   For procedures, store kind, name, L and M */
typedef struct symbol {  
	int kind;   // const = 1, var = 2, proc = 3 
	char name[12]; // name up to 11 chars 
	int val;   // value 
	int level;  // L level 
	int addr;   // M address
} symbol; 



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


symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
token curr_token;
int tokenNum = 0;

int main(){
	FILE *ifp = fopen("input.txt", "r");
	
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
	if(curr_token.type != identsym){
		getToken(ifp); 
		if(curr_token.type != becomessym){
			error(); 
		}
		getToken(ifp); 
		expression(ifp); 
	}
	else if(curr_token.type == callsym){
		getToken(ifp);
		if(curr_token.type != identsym){
			error(); 
		}
		getToken(ifp); 
	}
	else if(curr_token.type == beginsym){
		getToken(ifp);
		statement(ifp);
		do{
			getToken(ifp); 
			statement(ifp); 
		}while(curr_token.type == semicolonsym);
		if(curr_token.type != endsym)
		{
			error(); 
		}
		getToken(ifp); 
	else if(curr_token.type == ifsym){
		getToken(ifp); 
		condition();
		if(curr_token != thensym){
			error(); 
		}
		getToken(ifp);
		statement(ifp); 
	}
	else if(curr_token.type == whilesym){
		getToken(ifp); 
		condition(ifp); 
		if(curr_token.type != dosym){
			error(); 
		}
		getToken(ifp); 
		statement(ifp); 
	}
	}
}// end statement
void condition(FILE *ifp){
	if(curr_token.type == oddsym){
		getToken(ifp); 
		expression(ifp); 
	}
	else{
		expression(ifp); 
		if(curr_token.type != gtrsym || curr_token.type != lessym || curr_token.type != eqsym || curr_token.type != geqsym || curr_token.type != leqsym || curr_token.type != neqsym){
			error(20); 
		}
		getToken(ifp);
		expression(ifp);
	}
}// end condition

void expression(ifp){
	if(curr_token.type == plussym || curr_token.type == minussym){
		getToken(ifp); 
		term(ifp); 
		do{
			getToken(ifp);
			term(ifp); 
		}while(curr_token.type == plussym || curr_token.type == minussym)
	}
}// end expression

void term(ifp){
	factor(ifp); 
	do{
		getToken(ifp); 
		factor(ifp); 
	}while(curr_token.type == multsym || curr_token.type == slashsym);
}// end term

void factor(ifp){
	if(curr_token.type == identsym){
		getToken(ifp);
	}
	else if(curr_token.type == numbersym){
		getToken(ifp); 
	}
	else if(curr_token.type == lparentsym){
		getToken(ifp); 
		expression(ifp);
	}
	else if(curr_token.type != rparentsym){
		getToken(ifp); 
	}
	else{
		error(23); 
	}
}// end Factor

void emit(int op, int l, int m){
	
}
