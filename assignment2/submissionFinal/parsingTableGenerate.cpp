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

map < string,list < string > > getFirstSet ( char* firstFileName );
map < string, list < string > > getFollowSet ( char* followFileName );
void printTableToFile ( map < string, map < string, string > > table, char* outputFileName, list < string > termList, list < string > nonTermList );
bool isInSet ( string tokenName, string setToken, map < string, list <string> > setName );
map < string, map <string, string> > constructTable ( char* prodRuleFileName, map < string, list < string > > firstSet, map < string, list < string > > followSet );
list < string > getTermList ( char* prodRuleFileName );
list < string > getNonTermList ( char* prodRuleFileName );
/*
 * Function to read first set from the file and store in RAM
 */
map < string,list < string > > getFirstSet ( char* firstFileName ){
	ifstream fpFirst;
	fpFirst.open( firstFileName );
	if ( !fpFirst.is_open() ){
		perror("First set file could not be opened\n");
		exit (1);
	}
		
	map < string, list < string > > firstSet;
	string line;
	while ( getline ( fpFirst, line ) ){
		istringstream iss( line );
		string firstWord;
		iss >> firstWord;

		while ( iss ){
			string word;
			iss >> word;
			firstSet[firstWord].push_back(  word );
		}
	}
	return firstSet;
}
/*
 * Function to read follow set from file and store in RAM
 */
