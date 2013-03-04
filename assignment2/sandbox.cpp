/***********************************************************************
 * Author: Amogh Tolay. Date 1st March, 2013 Friday
 * This is the code to generate an NFA from a given regular expression
 * This is then fed into a code that generates a DFA from this NFA and
 * then this is used for the lexer
 **********************************************************************/

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <stack>
using namespace std;
enum operation          
{
	STAR,
	CONCAT,
	OR,
	EPSILON,
	LB,
	RB
};
char operators[] = { '*', '@', '|', '$', '(', ')' };
int isOperator (char s)
{
	int t;                                                     
	for (t = STAR; t <= RB; t++)
		if ( s == operators[t] )
			return t;     
	return -1;
}
string implicitConcat ( string token )
{
	bool flag1 = false;
	bool flag2 = false;
	string finalString = "";
	
	for ( unsigned int i=0; i<token.length()-1; i++ ){
		if ( token[i] == operators[OR] || token[i] == operators[CONCAT] || token[i] == operators[LB] )
			flag1 = true;
		if ( token[i+1] == operators[OR] || token[i+1] == operators[CONCAT] || token[i+1] == operators[RB] || token[i+1] == operators[STAR])
			flag2 = true;
		
		if ( !flag1 && !flag2 )
			finalString = finalString + token[i] + operators[CONCAT];
		else
			finalString = finalString + token[i];
		flag1 = false;
		flag2 = false;
	}
	finalString = finalString + token[token.length()-1];
	return finalString;
}
int main()
{
	string a;
	cout<<"Enter nonConcat exp:\n";
	cin>>a;	
	string b = implicitConcat ( a );
	cout<<"\n"<<b<<"\n";
	return 0;
}

