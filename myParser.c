#include <stdio.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_SIZE 500
#define MAX_LEXI 3

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
} opCodes;

typedef struct symbol {
    int kind;       // Const = 1, Var = 2, Proc = 3;
    char name[12];  // Name up to 11 chars
    int value;      // Value
    int level;      // L level
    int addr;       // M value
} symbol;

typedef struct codeStruct {
    int  op; // opcode
    int l; // lexicographical level
    int m; // modifier
} code;


symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
code codeList[MAX_CODE_SIZE];

FILE *input;
int currToken = 0;
int sTableCount = 0;
int mCount = 4;
int curLexLevel = 0;
int cx = 0;


void getToken();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
int isInSymTable(char temp[12]);
void error();
void emit(int op, int l, int m);
void printSymbolTable();
void printCode();


int main(void) {
    input = fopen("tokenlist.txt", "r");
    if (input == NULL) {
        printf("**ERROR READING INPUT FILE**");
        return 0;
    }
    
    getToken();
    block();
    
    printSymbolTable();
    
    if (currToken != periodsym)
        error(9);
    
    emit(SIO, 0, 2);
    
    printCode();
    
    return 0;
}


void getToken() {
    fscanf(input, "%d ", &currToken);
    return;
}


void block() {
    
    int mCount = 4;
    int jmpCX = cx;
    emit(JMP, 0, -99);
    //Constant
    if (currToken == constsym) {
        do {
            //Scans in indent
            getToken();
            if (currToken != identsym)
                error(4);
            
            //Scans in variable name and adds it to Symbol Table
            symbolTable[sTableCount].kind = 1;
            fscanf(input, "%s", symbolTable[sTableCount].name);
            
            //Scans in equal symbol
            getToken();
            if (currToken != eqsym) {
                if (currToken == becomessym) {
                    error(1);
                }
                error(3);
            }
            
            //Scans in numbersym
            getToken();
            if (currToken != numbersym)
                error(2);
            
            //Adds the value to the symbol Table
            fscanf(input, "%d", &symbolTable[sTableCount].value);
            
            getToken();
            sTableCount++;
        } while (currToken == commasym);
        
        if (currToken != semicolonsym)
            error(5);
        
        getToken();
    }
    
    //Variable
    if (currToken == varsym) {
        do {
            getToken();
            if (currToken != identsym)
                error(4);
            
            symbolTable[sTableCount].kind = 2;
            fscanf(input, "%s", symbolTable[sTableCount].name);
            symbolTable[sTableCount].level = curLexLevel;
            symbolTable[sTableCount].addr = mCount;
            mCount++;
            
            getToken();
            sTableCount++;
        } while (currToken == commasym);
        
        if (currToken != semicolonsym)
            error(5);
        
        getToken();
    }
    
    //Procedure
    while (currToken == procsym) {
        
        getToken();
        if (currToken != identsym)
            error(4);
        
        symbolTable[sTableCount].kind = 3;
        fscanf(input, "%s", symbolTable[sTableCount].name);
        symbolTable[sTableCount].level = curLexLevel;
        symbolTable[sTableCount].addr = cx;
        mCount++;
        sTableCount++;
        
        getToken();
        if (currToken != semicolonsym)
            error(5);
        
        getToken();
        curLexLevel++;
        block();
        curLexLevel--;
        emit(OPR, 0, OPR_RTN);
        
        if(currToken != semicolonsym)
            error(5);
        
        getToken();
    }
    codeList[jmpCX].m = cx;
    
    emit(INC, 0, mCount);
    
    statement();
}


void statement() {
    char temp[12];
    int result = 0;
    //Identsym
    if (currToken == identsym) {
        fscanf(input, "%s", temp);
        
        result = isInSymTable(temp);
        
        if (result == -1)
            error(11);
        else if (symbolTable[result].kind != 2)
            error(12);
        
        int tempM = symbolTable[result].addr;
        
        getToken();
        if (currToken != becomessym)
            error(13);
        
        getToken();
        expression();
        emit(STO, curLexLevel - symbolTable[result].value, tempM);
    }
    
    //Callsym
    else if (currToken == callsym) {
        getToken();
        if (currToken != identsym)
            error(14);
        
        fscanf(input, "%s", temp);
        result = isInSymTable(temp);
        emit(CAL, curLexLevel - symbolTable[result].value, symbolTable[result].addr);
        getToken();
    }
    
    //Beginsym
    else if (currToken == beginsym) {
        getToken();
        statement();
        
        while (currToken == semicolonsym) {
            getToken();
            statement();
        }
        
        if (currToken != endsym) {
            printf("\ntoken: %d\n", currToken);
            error(17);
        }
        
        getToken();
    }
    
    //Ifsym
    else if (currToken == ifsym) {
        getToken();
        condition();
        
        int currCX = cx;
        emit(JPC, 0, -99);
        
        if (currToken != thensym)
            error(16);
        
        getToken();
        statement();
        
        if (currToken == elsesym) {
            int elseCX = cx;
            emit(JMP, 0, -99);
            codeList[currCX].m = cx;
            getToken();
            statement();
            codeList[elseCX].m = cx;
        }
        
        else {
            codeList[currCX].m = cx;
        }
    }
    
    //Whilesym
    else if (currToken == whilesym) {
        getToken();
        
        int aCX = cx;
        
        condition();
        
        int bCX = cx;
        emit(JPC, 0, -99);
        
        if (currToken != dosym)
            error(18);
        
        getToken();
        statement();
        emit(JPC, 0, aCX);
        codeList[bCX].m = cx;
    }
}


