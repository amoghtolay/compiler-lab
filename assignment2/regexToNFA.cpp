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

/*
 * Defines the regular expression operators
 * It also shows priority, STAR has max priority, followed by the rest
 */
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

/*
 * Reads the first line of file which is space separated and contains
 * all the alphabets of the language
 */
vector < vector<int> > allTermStates ( string fileName )
{
	ifstream fpCal;
	fpCal.open( strdup(fileName.c_str()) );
	if (!fpCal.is_open()){
		perror("Regular expression value doesn't exist or could not be opened\n");
		exit (1);
	}
	
	int uniqueStateID = 0;
	string terminalString;
	vector < vector <int> > singletonStates;
	
	getline (fpCal,terminalString );	
    string buf; // Have a buffer string
    stringstream ss(terminalString); // Insert the string into a stream
    while (ss){
		string sub;
		ss >> sub;
		vector <int> inputState;
		inputState.push_back((int)(sub.c_str())[0]);
		inputState.push_back(uniqueStateID);
		uniqueStateID++;
		inputState.push_back(uniqueStateID);
		uniqueStateID++;
        singletonStates.push_back(inputState);
    }
	singletonStates.pop_back();
	fpCal.close();
	return singletonStates;
}
int isOperator (char c)
{
	int t;                                                     
	for (t = STAR; t <= RB; t++)
		if ( c == operators[t] )
			return t;     
	return -1;
}
string infixToPostfix( string expression )
{
	/*
	 * Converts given infix expression to postfix expression keeping
	 * in mind the appropriate precedence rules
	 */
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

int main(int argc, char *argv[])
{	
	if (argc < 2) {
		cout<<"ERROR: No regexFile provided";
		exit(1);
	}
	stringstream convert;
	convert << argv[1];
	string fileName = convert.str();
	/*
	 * This vector inputState contains all states of input in the
	 * format as shown:
	 * ascii_val(input), currentStateNum, transitionToStateNum
	 */
	vector < vector<int> > inputState = allTermStates( fileName );
	for ( unsigned int i=0; i<inputState.size(); i++)
		cout<<inputState[i][0]<<"\t"<<inputState[i][1]<<"\t"<<inputState[i][2]<<"\n";
	
	return 0;
}
