#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
} opCodes;

typedef enum {
    OPR_RTN = 0, OPR_NEG, OPR_ADD, OPR_SUB, OPR_MUL, OPR_DIV,
    OPR_ODD, OPR_MOD, OPR_EQL, OPR_NEQ, OPR_LSS, OPR_LEQ,
    OPR_GTR, OPR_GEQ
} oprCodes;

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

typedef struct {
    int nLine;
    int nOP;
    char cOP[4];
    int nL;
    int nM;
} Node;

symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
code codeList[MAX_CODE_SIZE];


int currToken = 0;
int sTableCount = 0;
int curLexLevel = 0;
int curLine = 1;
int cx = 0;

static const int MAX_STACK_HEIGHT = 2000;
static const int MAX_CODE_LENGTH = 500;
static const int MAX_LEXI_LEVELS = 3;

int pc = 0;
int bp = 1;
int sp = 0;
int ir = 0;

Node* structArray[500];

int stack[2000];
int optionsArray[6]; // 1 == -t , 2 == -s , 3 == -m, 4 == -a, 5 == -v

FILE *input;

void createCleanInput();
int createLexemetable();
void createTokenList();
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
void initialize (FILE *file, FILE *output);
void execute (FILE *output);
void ALU ();
int base (int l, int base);

int main (int argc, char *argv[]) {
    int cmdCounter = 2;
    FILE *inputTXT = fopen(argv[1], "r");
    if(inputTXT == NULL){
        printf("could Not open input file.");
    }
    if(argc>7){
        printf("too many arguments");
    }
    else if(argc != 1 && argc<=7){
        for(cmdCounter = 2; cmdCounter<argc;cmdCounter++){
            if(strcmp(argv[cmdCounter],"-t") == 0)
            {
                optionsArray[0] = 1;
            }
            else if(strcmp(argv[cmdCounter],"-s") == 0)
            {
                optionsArray[1] = 1;
            }
            else if(strcmp(argv[cmdCounter],"-m") == 0)
            {
                optionsArray[2] = 1;
            }
            else if(strcmp(argv[cmdCounter],"-a") == 0)
            {
                optionsArray[3] = 1;
            }
            else if(strcmp(argv[cmdCounter],"-v") == 0)
            {
                optionsArray[4] = 1;
            }
        }
    }
    
    createCleanInput(inputTXT);
    if (createLexemetable()) {
        return 0;
    }
    createTokenList();
    input = fopen("tokenlist.txt", "r");
    if (input == NULL) {
        printf("**ERROR READING INPUT FILE**");
        return 0;
    }
    
    getToken();
    block();
    
    if (currToken != periodsym){
        if (currToken == endsym) {
            error(9);
        }
        else if (currToken != nulsym){
            error(8);
        }
    }
    
    printSymbolTable();
    emit(11, 0, 3);

    printCode();
    
    FILE *file = fopen("mcode.txt", "r");
    if(file == NULL) {
        printf("\n***Error Reading File***\n");
        return 0;
    }
    FILE *aCodeOutput = fopen("acode.txt", "w");
    FILE *output = fopen("stacktrace.txt", "w");
    
    initialize (file, aCodeOutput);
    execute (output);
    
    fclose(output);
    
    return 0;
}

//Beginning of Assignment 2

void createCleanInput(FILE *input) {
    
    
    //Create output file
    FILE *clean = fopen("cleaninput.txt", "w");
    char c1, c2;
    
    //Skips over comments for ouput
    while ((c1 = fgetc(input)) != EOF) {
        if (c1 == '/') {
            c2 = fgetc(input);
            if (c2 == '*') {
                while (c2 != '/') {
                    c2 = fgetc(input);
                }
            } else {
                fputc(c1, clean);
                fputc(c2, clean);
            }
        } else {
            fputc(c1, clean);
        }
    }
    
    //Close files
    fclose(clean);
}

