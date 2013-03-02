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
int uniqueStateID = 1;
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
	epsilonState.push_back((int)( operators[EPSILON] ) );
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
	
	fpCal.close();
	return singletonStates;
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
vector < vector<int> > operationOR ( vector < vector<int> > a, vector < vector<int> > b )
{
	/*
	 * ORs the states pointed by a and b, and then returns the NFA for
	 * a|b
	 */
	vector < vector<int> > output;
	for ( int i=0; i<a.size() && a[i][0] != -1; i++ )
		output.push_back( a[i] );
	for ( int i=0; i<b.size() && b[i][0] != -1; i++ )
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
	vector<int> startEpsilon1, startEpsilon2;
	startEpsilon1.push_back ( operators[EPSILON] );
	startEpsilon1.push_back ( uniqueStateID );
	startEpsilon1.push_back ( oldStart1 );
	startEpsilon2.push_back ( operators[EPSILON] );
	startEpsilon2.push_back ( uniqueStateID );
	startEpsilon2.push_back ( oldStart2 );
	
	uniqueStateID++;
	
	vector<int> finalEpsilon1, finalEpsilon2;
	finalEpsilon1.push_back ( operators[EPSILON] );
	finalEpsilon1.push_back ( oldFinish1 );
	finalEpsilon1.push_back ( uniqueStateID );
	finalEpsilon2.push_back ( operators[EPSILON] );
	finalEpsilon2.push_back ( oldFinish2 );
	finalEpsilon2.push_back ( uniqueStateID );	 
	
	uniqueStateID++;

	output.push_back ( startEpsilon1 );
	output.push_back ( startEpsilon2 );
	output.push_back ( finalEpsilon1 );
	output.push_back ( finalEpsilon2 );
	
	return output;
}
vector < vector<int> > operationCONCAT ( vector<int> a, vector<int> b )
{
	/*
	 * CONCATs the states pointed by a and b, and then returns the NFA for
	 * a@b
	 */
}
vector < vector<int> > operationSTAR ( vector<int> a )
{
	/*
	 * STARs the states pointed by a and b, and then returns the NFA for
	 * a*
	 */
}
string generateOpStack ( string postfix, vector < vector<int> > inputState )
{
	for ( int i=0; i<postfix.length(); i++ ){
		if ( isOperator( postfix[i] ) != -1 ){
			//use the nfa for this state
		}
		if ( isOperator ( postfix[i] ) == OR ){
			//send the two preceding states to the function to OR
		}
		if ( isOperator ( postfix[i] ) == CONCAT ){
			//send the two preceding states to the function to CONCAT
		}			
		if ( isOperator ( postfix[i] ) == STAR ){
			//send the preceding state to the function to CONCAT
		}
	}
}

/*
 * Reads the first line of file which is space separated and contains
 * all the alphabets of the language
 */
int main()
{
	string a;
	cout<<"Enter infix exp:\n";
	getline ( std::cin, a );	
	/*
	 * Check infix to postfix conversion
	 */
	string post = infixToPostfix(a);
	cout<<post;
	string fileName;
	cout<<"Enter fileName:\n";
	cin>>fileName;
	vector < vector<int> > inputState = allTermStates( fileName );
	vector < vector<int> > tempPush1;
	vector < vector<int> > tempPush2;
	tempPush1.push_back ( inputState[2] );
	tempPush2.push_back ( inputState[3] );
	/*
	 * Outputing the vector inputstate
	 */
	cout<<"Output all singleton\n";
	for ( unsigned int i=0; i<inputState.size(); i++)
		cout<<inputState[i][0]<<"\t"<<inputState[i][1]<<"\t"<<inputState[i][2]<<"\n";
	cout<<"ENDS\n";
	for ( unsigned int i=0; i<tempPush1.size(); i++)
		cout<<tempPush1[i][0]<<"\t"<<tempPush1[i][1]<<"\t"<<tempPush1[i][2]<<"\n";
	for ( unsigned int i=0; i<tempPush2.size(); i++)
		cout<<tempPush2[i][0]<<"\t"<<tempPush2[i][1]<<"\t"<<tempPush2[i][2]<<"\n";

	cout<<"Till here OK\n";
	vector < vector<int> > outputOR = operationOR ( tempPush1, tempPush2 );
	for ( unsigned int i=0; i<outputOR.size(); i++)
		cout<<outputOR[i][0]<<"\t"<<outputOR[i][1]<<"\t"<<outputOR[i][2]<<"\n";
	
	return 0;
}

