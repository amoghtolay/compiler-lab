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
char operators[] = { '+','@' ,'|' ,'$' ,'{' , '}' };
char oldOperators[] = { '*', '@', '|', '$', '(', ')' };

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
string convertStringToEscapedString ( string raw )
{
	string newString = "";

	for ( unsigned int i = 0; i<raw.length(); i++ ){
		bool flag = false;
		for ( int j=STAR; j<=RB; j++ ){
			if ( raw[i] == '\\' && (i+1)<raw.length() ){
				newString = newString + raw[i+1];
				i++;
				flag = true;
				break;
			}
			if ( raw[i] == oldOperators[j] ){
				flag = true;
				newString = newString + operators[j];
			}
		}
		if ( !flag )
			newString = newString + raw[i];
	}
	return newString;
}

int main()
{
	string a;
	cout<<"Give expression:\n";
	cin>>a;
	string b = convertStringToEscapedString ( a );
	cout<<b<<"\n";
	//string b = implicitConcat ( a );
	//cout<<b<<"\n";
	return 0;
}