int createLexemetable() {
    FILE *input = fopen("cleaninput.txt", "r");
    FILE *output = fopen("lexemetable.txt", "w");
    char c1;
    int line = 1;
    fprintf(output, "lexeme\ttoken type\n");
    
    while ((c1 = fgetc(input)) != EOF) {
        // Checks to see if input is a number
        
        if (isdigit(c1)) {
            int counter = 1;
            
            do {
                fputc(c1, output);
                c1 = fgetc(input);
                counter++;
                if (counter > 6) {
                    printf("ERROR Line %d: Digit too long\n", line);
                    return 1;
                }
            } while (isdigit(c1));
            
            if (isalpha(c1)) {
                printf("ERROR Line %d: Wrong Variable Name\n", line);
            }
            
            ungetc(c1, input);
            fprintf(output, "\t%d\n", numbersym);
        }
        
        // Checks to see if input is in the alphabet
        else if (isalpha(c1)) {
            int i = 0;
            char temp[14];
            int alphaCounter = 1;
            
            int j;
            for (j = 0; j < 14; j++) {
                temp[j] = '\0';
            }
            
            do {
                temp[i++] = c1;
                c1 = fgetc(input);
                alphaCounter++;
                if (alphaCounter > 12) {
                    printf("ERROR Line %d: Identifier too long\n", line);
                    return 1;
                }
            } while (isalpha(c1) || isdigit(c1));
            
            ungetc(c1, input);
            
            if (!strcmp(temp, "odd")) {
                fprintf(output, "odd\t%d\n", oddsym);
            }
            else if (!strcmp(temp, "begin")) {
                fprintf(output, "begin\t%d\n", beginsym);
            }
            else if (!strcmp(temp, "end")) {
                fprintf(output, "end\t%d\n", endsym);
            }
            else if (!strcmp(temp, "if")) {
                fprintf(output, "if\t%d\n", ifsym);
            }
            else if (!strcmp(temp, "then")) {
                fprintf(output, "then\t%d\n", thensym);
            }
            else if (!strcmp(temp, "while")) {
                fprintf(output, "while\t%d\n", whilesym);
            }
            else if (!strcmp(temp, "do")) {
                fprintf(output, "do\t%d\n", dosym);
            }
            else if (!strcmp(temp, "call")) {
                fprintf(output, "call\t%d\n", callsym);
            }
            else if (!strcmp(temp, "const")) {
                fprintf(output, "const\t%d\n", constsym);
            }
            else if (!strcmp(temp, "var")) {
                fprintf(output, "var\t%d\n", varsym);
            }
            else if (!strcmp(temp, "procedure")) {
                fprintf(output, "procedure\t%d\n", procsym);
            }
            else if (!strcmp(temp, "write")) {
                fprintf(output, "write\t%d\n", writesym);
            }
            else if (!strcmp(temp, "read")) {
                fprintf(output, "read\t%d\n", readsym);
            }
            else if (!strcmp(temp, "else")) {
                fprintf(output, "else\t%d\n", elsesym);
            }
            else {
                fprintf(output, "%s\t%d\n", temp, identsym);
            }
        }
        
        // Checks to see if input is a symbol
        else if (c1 == '+') {
            fprintf(output, "+\t%d\n", plussym);
        }
        else if (c1 == '-') {
            fprintf(output, "-\t%d\n", minussym);
        }
        else if (c1 == '*') {
            fprintf(output, "*\t%d\n", multsym);
        }
        else if (c1 == '/') {
            fprintf(output, "/\t%d\n", slashsym);
        }
        else if (c1 == '=') {
            fprintf(output, "=\t%d\n", eqsym);
        }
        else if (c1 == '<') {
            c1 = fgetc(input);
            if (c1 == '>') {
                fprintf(output, "<>\t%d\n", neqsym);
            }
            else if (c1 == '=') {
                fprintf(output, "<=\t%d\n", leqsym);
            }
            else {
                ungetc(c1, input);
                fprintf(output, "<\t%d\n", lessym);
            }
        }
        else if (c1 == '>') {
            c1 = fgetc(input);
            if (c1 == '=') {
                fprintf(output, ">=\t%d\n", geqsym);
            }
            else {
                ungetc(c1, input);
                fprintf(output, ">\t%d\n", gtrsym);
            }
        }
        else if (c1 == '(') {
            fprintf(output, "(\t%d\n", lparentsym);
        }
        else if (c1 == ')') {
            fprintf(output, ")\t%d\n", rparentsym);
        }
        else if (c1 == ',') {
            fprintf(output, ",\t%d\n", commasym);
        }
        else if (c1 == ';') {
            line++;
            fprintf(output, ";\t%d\n", semicolonsym);
        }
        else if (c1 == '.') {
            fprintf(output, ".\t%d\n", periodsym);
        }
        else if (c1 == ':') {
            c1 = fgetc(input);
            fprintf(output, ":=\t%d\n", becomessym);
        }
        else if (c1 != ' ' && c1 != '\n' && c1 != '\t' && c1 != '\r'){
            printf("ERROR Line %d: Invalid Symbol\n", line);
            return 1;
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}

void createTokenList() {
    //Open input file
    FILE *input = fopen("lexemetable.txt", "r");
    
    //Open output file
    FILE *output = fopen("tokenlist.txt", "w");
    char temp[14], temp2[14];
    
    //Skips over titles on page
    fscanf(input, "%s", temp);
    fscanf(input, "%s", temp);
    fscanf(input, "%s", temp);
    //Scans through entire file
    if(optionsArray[0] == 1){
        printf("\n\n ********** TOKEN LIST  ********** \n\n");
        while (strcmp(temp2, "19")) {
            fscanf(input, "%s", temp);
            fscanf(input, "%s", temp2);
            if (!strcmp(temp2, "2")) {
                fprintf(output, "%s ", temp2);
                printf( "%s ", temp2);
                fprintf(output, "%s ", temp);
                printf( "%s ", temp);
            }
            else if (!strcmp(temp2, "3")) {
                fprintf(output, "%s ", temp2);
                printf("%s ", temp2);
                fprintf(output, "%s ", temp);
                printf("%s ", temp);
            }
            else {
                fprintf(output, "%s ", temp2);
                printf("%s ", temp2);
            }
        }
    }
    else{
        while (strcmp(temp2, "19")) {
            fscanf(input, "%s", temp);
            fscanf(input, "%s", temp2);
            if (!strcmp(temp2, "2")) {
                fprintf(output, "%s ", temp2);
                fprintf(output, "%s ", temp);
            }
            else if (!strcmp(temp2, "3")) {
                fprintf(output, "%s ", temp2);
                fprintf(output, "%s ", temp);
            }
            else {
                fprintf(output, "%s ", temp2);
            }
        }
    }
    
    //Close files
    fclose(input);
    fclose(output);
}


void getToken() {
    
    fscanf(input, "%d ", &currToken);
    
    if (currToken == semicolonsym)
        curLine++;
    
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
            error(6);
        
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
        emit(STO, curLexLevel - symbolTable[result].level, tempM);
    }
    
    //Callsym
    else if (currToken == callsym) {
        getToken();
        if (currToken != identsym)
            error(14);
        
        fscanf(input, "%s", temp);
        result = isInSymTable(temp);
        emit(CAL, curLexLevel - symbolTable[result].level, symbolTable[result].addr);
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
        else if(currToken == beginsym || currToken == identsym){
            error(10);
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
            emit(JMP, 0, cx + 1);
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
    
    //Writesym
    else if (currToken == writesym) {
        getToken();
        if (currToken != identsym)
            error(4);
        
        fscanf(input, "%s", temp);
        result = isInSymTable(temp);
        
        if (symbolTable[result].kind == 1)
            emit(LIT, 0, symbolTable[result].value);
        
        else if (symbolTable[result].kind == 2)
            emit(LOD, curLexLevel - symbolTable[result].level, symbolTable[result].addr);
        
        else
            error(11);
        
        emit(SIO, 0, 0);
        getToken();
    }
    
    //Readsym
    else if (currToken == readsym) {
        getToken();
        if (currToken != identsym)
            error(4);
        
        fscanf(input, "%s", temp);
        result = isInSymTable(temp);
        
        if (symbolTable[result].kind != 2)
            error(11);
        
        emit(SIO, 0, 1);
        emit(STO, curLexLevel - symbolTable[result].level, symbolTable[result].addr);
        
        getToken();
    }
    
    else{
        if(currToken == periodsym){
            error(17);
        }
        else if (currToken != endsym) {
            error(7);
        }
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
    
    if(currToken != numbersym && currToken != identsym && currToken != lparentsym
       && currToken != minussym && currToken != plussym){
        error(24);
    }
    
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
            emit(LOD, curLexLevel - symbolTable[result].level, symbolTable[result].addr);
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
    
    printf("Error Code %d on line %d: ", errNumber, curLine);
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
            printf("Assignment to constant or procedure is not allowed.\n");
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
    exit(errNumber);
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


void printCode() {
    FILE *mcode = fopen("mcode.txt", "w");
    int i;
    if(optionsArray[2] == 1){
        printf("\n\n ********** MCODE TEXT **********\n\n");
        for (i = 0; i < cx; i++) {
            fprintf(mcode, "%d %d %d\n", codeList[i].op, codeList[i].l, codeList[i].m);
            printf("%d %d %d\n", codeList[i].op, codeList[i].l, codeList[i].m);
            
        }
    }
    else{
        for (i = 0; i < cx; i++) {
            fprintf(mcode, "%d %d %d\n", codeList[i].op, codeList[i].l, codeList[i].m);
        }
    }
    fclose(mcode);
}

void printSymbolTable(){
    FILE *symTable = fopen("symboltable.txt", "w");
    int i;
    if(optionsArray[1] == 1){
        printf("\n\n\n ********** SYMBOL TABLE **********\n\n");
        fprintf(symTable,"Name \t Type \t Level \t Value\n");
        printf("Name \t Type \t Level \t Value\n");
    for(i = 0; i < sTableCount; i++){
        char type[6] = "";
        if(symbolTable[i].kind == 1){
            strcpy(type, "const");
            fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].value);
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].value);
        }
        else if(symbolTable[i].kind == 2){
            strcpy(type, "var");
            fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
        }
        else if(symbolTable[i].kind == 3){
            strcpy(type, "proc");
            fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
            printf("%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
        }
        
    }
    }
    else{
    fprintf(symTable,"Name \t Type \t Level \t Value\n");
            for(i = 0; i < sTableCount; i++){
                char type[6] = "";
                if(symbolTable[i].kind == 1){
                    strcpy(type, "const");
                    fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].value);
                }
                else if(symbolTable[i].kind == 2){
                    strcpy(type, "var");
                    fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
                }
                else if(symbolTable[i].kind == 3){
                    strcpy(type, "proc");
                    fprintf(symTable,"%s \t %s \t %d \t %d\n", symbolTable[i].name, type, symbolTable[i].level, symbolTable[i].addr);
                }
                
               }
        }
}



