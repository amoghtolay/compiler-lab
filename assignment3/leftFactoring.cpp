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
int uniqueID = 0;
bool isInSet ( string tokenName, string setToken, map < string, list <string> > setName );
list < string > getTermList ( char* prodRuleFileName );
list < string > getNonTermList ( char* prodRuleFileName );
// Read production rules from file, and then factor it and output the final factored grammar
map < string, vector < vector < string > > > factor ( char* prodRuleFileName );
void printToFile ( char* outputFileName, map < string, vector < string > > factoredGrammar );
string getNewNT ();
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
string getNewNT (){
	string newNT;
	// write code to generate new nonTerminal
	newNT = "T" + static_cast<ostringstream*>( &(ostringstream() << uniqueID) )->str();
	uniqueID++;
	return newNT;
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
	/*
	 * Now start left factoring it
	 */
	/* THINGS TO DO:
	 * 1. Repeat procedure multiple times for A->BD
	 					A->CD, A->BCD, A->BCE
	   2. Eliminate duplicate entries from grammar
	 */
	map < string, vector < vector < string > > > newGrammar;
	for ( map < string, vector < vector < string > > >::iterator itMap = originalGrammar.begin(); itMap != originalGrammar.end(); ++itMap )
		for ( vector < vector < string > >::iterator itVec1 = (itMap->second).begin(); itVec1 != (itMap->second).end(); ++itVec1 )
			for ( vector < vector < string > >::iterator itVec2 = itVec1+1; itVec2 != (itMap->second).end(); ++itVec2 ){
				unsigned int i;
				vector < string > tempVector;
				vector < string > tempRemaining1;
				vector < string > tempRemaining2;
				for ( i = 0; i < min ( (*itVec1).size(), (*itVec2).size() ); ++i ){
					if ( (*itVec1)[i] == (*itVec2)[i] ){
						tempVector.push_back ( (*itVec1)[i] );
					}
					else{
						break;
						//tempRemaining1.push_back ( (*itVec1)[i] );
						//tempRemaining2.push_back ( (*itVec2)[i] );
					}
				}
				// if till end common, then add epsilon rule to it now
				if ( i == (*itVec1).size() )
					tempRemaining1.push_back ( EPSILON );
				if ( i == (*itVec2).size() )
					tempRemaining2.push_back ( EPSILON );
				for ( unsigned i1 = i; i1 < (*itVec1).size(); i1++ )
					tempRemaining1.push_back ( (*itVec1)[i1] );
				for ( unsigned i2 = i; i2 < (*itVec2).size (); i2++ )
					tempRemaining2.push_back ( (*itVec2)[i2] );
				if ( tempVector.size() > 0 ){
					vector < vector < string > > constructNewVector1;
					vector < vector < string > > constructNewVector2;
					string newNT = getNewNT ();
					tempVector.push_back ( newNT );
					constructNewVector1.push_back ( tempVector );
					newGrammar[itMap->first] = ( constructNewVector1 );
					constructNewVector2.push_back ( tempRemaining1 );
					constructNewVector2.push_back ( tempRemaining2 );
					newGrammar[newNT] = ( constructNewVector2 );
				}
				else{
				// write code for this
					vector < vector < string > > constructNewVector1;
					vector < vector < string > > constructNewVector2;
					constructNewVector1.push_back ( tempRemaining1 );
					constructNewVector2.push_back ( tempRemaining2 );
					newGrammar[itMap->first].push_back( tempRemaining1 );
					newGrammar[itMap->first].push_back( tempRemaining2 );
				}

			}
	return newGrammar;
}
void printToFile ( char* outputFileName, map < string, vector < vector < string > > > grammar, list < string > nonTermList, list < string > termList ){
	ofstream fpOut;
	fpOut.open( outputFileName );
	if ( !fpOut.is_open() ){
		perror("Production rule file could not be opened\n");
		exit (1);
        }
	/*
	 * IMPORTANT: ADD TERMINALS AND NON TERMINALS LINE
	 */
	for ( list < string >::iterator it = termList.begin(); it != termList.end(); ++it )
		fpOut << *it << " ";
	fpOut << "\n";
	for ( list < string >::iterator it = nonTermList.begin(); it != nonTermList.end(); ++it )
		fpOut << *it << " ";
	fpOut << "\n";
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
	printToFile ( argv[2], factoredGrammar, nonTermList, termList );
	return ( EXIT_SUCCESS );
}