map < string,list < string > > getFollowSet ( char* followFileName ){
        ifstream fpFollow;
        fpFollow.open( followFileName );
        if ( !fpFollow.is_open() ){
                perror("Follow set file could not be opened\n");
                exit (1);
        }

        map < string, list < string > > followSet;
        string line;
        while ( getline ( fpFollow, line ) ){
                istringstream iss( line );
                string firstWord;
                iss >> firstWord;

                while ( iss ){
                        string word;
                        iss >> word;
                        followSet[firstWord].push_back(  word );
                }       
        }       
        return followSet;
}

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
map < string, map < string,string > > constructTable ( char* prodRuleFileName, map < string, list < string > > firstSet, map < string, list < string > > followSet  ){
	map < string, map < string,string > > table;
	// nonTerminals are in list, terminals are in list
	list < string > termList = getTermList ( prodRuleFileName );
	list < string > nonTermList = getNonTermList( prodRuleFileName );
	
	// Construct the table with blank entries
	for ( list < string >::iterator itNonTerms= nonTermList.begin(); itNonTerms!= nonTermList.end(); ++itNonTerms ){
		for ( list < string >::iterator itTerms = termList.begin(); itTerms != termList.end(); ++itTerms ){
			table[*itNonTerms][*itTerms] = "";
		}
		//table[*itNonTerms][ END_OF_STRING ] = "";
	}
	// Now read production rule A->alpha, and then for each alpha, check if in first or follow of RHS
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
		vector < string > oneRuleTerms;
		unsigned pos = prodRule.find( "-->" );
		string rhsString;
		if( pos != string::npos ){
			string lhs = prodRule.substr( 0,pos );
			oneRuleTerms.push_back( lhs );
			rhsString = prodRule.substr( pos+3 );
			istringstream iss( rhsString );
			while( iss ){
				string rhsTerm;
				iss >> rhsTerm;
				oneRuleTerms.push_back( rhsTerm );
			}
			oneRuleTerms.pop_back();
		}
		else{
			perror("The production rule is without an arrow sign\n");
			exit(1);
		}
		// Now for each rule, check the first and follow sets, and then appropriately add to the table
		// Now for terminal in first ( firstRHS ), add entry to table
		string lhsOfProdRule = oneRuleTerms[0];
		bool isEpsilonExists = false;
		int indexOneRuleTerms = 1;
		do {
			string lhs = oneRuleTerms[indexOneRuleTerms];
			for ( list < string >::iterator itTerm = termList.begin(); itTerm != termList.end(); ++itTerm )
				if ( isInSet ( *itTerm, lhs, firstSet ) ){
					if ( table[lhsOfProdRule][*itTerm] != "" ){
						perror("The grammar is NOT LL(1)\n");
						exit(1);
					}
					else
						table[lhsOfProdRule][*itTerm] = rhsString;
				}
			// If epsilon is in firstSet, then not only check the follow, but also check first and follow sets of
			// subsequent non-terminals which might go to epsilon
			if ( isInSet ( EPSILON, lhs, firstSet ) ){
				isEpsilonExists = true;
				indexOneRuleTerms++;
				// checking follow sets here
				if ( indexOneRuleTerms == (int)oneRuleTerms.size() ){
					for ( list < string >::iterator itTerm = termList.begin(); itTerm != termList.end(); ++itTerm )
						if ( isInSet ( *itTerm, lhsOfProdRule, followSet ) ){
							if ( table[lhsOfProdRule][*itTerm] != "" ){
								perror ( "The grammar is NOT LL(1)\n");
								exit(1);
							}
							else
								table[lhsOfProdRule][*itTerm] = rhsString;
						}
					// checking $ for follow set
					/*
					if ( isInSet ( END_OF_STRING, lhsOfProdRule, followSet ) ){
						if ( table[lhsOfProdRule][END_OF_STRING] != "" ){
							perror ( "The grammar is NOT LL(1)\n" );
							exit(1);
						}
						else
							table[lhsOfProdRule][END_OF_STRING] = rhsString;
					}
					*/
				}
			}
			else
				isEpsilonExists = false;
		} while ( isEpsilonExists && indexOneRuleTerms < (int) oneRuleTerms.size() );
		// check to see if whole RHS is nullable or not - NOT REQUIRED
		/*
		if ( indexOneRuleTerms == (int) oneRuleTerms.size() ){
			if ( table[lhsOfProdRule][END_OF_STRING] != "" ){
				perror ( "The grammar is ambigous\n");
				exit(1);
			}
			else
				table[lhsOfProdRule][END_OF_STRING] = rhsString;
		}
		*/
	}
	return table;
}
void printTableToFile ( map < string, map < string, string > > table, char* outputFileName, list < string > termList, list < string > nonTermList ){
	ofstream fpOut;
	fpOut.open( outputFileName );
	if ( !fpOut.is_open() ){
		perror("Output file could not be opened\n");
		exit (1);
        }
	for ( list < string >::iterator itNonTerm = nonTermList.begin(); itNonTerm != nonTermList.end(); ++itNonTerm ){
		for ( list < string >::iterator itTerm = termList.begin(); itTerm != termList.end(); ++itTerm )
			fpOut << table[*itNonTerm][*itTerm]<<",";
		fpOut << "\n";
		}
}
int main( int argc, char *argv[] ){
	if ( argc < 5 ) {
		cout<<"ERROR: No production rule file provided or no first set file or follow set file or output file provided.\n";
		cout<<"Usage: ./tableGen prodRule firstFile followFile outputTableFile\n";
		exit ( EXIT_FAILURE );
	}
	/*
	 * Just testing code below, printing and stuff. Should be commented later on.
	 */

	map < string, list < string > > firstSet = getFirstSet ( argv[2] );
	map < string, list < string > > followSet = getFollowSet ( argv[3] );
	map < string, map < string, string > > table = constructTable ( argv[1], firstSet, followSet );
	cout<<"Table constructed successfully\n";
	list < string > termList = getTermList ( argv[1] );
	list < string > nonTermList = getNonTermList ( argv[1] );
	printTableToFile ( table, argv[4], termList, nonTermList );
	/*
	for ( map < string, list <string> >::iterator itMap = firstSet.begin(); itMap != firstSet.end(); ++itMap ){
		cout << itMap->first << " => \n";
		for ( list < string >::iterator itList = firstSet[itMap->first].begin(); itList != firstSet[itMap->first].end(); ++itList )
			cout << ' ' << *itList;
		cout<<"\n";
	}
	list < string > termList = getTermList ( argv[1] );
	for ( list < string >::iterator it=termList.begin(); it != termList.end(); ++it )
		cout<<*it<<"\t";
	*/
        return ( EXIT_SUCCESS );
}
