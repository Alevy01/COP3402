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