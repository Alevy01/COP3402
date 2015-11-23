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

typedef enum {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO, OPR_RTN,
    OPR_NEG, OPR_ADD, OPR_SUB, OPR_MUL, OPR_DIV, OPR_ODD, OPR_MOD,
    OPR_EQL, OPR_NEQ, OPR_LSS, OPR_LEQ, OPR_GTR, OPR_GEQ
} shortcut;


#define MAX_STRING_LEN 11
#define MAX_SYMBOL_TABLE_SIZE 100
#define LIT 1
#define OPR 2
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8
#define SIO 9
#define OPR_RTN 0
#define OPR_NEG 1
#define OPR_ADD 2
#define OPR_SUB 3
#define OPR_MUL 4
#define OPR_DIV 5
#define OPR_ODD 6
#define OPR_MOD 7
#define OPR_EQL 8
#define OPR_NEQ 9
#define OPR_LSS 10
#define OPR_LEQ 11
#define OPR_GTR 12
#define OPR_GEQ 13
#define MAX_CODE_SIZE 500
#define MAX_LEXI 3

/* For constants, store kind, name and val   For variables, store kind, name, L and M   For procedures, store kind, name, L and M */
typedef struct symbol {
    int kind;   // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val;   // value
    int level;  // L level
    int addr;   // M address
} symbol;


int LexLevel = 0;
int m = 4;

void getToken(FILE *ifp);
void program(FILE *ifp);
void error(int errNumber);
void block (FILE *ifp);
void statement(FILE *ifp);
void condition(FILE *ifp);
void expression(FILE *ifp);
void term(FILE *ifp);
void factor(FILE *ifp);
void emit(int op, int l, int m);
void printSymbolTable();
void printCode();
int getAddrFromSymbol(char tempString[]);
int getValFromSymbol(char tempString[]);
int getSymbolType(char tempString[]);

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
} indCode;

indCode code[MAX_CODE_SIZE];
int cx = 0;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
token curr_token;
int tokenNum = 0;
int filePosition = 0;
char tempString[12];


int main(){
    FILE *ifp = fopen("/Users/sagarmistry/Desktop/input.txt", "r");
    
    program(ifp);
    
    return 0;
}