// End of Assignment 4

// Beginning of Assignment 1

//Scans the input and creates an array of structs for the
//  input of instructions
void initialize (FILE *file, FILE *output) {
    int i = 0;
    int tempOP = 0;
    if(optionsArray[3] == 1){
        printf("\n\n********** DISASSEMBLED CODE **********\n\n");
        fprintf(output, "Line\tOP\tL\tM\n");
        printf("Line\tOP\tL\tM\n");
        while (fscanf(file, "%d", &tempOP) != EOF) {
            Node *newNode = (Node*) malloc(sizeof(Node));
            
            newNode->nOP = tempOP;
            
            switch (newNode->nOP) {
                case 1:
                    strcpy(newNode->cOP, "lit");
                    break;
                case 2:
                    strcpy(newNode->cOP, "opr");
                    break;
                case 3:
                    strcpy(newNode->cOP, "lod");
                    break;
                case 4:
                    strcpy(newNode->cOP, "sto");
                    break;
                case 5:
                    strcpy(newNode->cOP, "cal");
                    break;
                case 6:
                    strcpy(newNode->cOP, "inc");
                    break;
                case 7:
                    strcpy(newNode->cOP, "jmp");
                    break;
                case 8:
                    strcpy(newNode->cOP, "jpc");
                    break;
                case 9:
                    strcpy(newNode->cOP, "sio");
                    break;
                case 10:
                    strcpy(newNode->cOP, "sio");
                    break;
                case 11:
                    strcpy(newNode->cOP, "sio");
                    break;
                default:
                    strcpy(newNode->cOP, "ERR");
                    break;
            }
            
            fscanf(file, "%d %d", &newNode->nL, &newNode->nM);
            newNode->nLine = i;
            structArray[i] = newNode;
            
            fprintf(output, "%d\t%s\t%d\t%d\n", newNode->nLine, newNode->cOP, newNode->nL, newNode->nM);
            printf("%d\t%s\t%d\t%d\n", newNode->nLine, newNode->cOP, newNode->nL, newNode->nM);
            
            i++;
        }
    }
    else{
        fprintf(output, "Line\tOP\tL\tM\n");
        
        while (fscanf(file, "%d", &tempOP) != EOF) {
            Node *newNode = (Node*) malloc(sizeof(Node));
            
            newNode->nOP = tempOP;
            
            switch (newNode->nOP) {
                case 1:
                    strcpy(newNode->cOP, "lit");
                    break;
                case 2:
                    strcpy(newNode->cOP, "opr");
                    break;
                case 3:
                    strcpy(newNode->cOP, "lod");
                    break;
                case 4:
                    strcpy(newNode->cOP, "sto");
                    break;
                case 5:
                    strcpy(newNode->cOP, "cal");
                    break;
                case 6:
                    strcpy(newNode->cOP, "inc");
                    break;
                case 7:
                    strcpy(newNode->cOP, "jmp");
                    break;
                case 8:
                    strcpy(newNode->cOP, "jpc");
                    break;
                case 9:
                    strcpy(newNode->cOP, "sio");
                    break;
                case 10:
                    strcpy(newNode->cOP, "sio");
                    break;
                case 11:
                    strcpy(newNode->cOP, "sio");
                    break;
                default:
                    strcpy(newNode->cOP, "ERR");
                    break;
            }
            
            fscanf(file, "%d %d", &newNode->nL, &newNode->nM);
            newNode->nLine = i;
            structArray[i] = newNode;
            
            fprintf(output, "%d\t%s\t%d\t%d\n", newNode->nLine, newNode->cOP, newNode->nL, newNode->nM);
            
            i++;
        }
    }
}

