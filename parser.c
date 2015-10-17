#include <stdio.h>
#include <stdlib.h>

typedef enum {
     int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
     multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
     gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
     periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
     whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
     readsym = 32, elsesym = 33;
} token_type;

	createCleanInput(file);
	createLexemetable(file);
	createToeknList(file);
	FILE *clean = fopen("cleaninput.txt", "a");
	FILE *lextable = fopen("lexemetable.txt", "a");
	FILE *tokenlist = fopen("tokenlist.txt", "a");

int main (void) {
	//Opens input file
	FILE *input = fopen("input.txt", "r");

	//Create output files
	createCleanInput(input);
	createLexemetable(input);
	createToeknList(input);





	return 0;
}

void createCleanInput(FILE *input) {
	char c1, c2;
	while (c1 = fgetc(input) != EOF) {
		if (c1 == '/') {
			while (c2)
		}
	}
}