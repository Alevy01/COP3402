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
void program(FILE *ifp);
void error(int errNumber);
void block (FILE *ifp);
void const_dec(FILE *ifp);
void var_dec(FILE *ifp);
void proc_dec(FILE *ifp);
void statement(FILE *ifp);
void condition(FILE *ifp);
void expression(FILE *ifp);
void term(FILE *ifp);
void factor(FILE *ifp);
void printSymbolTable();
//Global token
typedef struct{
    char string[MAX_STRING_LEN];
    token_type type;
    int numeric;
} token;

typedef struct{
    int op;
    int l;
    int m;
} code;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
token curr_token;
int tokenNum = 0;
int filePosition = 0;

int main(){
    FILE *ifp = fopen("/Users/sagarmistry/Desktop/input.txt", "r");
    
    program(ifp);
    
    
    return 0;
}

void program(FILE *ifp){
    while(!feof(ifp)) {
    getToken(ifp);
    block(ifp);
    if(curr_token.type != periodsym){
        error(9);
    }
    else{
        printf("no errors\n\n\n");
    }}
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
    
    filePosition++;
    /*
     printf("%d ", curr_token.type);
     printf("%s ", curr_token.string);
     printf("%d \n", curr_token.numeric);
     */
    
}


void error(int errNumber){
    /*
     switch(errNumber){
     case 1:
     printf("Use = instead of :=\n");
     case 2:
     printf("= must be followed by a number\n");
     case 3:
     printf("Identifier must be folowed by :=\n");
     case 4:
     printf("Const,Var,Procedure, must be followed by identifier\n");
     case 5:
     printf("Semicolon or comma missing.\n");
     case 6:
     printf("Incorrect symbol after procedure declaration.\n");
     case 7:
     printf("Statement Expected.\n");
     case 8:
     printf("Incorrect symbol after statement part in block.\n");
     case 9:
     printf("Period expected.\n");
     case 10:
     printf("Semicolon between statements missing.\n");
     case 11:
     printf("Undeclared Identifier\n");
     case 12:
     printf("Assignment to constatn or procedure is not allowed.\n");
     case 13:
     printf("Assignment operator expected\n");
     case 14:
     printf("Call must be followed by an identifier.\n");
     case 15:
     printf("Call of a constant or variable is meaningless.\n");
     case 16:
     printf("then expcted.\n");
     case 17:
     printf("Semicolon or } expected.\n");
     case 18:
     printf("do expected.\n");
     case 19:
     printf("Incorrect symbol following statement.\n");
     case 20:
     printf("Relational operator expected.\n");
     case 21:
     printf("Expression must not contain a procedure identifier.\n");
     case 22:
     printf("Right Parenthesis is missing.\n");
     case 23:
     printf("The preceding factor cannot begin with this symbol.\n");
     case 24:
     printf("An expression canot begin with this symbol.\n");
     case 25:
     printf("This number is too large.\n");
     default:
     printf("Error. Not a valid code Error value.\n");
     }
     printf("done\n");*/
}

void block(FILE *ifp){
    
    if(curr_token.type == constsym)
    {
        const_dec(ifp);
    }
    else if(curr_token.type == varsym)
    {
        var_dec(ifp);
    }
    else if(curr_token.type == procsym){
        proc_dec(ifp);
    }
    else {
        statement(ifp);
    }
}

void const_dec(FILE *ifp){
    do{
        getToken(ifp);
        symbol_table[tokenNum].kind = 1;
        if(curr_token.type != identsym){
            error(4);
        }
        strcpy(symbol_table[tokenNum].name,curr_token.string);
        getToken(ifp);
        if(curr_token.type != eqsym){
            error(3);
        }
        getToken(ifp);
        if(curr_token.type == numbersym)
        {
            error(2);
        }
        symbol_table[tokenNum].val = curr_token.numeric;
        getToken(ifp);
        tokenNum++;
        
    }while(curr_token.type == commasym);
    
    if(curr_token.type != semicolonsym){
        error(5);
    }
    printSymbolTable();
}

void var_dec(FILE *ifp){
    do{
        symbol_table[tokenNum].kind = 2;
        getToken(ifp);
        if(curr_token.type != identsym){
            error(4);}
        strcpy(symbol_table[tokenNum].name,curr_token.string);
        symbol_table[tokenNum].level = 0;
        //symbol_table[tokenNum].addr = m;
        //m++;
        tokenNum++;
        getToken(ifp);
    }while(curr_token.type == commasym);
    
    if(curr_token.type != semicolonsym){
        error(2);
    }
    
    getToken(ifp);
    printSymbolTable();
}

void proc_dec(FILE *ifp){
    while(curr_token.type == procsym){
        symbol_table[tokenNum].kind = 3;
        getToken(ifp);
        if(curr_token.type != identsym){
            error(6);
        }
        strcpy(symbol_table[tokenNum].name, curr_token.string);
        tokenNum++;
        if(curr_token.type != semicolonsym){
            error(5);
        }
        getToken(ifp);
        block(ifp);
        
        if(curr_token.type != semicolonsym){
            error(5);
        }
        getToken(ifp);
        block(ifp);
        
        if(curr_token.type != semicolonsym)
        {
            error(5);
        }
        getToken(ifp);
    }
    statement(ifp);
    printSymbolTable();
}

void statement(FILE *ifp){
    if(curr_token.type == identsym){
        getToken(ifp);
        if(curr_token.type != becomessym){
            error(13);
        }
        getToken(ifp);
        expression(ifp);
    }
    else if(curr_token.type == callsym){
        getToken(ifp);
        if(curr_token.type != identsym){
            error(14);
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
            error(8);
        }
        getToken(ifp);
    }
    else if(curr_token.type == ifsym){
        getToken(ifp);
        condition(ifp);
        if(curr_token.type != thensym){
            error(16);
        }
        getToken(ifp);
        statement(ifp);
    }
    else if(curr_token.type == whilesym){
        getToken(ifp);
        condition(ifp);
        if(curr_token.type != dosym){
            error(18);
        }
        getToken(ifp);
        statement(ifp);
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

void expression(FILE *ifp){
    if(curr_token.type == plussym || curr_token.type == minussym){
        getToken(ifp);
        term(ifp);
        do{
            getToken(ifp);
            term(ifp);
        }while(curr_token.type == plussym || curr_token.type == minussym);
    }
}// end expression

void term(FILE *ifp){
    factor(ifp); 
    do{
        getToken(ifp); 
        factor(ifp); 
    }while(curr_token.type == multsym || curr_token.type == slashsym);
}// end term

void factor(FILE *ifp){
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

/*void emit(int op, int l, int m){
	if(cx>CODE_SIZE){
 error(25); 
	}
	else{
 
	}
	
 }*/

void printSymbolTable(){
    int i;
    for(i = 0; i < tokenNum; i++){
        if(symbol_table[i].kind == 1){
            printf("Name %s Kind %s, Level  Value %d\n", symbol_table[i].name, "const", symbol_table[i].val);
        }
        else if(symbol_table[i].kind == 2){
              printf("Name %s Kind %s, Level  Value %d\n", symbol_table[i].name, "var", symbol_table[i].val);
        }
        else if(symbol_table[i].kind == 3){
              printf("Name %s Kind %s, Level  Value %d\n", symbol_table[i].name, "proc", symbol_table[i].val);
        }
    }
}