//Executes the instructions.
void execute (FILE *output) {
    int flag = 0;
    if(optionsArray[4] == 1){
        printf("\n\n ********** STACK TRACE ********** \n\n");
        fprintf(output, "\n\t\t\t\tpc\tbp\tsp\tstack\nInitial values\t\t\t0\t1\t0\n");
        printf("\n\t\t\t\tpc\tbp\tsp\tstack\nInitial values\t\t\t0\t1\t0\n");
        while (flag != 1) {
            switch (structArray[pc]->nOP) {
                case 1:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    stack[sp] = structArray[pc]->nM;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 2:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    ALU();
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 3:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    stack[sp] = stack[base(structArray[pc]->nL, bp) + structArray[pc]->nM];
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 4:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    stack[base(structArray[pc]->nL, bp) + structArray[pc]->nM] = stack[sp];
                    sp--;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 5:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    stack[sp + 1] = 0;
                    stack[sp + 2] = base(structArray[pc]->nL, bp);
                    stack[sp + 3] = bp;
                    stack[sp + 4] = pc + 1;
                    bp = sp + 1;
                    pc = structArray[pc]->nM;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 6:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp = sp + structArray[pc]->nM;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 7:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    pc = structArray[pc]->nM;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 8:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    if (stack[sp] == 0) {
                        pc = structArray[pc]->nM;
                    } else {
                        pc++;
                    }
                    sp--;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 9:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("\nstack[%d] = %d\n", sp, stack[sp]);
                    sp--;
                    break;
                case 10:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    printf("Enter in value: ");
                    scanf("%d", &stack[sp]);
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 11:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    flag = 1;
                    pc = 0;
                    bp = 0;
                    sp = 0;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    printf("\t%d\t%d\t%d", pc, bp, sp);
                    break;
                default:
                    flag = 1;
                    break;
            }
            
            fprintf(output, "\t");
            printf("\t");
            
            if (sp != 0) {
                int i;
                for (i = 1; i <= sp; i++) {
                    if (i != 1) {
                        if (i == bp) {
                            fprintf(output, "| ");
                            printf("| ");
                        }
                    }
                    fprintf(output, "%d ", stack[i]);
                    printf("%d ", stack[i]);
                }
            }
            
            fprintf(output, "\n");
            printf("\n");
            
        }
    }
    else{
        fprintf(output, "\n\t\t\t\tpc\tbp\tsp\tstack\nInitial values\t\t\t0\t1\t0\n");
        
        while (flag != 1) {
            switch (structArray[pc]->nOP) {
                case 1:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    stack[sp] = structArray[pc]->nM;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 2:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    ALU();
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 3:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    stack[sp] = stack[base(structArray[pc]->nL, bp) + structArray[pc]->nM];
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 4:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    stack[base(structArray[pc]->nL, bp) + structArray[pc]->nM] = stack[sp];
                    sp--;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 5:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    stack[sp + 1] = 0;
                    stack[sp + 2] = base(structArray[pc]->nL, bp);
                    stack[sp + 3] = bp;
                    stack[sp + 4] = pc + 1;
                    bp = sp + 1;
                    pc = structArray[pc]->nM;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 6:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp = sp + structArray[pc]->nM;
                    pc++;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 7:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    pc = structArray[pc]->nM;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 8:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    if (stack[sp] == 0) {
                        pc = structArray[pc]->nM;
                    } else {
                        pc++;
                    }
                    sp--;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 9:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    printf("\nstack[%d] = %d\n", sp, stack[sp]);
                    sp--;
                    break;
                case 10:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    sp++;
                    printf("Enter in value: ");
                    scanf("%d", &stack[sp]);
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                case 11:
                    fprintf(output, "%d\t%s\t%d\t%d", structArray[pc]->nLine, structArray[pc]->cOP, structArray[pc]->nL, structArray[pc]->nM);
                    flag = 1;
                    pc = 0;
                    bp = 0;
                    sp = 0;
                    fprintf(output, "\t%d\t%d\t%d", pc, bp, sp);
                    break;
                default:
                    flag = 1;
                    break;
            }
            
            fprintf(output, "\t");
            
            if (sp != 0) {
                int i;
                for (i = 1; i <= sp; i++) {
                    if (i != 1) {
                        if (i == bp) {
                            fprintf(output, "| ");
                        }
                    }
                    fprintf(output, "%d ", stack[i]);
                }
            }
            
            fprintf(output, "\n");
            
        }
    }
    
}

