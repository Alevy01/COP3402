#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
     nulsym = 1, identsym, numbersym, plussym, minussym,
     multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
     gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
     periodsym, becomessym, beginsym, endsym, ifsym, thensym,
     whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
     readsym, elsesym
} token_type;

	void createCleanInput();
	void createLexemetable();
	void createTokenList();
	void getTokenType(char string[], FILE *ofp);

int main (void) {
	//Create output files
	createCleanInput();
	createLexemetable();
	createTokenList();

	return 0;
}


void createCleanInput() {
	//Open input file
	FILE *input = fopen("input.txt", "r");

	//Create output file
    FILE *clean = fopen("cleaninput.txt", "w");
    char c1, c2;
    
    //Skips over comments for ouput
    while ((c1 = fgetc(input)) != EOF) {
        if (c1 == '/') {
            c2 = fgetc(input);
            while (c2 != '/') {
                c2 = fgetc(input);
            }
        } else {
            fputc(c1, clean);
        }
    }
    
    //Close files
    fclose(input);
    fclose(clean);
}

void createLexemetable(){
	FILE * ifp = fopen("cleaninput.txt", "r");
	FILE * ofp = fopen("lexemetable.txt", "w");

	char string[11];
	char c;

	char syntax[6] = {'(', ')', ',', ';', '.', '='};

	fprintf(ofp, "lexeme\t\t token type\n");

	while(c != EOF){
		c = fscanf(ifp, "%s", string);
		if(c == EOF){
			break;
		}
		char overflow[1];
		int flag = 0;
		for(int i = 0; i<sizeof(syntax); i++){
			if(string[strlen(string)-1] == syntax[i]){
				if(syntax[i] != '='){
					overflow[0] = string[strlen(string)-1];
					flag = 1;
					string[strlen(string)-1] = '\0';
				}
			}
		}

		
		if(flag == 1){
			printf("%s\n", string);
			getTokenType(string, ofp);
			getTokenType(overflow, ofp);
		}
		else{
			printf("%s\n", string);
			getTokenType(string, ofp);
		}
	}
	fclose(ifp);
	fclose(ofp);
}


void getTokenType(char string[], FILE *ofp){

	if((int)string[0] >= 48 && (int)string[0] <= 57){
		fprintf(ofp, "%s\t\t %d\n", string, numbersym);
	}
	else if(string[0] == '+'){
		fprintf(ofp, "%c\t\t %d\n", string[0], plussym);
	}
	else if(string[0] == '-'){
		fprintf(ofp, "%c\t\t %d\n", string[0], minussym);	
	}
	else if(string[0] == '*'){
		fprintf(ofp, "%c\t\t %d\n", string[0], multsym);
	}
	else if(string[0] == '/'){
		fprintf(ofp, "%c\t\t %d\n", string[0], slashsym);
	}
	else if(string[0] == '='){
		fprintf(ofp, "%c\t\t %d\n", string[0], eqsym);
	}
	else if(string[0] == '<'){
		fprintf(ofp, "%c\t\t %d\n", string[0], lessym);
	}
	else if(string[0] == '>'){
		fprintf(ofp, "%c\t\t %d\n", string[0], gtrsym);
	}
	else if(string[0] == '('){
		fprintf(ofp, "%c\t\t %d\n", string[0], lparentsym);
	}
	else if(string[0] == ')'){
		fprintf(ofp, "%c\t\t %d\n", string[0], rparentsym);
	}
	else if(string[0] == ','){
		fprintf(ofp, "%c\t\t %d\n", string[0], commasym);
	}
	else if(string[0] == ';'){
		fprintf(ofp, "%c\t\t %d\n", string[0], semicolonsym);
	}
	else if(string[0] == '.'){
		fprintf(ofp, "%c\t\t %d\n", string[0], periodsym);
	}
	else if(!strcmp(string, "odd")){
		fprintf(ofp, "%s\t\t %d\n", string, oddsym);
	}
	else if(!strcmp(string, "<>")){
		fprintf(ofp, "%s\t\t %d\n", string, neqsym);
	}
	else if(!strcmp(string, "<=")){
		fprintf(ofp, "%s\t\t %d\n", string, leqsym);
	}
	else if(!strcmp(string, ">=")){
		fprintf(ofp, "%s\t\t %d\n", string, geqsym);
	}
	else if(!strcmp(string, ":=")){
		fprintf(ofp, "%s\t\t %d\n", string, becomessym);
	}
	else if(!strcmp(string, "begin")){
		fprintf(ofp, "%s\t\t %d\n", string, beginsym);
	}
	else if(!strcmp(string, "end")){
		fprintf(ofp, "%s\t\t %d\n", string, endsym);
	}
	else if(!strcmp(string, "if")){
		fprintf(ofp, "%s\t\t %d\n", string, ifsym);
	}
	else if(!strcmp(string, "then")){
		fprintf(ofp, "%s\t\t %d\n", string, thensym);
	}
	else if(!strcmp(string, "while")){
		fprintf(ofp, "%s\t\t %d\n", string, whilesym);
	}
	else if(!strcmp(string, "do")){
		fprintf(ofp, "%s\t\t %d\n", string, dosym);
	}
	else if(!strcmp(string, "call")){
		fprintf(ofp, "%s\t\t %d\n", string, callsym);
	}
	else if(!strcmp(string, "const")){
		fprintf(ofp, "%s\t\t %d\n", string, constsym);
	}
	else if(!strcmp(string, "var")){
		fprintf(ofp, "%s\t\t %d\n", string, varsym);
	}
	else if(!strcmp(string, "procedure")){
		fprintf(ofp, "%s\t\t %d\n", string, procsym);
	}
	else if(!strcmp(string, "write")){
		fprintf(ofp, "%s\t\t %d\n", string, writesym);
	}
	else if(!strcmp(string, "read")){
		fprintf(ofp, "%s\t\t %d\n", string, readsym);
	}
	else if(!strcmp(string, "else")){
		fprintf(ofp, "%s\t\t %d\n", string, elsesym);
	}
	else{
		fprintf(ofp, "%s\t\t %d\n", string, identsym);
	}

}


void createTokenList() {
	//Open input file
    FILE *input = fopen("lexemetable.txt", "r");

    //Open output file
    FILE *output = fopen("tokenlist.txt", "w");
    char temp[11], temp2[11];
    
    //Skips over titles on page
    fscanf(input, "%s", temp);
    fscanf(input, "%s", temp);
    fscanf(input, "%s", temp);
    
    //Scans through entire file
    while (strcmp(temp2, "19")) {
        fscanf(input, "%s", temp);
        fscanf(input, "%s", temp2);
        if (!strcmp(temp2, "2")) {
            fprintf(output, "%s ", temp2);
            fprintf(output, "%s ", temp);
        }
        else {
            fprintf(output, "%s ", temp2);
        }
    }

    //Close files
    fclose(input);
    fclose(output);
}