void condition() {
    
    if (currToken == oddsym) {
        getToken();
        expression();
        emit(OPR, 0, OPR_ODD);
    }
    
    else {
        expression();
        
        if (currToken < 9 || currToken > 14 )
            error(20);
        
        int addOP = currToken;
        getToken();
        expression();
        emit(OPR, 0, addOP - 1);
    }
}


void expression() {
    int addOP;
    
    if (currToken == plussym || currToken == minussym) {
        addOP = currToken;
        
        getToken();
        term();
        
        if (addOP == minussym)
            emit(OPR, 0, OPR_NEG);
    }
    
    else {
        term();
    }
    
    while(currToken == plussym || currToken == minussym) {
        addOP = currToken;
        
        getToken();
        term();
        
        if (addOP == plussym)
            emit(OPR, 0, OPR_ADD);
        else
            emit(OPR, 0, OPR_SUB);
    }
}


void term() {
    int mulOP;
    
    factor();
    
    while (currToken == multsym || currToken == slashsym) {
        mulOP = currToken;
        
        getToken();
        factor();
        
        if(mulOP == multsym)
            emit(OPR, 0, OPR_MUL);
        else
            emit(OPR, 0, OPR_DIV);
    }
}


void factor() {
    int result;
    char tempName[12];
    
    if (currToken == identsym) {
        fscanf(input, "%s", tempName);
        result = isInSymTable(tempName);
        
        if (symbolTable[result].kind == 1) {
            emit(LIT, 0, symbolTable[result].value);
        }
        
        else if (symbolTable[result].kind == 2) {
            emit(LOD, 0, symbolTable[result].addr);
        }
        
        else if (symbolTable[result].kind == 0) {
            error(11);
        }
        
        else if (symbolTable[result].kind == 3) {
            error(21);
        }
        
        getToken();
    }
    
    else if (currToken == numbersym) {
        fscanf(input, "%d", &result);
        emit(LIT, 0, result);
        getToken();
    }
    
    else if (currToken == lparentsym) {
        getToken();
        expression();
        
        if (currToken != rparentsym)
            error(22);
        
        getToken();
    }
    
    else {
        error(23);
    }
}


int isInSymTable(char temp[12] ) {
    int i;
    for (i = 0; i < sTableCount; i++) {
        if (strcmp(temp, symbolTable[i].name) == 0) {
            return i;
        }
    }
    return -1;
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
            // case 6:
            //     printf("Incorrect symbol after procedure declaration.\n");
            //     break;
            // case 7:
            //     printf("Statement Expected.\n");
            //     break;
            // case 8:
            //     printf("Incorrect symbol after statement part in block.\n");
            //     break;
        case 9:
            printf("Period expected.\n");
            break;
            // case 10:
            //     printf("Semicolon between statements missing.\n");
            //     break;
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
            // case 15:
            //     printf("Call of a constant or variable is meaningless.\n");
            //     break;
        case 16:
            printf("then expcted.\n");
            break;
        case 17:
            printf("Semicolon or } expected.\n");
            break;
        case 18:
            printf("do expected.\n");
            break;
            // case 19:
            //     printf("Incorrect symbol following statement.\n");
            //     break;
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
            // case 24:
            //     printf("An expression canot begin with this symbol.\n");
            //     break;
        case 25:
            printf("This number is too large.\n");
            break;
        default:
            printf("Error. Not a valid code Error value.\n");
            break;
    }
}


void emit(int op, int l, int m) {
    if (cx > MAX_CODE_SIZE) {
        error(25);
    } else {
        codeList[cx].op = op; // opcode
        codeList[cx].l = l; // lexicographical level
        codeList[cx].m = m; // modifier
        cx++;
    }
}


void printSymbolTable(){
    int i;
    printf("Name \t Type \t Level \t Value\n");
    for(i = 0; i < sTableCount; i++){
        char type[6] = "";
        if(symbolTable[i].kind == 1){
            strcpy(type, "const");
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].value);
        }
        else if(symbolTable[i].kind == 2){
            strcpy(type, "var");
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
        }
        else if(symbolTable[i].kind == 3){
            strcpy(type, "proc");
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
        }
        
    }
}


void printCode() {
    FILE *mcode = fopen("mcode.txt", "w");
    int i;
    for (i = 0; i < cx; i++) {
        fprintf(mcode, "%d %d %d\n", codeList[i].op, codeList[i].l, codeList[i].m);
    }
    fclose(mcode);
}