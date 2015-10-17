#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
     int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
     multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
     gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
     periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
     whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
     readsym = 32, elsesym = 33;
} token_type;

	void createCleanInput(FILE *input);
	void createLexemetable();
	void createTokenList();

	FILE *lextable = fopen("lexemetable.txt", "w");
	FILE *tokenlist = fopen("tokenlist.txt", "w");



int main (void) {
	//Opens input file
	FILE *input = fopen("input.txt", "r");

	//Create output files
	createCleanInput(input);
	createLexemetable();
	createTokenList();


	return 0;
}



void createCleanInput(FILE *input) {
    FILE *clean = fopen("cleaninput.txt", "w");
    char c1, c2;
    
    while ((c1 = fgetc(input)) != EOF) {
        printf("%c", c1);
        if (c1 == '/') {
            c2 = fgetc(input);
            while (c2 != '/') {
                c2 = fgetc(input);
            }
        } else {
            fputc(c1, clean);
        }
    }
    
    fclose(clean);
}

void createLexemetable(){
	FILE * ifp = fopen("cleaninput.txt", "r");
	
	char string[11];
	char c;

	char syntax[6] = {'(', ')', ',', ';', '.', '='};

	while(c != EOF){
		c = fscanf(ifp, "%s", string);
		if(c == EOF){
			break;
		}
		char overflow;
		int flag = 0;
		for(int i = 0; i<sizeof(syntax); i++){
			if(string[strlen(string)-1] == syntax[i]){
				if(syntax[i] != '='){
					overflow = string[strlen(string)-1];
					flag = 1;
					string[strlen(string)-1] = '\0';
				}
			}
			
		}

		

	}

}


void createTokenList(FILE *input){
  FILE *output = fopen("tokenlist.txt", "w");


}

