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

string infixToPostfix( string expression )
{
	expression = expression + ")";
	stack<char> stringStack;
	string postfix = "";
	char dummy = '(';
    stringStack.push(dummy);
    
    for ( unsigned int i=0; i < expression.length(); i++ ){
		
		char sub = expression[i];
		if ( isOperator(sub) == -1 || isOperator(sub) == EPSILON )
			postfix = postfix + sub;
		if ( isOperator(sub) == LB )
			stringStack.push(sub);
		if ( isOperator(sub) != LB && isOperator(sub) != RB && isOperator(sub) != -1){
			while ( ( isOperator((stringStack.top())) <= isOperator(sub) ) && isOperator((stringStack.top())) != -1 ){
				postfix = postfix + stringStack.top();
				stringStack.pop();
			}
			stringStack.push(sub);
		}
		if ( isOperator(sub) == RB ){
			while ( (isOperator( (stringStack.top()) ) != LB) && isOperator((stringStack.top())) != -1){
				postfix = postfix + stringStack.top();
				stringStack.pop();
			}
			stringStack.pop();
		}
	}
	return postfix;
}
/*
 * Reads the first line of file which is space separated and contains
 * all the alphabets of the language
 */
int main()
{
	string a;
	getline ( std::cin, a );	
	/*
	 * Check infix to postfix conversion
	 */
	string post = infixToPostfix(a);
	cout<<post;
	return 0;
}

