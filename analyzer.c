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

void createCleanInput();
int createLexemetable();
void createTokenList();

int main (void) {
	createCleanInput();
	if (createLexemetable()) {
		return 0;
	}
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
    fclose(input);
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

    //Close files
    fclose(input);
    fclose(output);
}