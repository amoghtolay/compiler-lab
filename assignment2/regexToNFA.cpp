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
#include <algorithm>

/*
 * Algorithm:
 * Takes the regex from file for each token class. Then generate the
 * postfix notation of this regex, and then generate the individual NFAs
 * Then finally OR these NFA units to make one single unit NFA with
 * one accepting state for each token class.
 */ 
using namespace std;
typedef vector < vector <int> > NFAStateSet;
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
 * Function declarations
 */
NFAStateSet allTermStates ( string fileName );
int isOperator (char c);
NFAStateSet operationOR ( NFAStateSet a, NFAStateSet b );
NFAStateSet operationCONCAT ( NFAStateSet a, NFAStateSet b );
NFAStateSet operationSTAR ( NFAStateSet a );
string infixToPostfix( string expression );
NFAStateSet generateOpStack ( string postfix, string fileName );
NFAStateSet sortStatesForDFA ( NFAStateSet unsortedStates );
int getFinalState ( NFAStateSet states );
NFAStateSet combineAllNFA ( vector < NFAStateSet> allRegexNFA );

/*
 * Reads the first line of file which is space separated and contains
 * all the alphabets of the language
 */
int isOperator (char c)
{
	int t;                                                     
	for (t = STAR; t <= RB; t++)
		if ( c == operators[t] )
			return t;     
	return -1;
}
NFAStateSet allTermStates ( string fileName )
{
	ifstream fpCal;
	fpCal.open( strdup(fileName.c_str()) );
	if (!fpCal.is_open()){
		perror("Regular expression file doesn't exist or could not be opened\n");
		exit (1);
	}
	
	string terminalString;
	NFAStateSet singletonStates;
	
	getline (fpCal,terminalString );	
    stringstream ss(terminalString); // Insert the string into a stream
    /*
     * Push the EPSILON state first as state 1->2 on EPSILON
     */
	vector <int> epsilonState;
	epsilonState.push_back( (int)operators[EPSILON] );
	epsilonState.push_back( -1 );
	epsilonState.push_back( -1 );
	singletonStates.push_back(epsilonState);
	/*
	 * Now do the same for all other terminals
	 */
    while (ss){
		string sub;
		ss >> sub;
		vector <int> inputState;
		inputState.push_back((int)(sub.c_str())[0]);
		inputState.push_back(-1);
		inputState.push_back(-1);
        singletonStates.push_back(inputState);
    }
	singletonStates.pop_back();
	
	fpCal.close();
	return singletonStates;
}
bool compare (vector <int> i,vector <int> j){ return (i[1]<j[1]); }
NFAStateSet sortStatesForDFA ( NFAStateSet unsortedStates )
{
	sort (unsortedStates.begin(), unsortedStates.end(), compare);
	int startState = unsortedStates[0][1];
	vector < int > temp;
	temp.push_back(operators[EPSILON]);
	temp.push_back(0);
	temp.push_back(startState);
	unsortedStates.insert( unsortedStates.begin(), temp );
	
	return unsortedStates;
}
int getFinalState ( NFAStateSet states )
{
	int finalState;
	for ( unsigned int i=0; i<states.size(); i++)
		if ( states[i][0] == -1 )
			finalState = states[i][2];
	return finalState;
}
NFAStateSet combineAllNFA ( vector < NFAStateSet> allRegexNFA )
{
	NFAStateSet combined;
	for ( unsigned int i=0; i<allRegexNFA.size(); i++){
		for ( unsigned int j=0; j<allRegexNFA[i].size(); j++){
			if ( allRegexNFA[i][j][0] != -1 )
				combined.push_back(allRegexNFA[i][j]);
			if ( allRegexNFA[i][j][0] == -1 ){
				vector <int> temp;
				temp.push_back( (int)operators[EPSILON] );
				temp.push_back( 0 );
				temp.push_back( allRegexNFA[i][j][1] );
				combined.push_back ( temp );

				vector <int> startEndInfo;
				startEndInfo.push_back( -1 );
				startEndInfo.push_back( 0 );
				startEndInfo.push_back( allRegexNFA[i][j][2] );
				combined.push_back ( startEndInfo );
			}
		}
	}
	return combined;
}
bool fileReadWrite ( string inFile, string outFile )
{
	ifstream fpCal;
	ofstream fpOut;
	fpCal.open( strdup(inFile.c_str()) );
	fpOut.open( strdup(outFile.c_str()) );
	if (!fpCal.is_open() || !fpOut.is_open()){
		perror("Regular expression or output file doesn't exist or could not be opened\n");
		exit (1);
	}
	string discardAlphabetString;	
	getline (fpCal,discardAlphabetString );	
	vector < NFAStateSet > allRegexNFA;
	vector <string> tokenClassName;
	
	string infixExp;
	while (	getline ( fpCal,infixExp ) ){
		stringstream ss(infixExp);
		vector <string> tokenRegEx;
		while (ss){
			string sub;
			ss >> sub;
			tokenRegEx.push_back( sub );
		}
		tokenClassName.push_back( tokenRegEx[0] );
		allRegexNFA.push_back( generateOpStack ( infixToPostfix(tokenRegEx[1]), inFile ) );
	}
	/*
	 * Now allRegexNFA contains all NFAs for each expression as a vector
	 * of vectors. Now call function to join these together, single start
	 * state goes to each start state of these NFAs, and then each end
	 * is stored separately with TOKEN_IDs (contained in tokenClassName)
	 */
	NFAStateSet resultCombined = combineAllNFA ( allRegexNFA );
	
	int tokenPtr = 0;
	for ( unsigned int i=0; i < resultCombined.size(); i++ )
		if ( resultCombined[i][0] == -1 )
			fpOut<<tokenClassName[tokenPtr++]<<" "<<resultCombined[i][2]<<",";
	fpOut<<"\n";
	fpOut<<uniqueStateID<<"\n";
	for ( unsigned int i=0; i < resultCombined.size(); i++ ){
		bool endReached = false;
		for ( unsigned int j=0; j<resultCombined[i].size(); j++ )
			if ( resultCombined[i][0] != -1 ){
				fpOut<<resultCombined[i][j]<<"\t";
				endReached = true;
			}
		if ( endReached )
			fpOut<<"\n";
	}

	fpCal.close();
	fpOut.close();
	return (0);
}
NFAStateSet operationOR ( NFAStateSet a, NFAStateSet b )
{
	/*
	 * ORs the states pointed by a and b, and then returns the NFA for
	 * a|b
	 */
	NFAStateSet output;
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
NFAStateSet operationCONCAT ( NFAStateSet a, NFAStateSet b )
{
	/*
	 * CONCATs the states pointed by a and b, and then returns the NFA for
	 * a@b
	 */
	NFAStateSet output;
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
	vector<int> startEpsilon, middleEpsilon;
	startEpsilon.push_back ( operators[EPSILON] );
	startEpsilon.push_back ( newStart );
	startEpsilon.push_back ( oldStart1 );
	
	middleEpsilon.push_back ( operators[EPSILON] );
	middleEpsilon.push_back ( oldFinish1 );
	middleEpsilon.push_back ( oldStart2 );
	
	uniqueStateID++;
	int newFinal = uniqueStateID;
	vector<int> finalEpsilon;
	finalEpsilon.push_back ( operators[EPSILON] );
	finalEpsilon.push_back ( oldFinish2 );
	finalEpsilon.push_back ( newFinal );
	uniqueStateID++;
	vector<int> startEndInfo;
	startEndInfo.push_back( -1 );
	startEndInfo.push_back( newStart );
	startEndInfo.push_back( newFinal );
	
	output.push_back ( startEpsilon );
	output.push_back ( middleEpsilon );
	output.push_back ( finalEpsilon );
	output.push_back ( startEndInfo );
	return output;
}
NFAStateSet operationSTAR ( NFAStateSet a )
{
	/*
	 * CONCATs the states pointed by a and b, and then returns the NFA for
	 * a*
	 */
	NFAStateSet output;
	for ( unsigned int i=0; i<a.size() && a[i][0] != -1; i++ )
		output.push_back( a[i] );
	int oldStart;
	int oldFinish;
	if ( a[a.size()-1][0] == -1 ){
		oldStart = a[a.size()-1][1];
		oldFinish = a[a.size()-1][2];
	}
	else{
		oldStart = a[0][1];
		oldFinish = a[0][2];
	}
	int newStart = uniqueStateID;
	uniqueStateID++;
	int newFinal = uniqueStateID;
	uniqueStateID++;

	vector<int> startEpsilon, startFinishEpsilon;
	startEpsilon.push_back ( operators[EPSILON] );
	startEpsilon.push_back ( newStart );
	startEpsilon.push_back ( oldStart );
	
	startFinishEpsilon.push_back ( operators[EPSILON] );
	startFinishEpsilon.push_back ( newStart );
	startFinishEpsilon.push_back ( newFinal );
	
	vector<int> loopBack, finishEpsilon;
	loopBack.push_back ( operators[EPSILON] );
	loopBack.push_back ( oldFinish );
	loopBack.push_back ( oldStart );
	
	finishEpsilon.push_back ( operators[EPSILON] );
	finishEpsilon.push_back ( oldFinish );
	finishEpsilon.push_back ( newFinal );
	
	vector<int> startEndInfo;
	startEndInfo.push_back( -1 );
	startEndInfo.push_back( newStart );
	startEndInfo.push_back( newFinal );
	
	output.push_back ( startEpsilon );
	output.push_back ( startFinishEpsilon );
	output.push_back ( loopBack );
	output.push_back ( finishEpsilon );
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
NFAStateSet generateOpStack ( string postfix, string fileName )
{
	stack < NFAStateSet > stateStack;
	NFAStateSet inputState = allTermStates( fileName );
	bool isSingleton = true;
	for ( unsigned int i=0; i<postfix.length(); i++ ){
		if ( isOperator( postfix[i] ) == -1 ){
			//use the nfa for this state
			for ( unsigned int j = 0; j<inputState.size(); j++ ){
				if ( inputState[j][0] == (int)postfix[i] ){
					NFAStateSet tempToBePushed;
					/*
					 * Copy this to uidGiven to assign unique IDs
					 */
					vector <int> uidGiven = inputState[j];
					uidGiven[1]=uniqueStateID;
					uniqueStateID++;
					uidGiven[2]=uniqueStateID;
					uniqueStateID++;
					
					tempToBePushed.push_back(uidGiven);
					stateStack.push ( tempToBePushed );
					break;
				}
			}
		}
		if ( isOperator ( postfix[i] ) == OR ){
			//send the two preceding states to the function to OR
			NFAStateSet output;
			NFAStateSet operand2 = stateStack.top();
			stateStack.pop();
			NFAStateSet operand1 = stateStack.top();
			stateStack.pop();
			output = operationOR ( operand1, operand2 );
			stateStack.push(output);
			isSingleton = false;
		}
		if ( isOperator ( postfix[i] ) == CONCAT ){
			//send the two preceding states to the function to CONCAT
			NFAStateSet output;
			NFAStateSet operand2 = stateStack.top();
			stateStack.pop();
			NFAStateSet operand1 = stateStack.top();
			stateStack.pop();
			output = operationCONCAT ( operand1, operand2 );
			stateStack.push(output);
			isSingleton = false;
		}
		if ( isOperator ( postfix[i] ) == STAR ){
			//send the preceding state to the function to STAR
			NFAStateSet output;
			NFAStateSet operand = stateStack.top();
			stateStack.pop();
			output = operationSTAR ( operand );
			stateStack.push(output);
			isSingleton = false;
		}
	}
	if (!isSingleton){
		NFAStateSet finalStateSet = stateStack.top();
		if (!stateStack.empty())
			stateStack.pop();
		if (stateStack.empty())
			return finalStateSet;
		else{
			cout<<"ERROR: Stack didn't empty itself\n";
			exit(1);
		}
	}
	
	else if ( isSingleton ){
		NFAStateSet finalStateSet = stateStack.top();
		vector <int> startEndInfo;
		startEndInfo.push_back(-1);
		startEndInfo.push_back(finalStateSet[0][1]);
		startEndInfo.push_back(finalStateSet[0][2]);
		finalStateSet.push_back( startEndInfo );
	
		if (!stateStack.empty())
			stateStack.pop();
		if ( stateStack.empty() )
			return finalStateSet;
		else{
			cout<<"ERROR: Stack didn't empty itself\n";
			exit(1);
		}
	}
	NFAStateSet dummyReturnVal;
	return (dummyReturnVal);

}
int main(int argc, char *argv[])
{	
	if (argc < 3) {
		cout<<"ERROR: No regexFile provided";
		exit(1);
	}
	string inFile, outFile;

	stringstream convert1;
	convert1 << argv[1];
	inFile = convert1.str();
	
	stringstream convert2;
	convert2 << argv[2];
	outFile = convert2.str();
	/*
	 * This vector inputState contains all states of input in the
	 * format as shown:
	 * ascii_val(input), currentStateNum, transitionToStateNum
	 */
	
	if ( fileReadWrite ( inFile, outFile ) ){
		cout<<"ERROR: Could not write the output NFA file from the Regex";
		exit(1);
	}
	return 0;
}