//Computes the ALU
void ALU () {
    switch (structArray[pc]->nM) {
        case 0:
            sp = bp - 1;
            pc = stack[sp + 4];
            bp = stack[sp + 3];
            break;
        case 1:
            stack[sp] = 0 - stack[sp];
            pc++;
            break;
        case 2:
            stack[sp - 1] = stack[sp] + stack[sp - 1];
            sp--;
            pc++;
            break;
        case 3:
            stack[sp - 1] = stack[sp - 1] - stack[sp];
            sp--;
            pc++;
            break;
        case 4:
            stack[sp - 1] = stack[sp] * stack[sp - 1];
            sp--;
            pc++;
            break;
        case 5:
            stack[sp - 1] = stack[sp - 1] / stack[sp];
            sp--;
            pc++;
            break;
        case 6:
            if (stack[sp] % 2 == 0)
                stack[sp] = 0;
            else
                stack[sp] = 1;
            pc++;
            break;
        case 7:
            stack[sp - 1] = stack[sp - 1] % stack[sp];
            sp--;
            pc++;
            break;
        case 8:
            if (stack[sp - 1] == stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp - 1] = 0;
            sp--;
            pc++;
            break;
        case 9:
            if (stack[sp - 1] != stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp - 1] = 0;
            sp--;
            pc++;
            break;
        case 10:
            if (stack[sp - 1] < stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp - 1] = 0;
            sp--;
            pc++;
            break;
        case 11:
            if (stack[sp - 1] <= stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp - 1] = 0;
            sp--;
            pc++;
            break;
        case 12:
            if (stack[sp - 1] > stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp - 1] = 0;
            sp--;
            pc++;
            break;
        case 13:
            if (stack[sp - 1] >= stack[sp])
                stack[sp - 1] = 1;
            else
                stack[sp -1] = 0;
            sp--;
            pc++;
            break;
        default:
            printf("ERROR with ALU");
            break;
    }
    return;
}

//Finds the base "l" levels down.
int base (l, base) {
    int b1 = base; // find base L levels down
    while (l > 0) {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}

// End of Assignment 1
