/*********************************************************************
* Author: Amogh Tolay
* Date: Saturday, 30th March, 2013
* Compiler lab assignment3, generating a parsing table ( LL(1) ) given
* the first and follow sets to me in the form of a file
***********************************************************************/

#include <iostream>
#include <cstdio>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <set>
#include <list>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;
#define EPSILON "epsilon"
#define END_OF_STRING "$"
/*
 * Prototype declarations
 */

bool isInSet ( string tokenName, string setToken, map < string, list <string> > setName );
list < string > getTermList ( char* prodRuleFileName );
list < string > getNonTermList ( char* prodRuleFileName );
// Read production rules from file, and then factor it and output the final factored grammar
map < string, vector < vector < string > > > factor ( char* prodRuleFileName );
void printToFile ( char* outputFileName, map < string, vector < string > > factoredGrammar );
/*
 * Function to read first set from the file and store in RAM
 */

bool isInSet ( string tokenName, string setToken, map < string, list <string> > setName ){
	bool found = false;
	for ( list < string >::iterator itList = setName[setToken].begin(); itList != setName[setToken].end(); ++itList ){
		if ( *itList == tokenName ){
			found = true;
			break;
		}
	}
	return found;
}
list < string > getNonTermList ( char* prodRuleFileName ){
	list < string > nonTermList;
	ifstream fpProdRule;
	fpProdRule.open( prodRuleFileName );
	if ( !fpProdRule.is_open() ){
		perror("Production rule file could not be opened\n");
		exit (1);
        }
	string line;
	getline ( fpProdRule, line );
	getline ( fpProdRule, line );
	istringstream iss( line );
	while (iss){
		string nonTerm;
		iss >> nonTerm;
		nonTermList.push_back ( nonTerm );
	}
	nonTermList.pop_back();
	return nonTermList;
}
list < string > getTermList ( char* prodRuleFileName ){
	list < string > termList;
	ifstream fpProdRule;
	fpProdRule.open( prodRuleFileName );
	if ( !fpProdRule.is_open() ){
		perror("Production rule file could not be opened\n");
		exit (1);
        }
	string line;
	getline ( fpProdRule, line );
	istringstream iss( line );
	while (iss){
		string term;
		iss >> term;
		termList.push_back ( term );
	}
	termList.pop_back();
	return termList;
}
map < string, vector < vector < string > > > factor ( char* prodRuleFileName ){
	map < string, vector < vector < string > > > originalGrammar;
	ifstream fpProdRule;
	fpProdRule.open( prodRuleFileName );
	if ( !fpProdRule.is_open() ){
		perror("Production rule file could not be opened\n");
		exit (1);
        }
	string prodRule;
	// Ignoring the first two lines which are list of terms and nonterms
	getline ( fpProdRule, prodRule );
	getline ( fpProdRule, prodRule );
	// Now populating a list of LHS and RHS components for each rule
	while( getline ( fpProdRule, prodRule ) ){
		vector < string > rhsTerms;
		unsigned pos = prodRule.find( "-->" );
		string rhsString;
		if( pos != string::npos ){
			string lhs = prodRule.substr( 0,pos );
			rhsString = prodRule.substr( pos+3 );
			istringstream iss( rhsString );
			while( iss ){
				string rhsTerm;
				iss >> rhsTerm;
				rhsTerms.push_back( rhsTerm );
			}
			rhsTerms.pop_back();
			originalGrammar[ lhs ].push_back( rhsTerms );
		}
		else{
			perror("The production rule is without an arrow sign\n");
			exit(1);
		}
	}
	// The complete original grammar is in the memory now, called originalGrammar
	return originalGrammar;
}
void printToFile ( char* outputFileName, map < string, vector < vector < string > > > grammar ){
	ofstream fpOut;
	fpOut.open( outputFileName );
	if ( !fpOut.is_open() ){
		perror("Production rule file could not be opened\n");
		exit (1);
        }
	/*
	 * IMPORTANT: ADD TERMINALS AND NON TERMINALS LINE
	
	for ( map < string, vector < vector < string > > >::iterator itMap = grammar.begin(); itMap != grammar.end(); ++itMap ){
		for ( vector < vector < string > >::iterator itVec1 = itMap->second.begin(); itVec1 != itMap->second.end(); ++itVec1 ){
			fpOut << itMap->first << "--> ";
			for ( vector < string >::iterator itVec2 = (*itVec1).begin(); itVec2 != (*itVec1).end(); ++itVec2 ){
				fpOut << *itVec2 << " ";
			}
			fpOut << "\n";
		}
	}
}

int main( int argc, char *argv[] ){
	if ( argc < 3 ) {
		cout<<"ERROR: No production rule file provided or no output file provided.\n";
		cout<<"Usage: ./leftFactor prodRule outProdRule\n";
		exit ( EXIT_FAILURE );
	}
	/*
	 * Just testing code below, printing and stuff. Should be commented later on.
	 */

	list < string > termList = getTermList ( argv[1] );
	list < string > nonTermList = getNonTermList ( argv[1] );
	map < string, vector < vector < string > > > factoredGrammar = factor ( argv[1] );
	printToFile ( argv[2], factoredGrammar );
	return ( EXIT_SUCCESS );
}
