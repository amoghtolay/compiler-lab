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
/*
 * Algorithm:
 * Takes the regex from file for each token class. Then generate the
 * postfix notation of this regex, and then generate the individual NFAs
 * Then finally OR these NFA units to make one single unit NFA with
 * one accepting state for each token class.
 */ 
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
 * Remove this global declaration later
 */
int uniqueStateID = 1;

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
	
	string terminalString;
	vector < vector <int> > singletonStates;
	
	getline (fpCal,terminalString );	
    string buf; // Have a buffer string
    stringstream ss(terminalString); // Insert the string into a stream
    /*
     * Push the EPSILON state first as state 1->2 on EPSILON
     */
	vector <int> epsilonState;
	epsilonState.push_back( (int)operators[EPSILON] );
	epsilonState.push_back( uniqueStateID );
	uniqueStateID++;
	epsilonState.push_back(uniqueStateID);
	uniqueStateID++;
	singletonStates.push_back(epsilonState);
	/*
	 * Now do the same for all other terminals
	 */
	int start, end;
    while (ss){
		string sub;
		ss >> sub;
		vector <int> inputState;
		inputState.push_back((int)(sub.c_str())[0]);
		start = uniqueStateID;
		inputState.push_back(start);
		uniqueStateID++;
		end = uniqueStateID;
		inputState.push_back(end);
		uniqueStateID++;
        singletonStates.push_back(inputState);
    }
	singletonStates.pop_back();
	uniqueStateID = uniqueStateID-2;
	
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
vector < vector<int> > operationOR ( vector < vector<int> > a, vector < vector<int> > b )
{
	/*
	 * ORs the states pointed by a and b, and then returns the NFA for
	 * a|b
	 */
	vector < vector<int> > output;
	for ( unsigned int i=0; i<a.size() && a[i][0] != -1; i++ )
		output.push_back( a[i] );
	for ( unsigned int i=0; i<b.size() && b[i][0] != -1; i++ )
		output.push_back( b[i] );
	int oldStart1;
	int oldStart2;
	int oldFinish1;
	int oldFinish2;
	if ( a[a.size()-1][0] == -1 ){
		oldStart1 = a[a.size()-1][1];
		oldFinish1 = a[a.size()-1][2];
	}
	else{
		oldStart1 = a[0][1];
		oldFinish1 = a[0][2];
	}
	if ( b[b.size()-1][0] == -1 ){
		oldStart2 = b[b.size()-1][1];
		oldFinish2 = b[b.size()-1][2];
	}
	else {
		oldStart2 = b[0][1];
		oldFinish2 = b[0][2];
	}
	int newStart = uniqueStateID;
	vector<int> startEpsilon1, startEpsilon2;
	startEpsilon1.push_back ( operators[EPSILON] );
	startEpsilon1.push_back ( newStart );
	startEpsilon1.push_back ( oldStart1 );
	startEpsilon2.push_back ( operators[EPSILON] );
	startEpsilon2.push_back ( newStart );
	startEpsilon2.push_back ( oldStart2 );
	
	uniqueStateID++;
	int newFinal = uniqueStateID;
	vector<int> finalEpsilon1, finalEpsilon2;
	finalEpsilon1.push_back ( operators[EPSILON] );
	finalEpsilon1.push_back ( oldFinish1 );
	finalEpsilon1.push_back ( newFinal );
	finalEpsilon2.push_back ( operators[EPSILON] );
	finalEpsilon2.push_back ( oldFinish2 );
	finalEpsilon2.push_back ( newFinal );	 
	
	uniqueStateID++;
	vector<int> startEndInfo;
	startEndInfo.push_back( -1 );
	startEndInfo.push_back( newStart );
	startEndInfo.push_back( newFinal );
	
	output.push_back ( startEpsilon1 );
	output.push_back ( startEpsilon2 );
	output.push_back ( finalEpsilon1 );
	output.push_back ( finalEpsilon2 );
	output.push_back ( startEndInfo );
	return output;
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

vector < vector < vector<int> > > generateOpStack ( string postfix, string fileName )
{
	cout<<"Here val is"<<uniqueStateID<<"\n";
	vector < vector < vector<int> > > setOfStates;
	vector < vector<int> > inputState = allTermStates( fileName );
	cout<<"Here val is"<<uniqueStateID<<"\n";

	for ( unsigned int i=0; i<postfix.length(); i++ ){
		if ( isOperator( postfix[i] ) == -1 ){
			//use the nfa for this state
			for ( unsigned int j = 0; j<inputState.size(); j++ ){
				if ( inputState[j][0] == (int)postfix[i] ){
					vector < vector<int> > temp;
					temp.push_back(inputState[j]);
					setOfStates.push_back(temp);
					break;
				}
			}
		}
		if ( isOperator ( postfix[i] ) == OR ){
			//send the two preceding states to the function to OR
			vector < vector<int> > output;
			vector < vector<int> > operand1 = setOfStates[setOfStates.size()-1];
			vector < vector<int> > operand2 = setOfStates[setOfStates.size()-2];
			setOfStates.pop_back();
			setOfStates.pop_back();
			output = operationOR ( operand1, operand2 );
			setOfStates.push_back ( output );
		}
		if ( isOperator ( postfix[i] ) == CONCAT ){
			//send the two preceding states to the function to CONCAT
		}
		if ( isOperator ( postfix[i] ) == STAR ){
			//send the preceding state to the function to CONCAT
		}
	}
	return setOfStates;
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
	string infixExp;
	cout<<"Please enter expression:\n";
	cin>>infixExp;
	
	vector < vector < vector<int> > > setOfStates;
	setOfStates = generateOpStack ( infixToPostfix(infixExp), fileName );
	
	for ( unsigned int i=0; i<setOfStates.size(); i++ )
		for ( unsigned int j=0; j<setOfStates[i].size(); j++ ){
			for ( unsigned int k=0; k<setOfStates[i][j].size(); k++)
				cout<<setOfStates[i][j][k]<<"\t";
			cout<<"\n";
		}
	return 0;
}