void program(FILE *ifp){
    while(!feof(ifp)){
        getToken(ifp);
        block(ifp);
    }
    printSymbolTable();
    
    if(curr_token.type != periodsym){
        error(9);
    }
    else{
        printf("no errors\n\n\n");
    }
    printCode();
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
    
    switch(errNumber){
        case 1:
            printf("Use = instead of :=\n");
            break;
        case 2:
            printf("= must be followed by a number\n");
            break;
        case 3:
            printf("Identifier must be folowed by :=\n");
            break;
        case 4:
            printf("Const,Var,Procedure, must be followed by identifier\n");
            break;
        case 5:
            printf("Semicolon or comma missing.\n");
            break;
        case 6:
            printf("Incorrect symbol after procedure declaration.\n");
            break;
        case 7:
            printf("Statement Expected.\n");
            break;
        case 8:
            printf("Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Period expected.\n");
            break;
        case 10:
            printf("Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Undeclared Identifier\n");
            break;
        case 12:
            printf("Assignment to constatn or procedure is not allowed.\n");
            break;
        case 13:
            printf("Assignment operator expected\n");
            break;
        case 14:
            printf("Call must be followed by an identifier.\n");
            break;
        case 15:
            printf("Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("then expcted.\n");
            break;
        case 17:
            printf("Semicolon or } expected.\n");
            break;
        case 18:
            printf("do expected.\n");
            break;
        case 19:
            printf("Incorrect symbol following statement.\n");
            break;
        case 20:
            printf("Relational operator expected.\n");
            break;
        case 21:
            printf("Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("Right Parenthesis is missing.\n");
            break;
        case 23:
            printf("The preceding factor cannot begin with this symbol.\n");
            break;
        case 24:
            printf("An expression canot begin with this symbol.\n");
            break;
        case 25:
            printf("This number is too large.\n");
            break;
        default:
            printf("Error. Not a valid code Error value.\n");
            break;
    }
}

int first = 1;

void block(FILE *ifp){
    
    emit(JMP, 0, 1);
    if(first == 1){
        LexLevel = 0;
        first = 0;
    }
    else{
        LexLevel++;
    }
    
    if(curr_token.type == constsym)
    {
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
            if(curr_token.type != numbersym)
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

    }
    
    else if(curr_token.type == varsym){
        do{
            symbol_table[tokenNum].kind = 2;
            getToken(ifp);
            if(curr_token.type != identsym){
                error(4);}
            strcpy(symbol_table[tokenNum].name,curr_token.string);
            symbol_table[tokenNum].level = LexLevel;
            symbol_table[tokenNum].addr = m;
            m++;
            tokenNum++;
            getToken(ifp);
        }while(curr_token.type == commasym);
        
        if(curr_token.type != semicolonsym){
            error(5);
        }
        
        
        
    }
    else if(curr_token.type == procsym){
        while(curr_token.type == procsym){
            symbol_table[tokenNum].kind = 3;
            getToken(ifp);
            if(curr_token.type != identsym){
                error(6);
            }
            strcpy(symbol_table[tokenNum].name, curr_token.string);
            symbol_table[tokenNum].level = LexLevel;
            symbol_table[tokenNum].addr = m;
            m++;
            tokenNum++;
            getToken(ifp);
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
        emit(INC, 0, m);
        statement(ifp);
        
    }
    else {
        emit(INC, 0, m);
        statement(ifp);
    }
    LexLevel--;
}


void statement(FILE *ifp){
    if(curr_token.type == identsym){
        getToken(ifp);
        if(curr_token.type != becomessym){
            error(13);
        }
        getToken(ifp);
        expression(ifp);
        emit(STO, 0, m);
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
        int currentCX = cx;
        emit(JPC, 0, -99);
        
        if(curr_token.type != thensym){
            error(16);
        }
        getToken(ifp);
        statement(ifp);
        code[currentCX].m = cx;
    }
    else if(curr_token.type == whilesym){
        getToken(ifp);
        int CXbc = cx;
        condition(ifp);
        int CXac = cx;
        emit(JPC, 0, -99);
        
        if(curr_token.type != dosym){
            error(18);
        }
        getToken(ifp);
        statement(ifp);
        emit(JMP, 0, CXbc);
        code[CXac].m = cx;
    }
}// end statement


void condition(FILE *ifp){
    if(curr_token.type == oddsym){
        getToken(ifp);
        expression(ifp);
        emit(OPR, 0, OPR_ODD);
    }
    else{
        expression(ifp);
        if(curr_token.type != gtrsym && curr_token.type != lessym && curr_token.type != eqsym && curr_token.type != geqsym && curr_token.type != leqsym && curr_token.type != neqsym){
            error(20);
        }
        getToken(ifp);
        expression(ifp);
        emit(OPR, 0, curr_token.type-1);
    }
}// end condition


void expression(FILE *ifp){
    if(curr_token.type == plussym || curr_token.type == minussym){
        getToken(ifp);
        if(curr_token.type == minussym)
            emit(OPR, 0, OPR_NEG); //negation
    }
    
    term(ifp);
    while(curr_token.type == plussym || curr_token.type == minussym){
        getToken(ifp);
        term(ifp);
        if(curr_token.type == plussym)
            emit(OPR, 0, OPR_ADD);
        else
            emit(OPR, 0, OPR_SUB);
    }
}// end expression


void term(FILE *ifp){
    factor(ifp);
    while(curr_token.type == multsym || curr_token.type == slashsym){
        getToken(ifp);
        factor(ifp);
        if (curr_token.type == multsym)
            emit(OPR, 0, OPR_MUL);
        else
            emit(OPR, 0, OPR_DIV);
    }
}// end term

int getSymbolType(char tempString[]){
    int i = 0;
    for(i = 0; i<tokenNum; i++){
        if(strcmp(symbol_table[i].name, tempString) == 0){
            return symbol_table[i].kind;
        }
    }
    
    return 0;
}

int getValFromSymbol(char tempString[]){
    int i = 0;
    for(i = 0; i<tokenNum; i++){
        if(strcmp(symbol_table[i].name, tempString) == 0){
            return symbol_table[i].val;
        }
    }
    
    return 0;
}


int getAddrFromSymbol(char tempString[]){
    int i = 0;
    for(i = 0; i<tokenNum; i++){
        if(strcmp(symbol_table[i].name, tempString) == 0){
            return symbol_table[i].addr;
        }
    }
    
    return 0;
}



void factor(FILE *ifp){
    int temp = 0;
    if(curr_token.type == identsym){
        m = getSymbolType(curr_token.string);
        if(m == 0){
            error(11);
        }
        if(m == 1){
            temp = getValFromSymbol(curr_token.string);
            emit(LIT, 0, temp);
        }
        if(m == 2){
            temp = getAddrFromSymbol(curr_token.string);
            emit(LOD, 0, temp);
        }
        if(m == 3){
            error(21);
        }
        
        getToken(ifp);
    }
    else if(curr_token.type == numbersym){
        emit(LIT, 0, curr_token.numeric);
        getToken(ifp);
    }
    else if(curr_token.type == lparentsym){
        getToken(ifp);
        expression(ifp);
        if(curr_token.type != rparentsym){
            error(22);
        }
        getToken(ifp);
    }
    else{
        error(23);
    }
}// end Factor


void emit(int op, int l, int m) {
    if (cx > MAX_CODE_SIZE) {
        error(25);
    } else {
        code[cx].op = op; // opcode
        code[cx].l = l; // lexicographical level
        code[cx].m = m; // modifier
        cx++;
    }
}

void printSymbolTable(){
    int i;
    printf("Name \t Type \t Level \t Value\n");
    for(i = 0; i < tokenNum; i++){
        char type[6] = "";
        if(symbol_table[i].kind == 1){
            strcpy(type, "const");
            printf("%s \t %s \t %d \t %d\n", symbol_table[i].name, type, symbol_table[i].level, symbol_table[i].val);
        }
        else if(symbol_table[i].kind == 2){
            strcpy(type, "var");
            printf("%s \t %s \t %d \t %d\n", symbol_table[i].name, type, symbol_table[i].level, symbol_table[i].addr);
        }
        else if(symbol_table[i].kind == 3){
            strcpy(type, "proc");
            printf("%s \t %s \t %d \t %d\n", symbol_table[i].name, type, symbol_table[i].level, symbol_table[i].addr);
        }
        
    }
}

void printCode() {
    FILE *mcode = fopen("mcode.txt", "w");
    int q;
    for (q = 0; q < cx; q++) {
        fprintf(mcode, "%d %d %d\n", code[q].op, code[q].l, code[q].m);
    }
    fclose(mcode);
}