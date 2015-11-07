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