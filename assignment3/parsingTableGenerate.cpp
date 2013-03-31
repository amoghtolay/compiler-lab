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
#define EPSILON epsilon;
#define END_OF_STRING $;
/*
 * Prototype declarations
 */

map < string,list < string > > getFirstSet ( char* firstFileName );
map < string, list < string > > getFollowSet ( char* followFileName );
void printTableToFile ( vector< vector<string> > table, char* outputFileName );
bool isInSet ( string tokenName, string setToken, map < string, list <string> > setName );
map < string, map <string, string> > constructTable ( char* prodRuleFileName );
list < string > getTermList ( char* prodRuleFileName );
list < string > getNonTermList ( char* prodRuleFileName );
vector < string > rhsTerms ( string productionRule );
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
	// Skipping two lines for terms and nonterms
	getline ( fpFirst, line );
	getline ( fpFirst, line );
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
        // Skipping two lines for terms and nonterms
        getline ( fpFollow, line );
        getline ( fpFollow, line );
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
	return termList;
}
map < string, map < string,string > > constructTable ( char* prodRuleFileName ){
	map < string, map < string,string > > table;
	list < string > termList = getTermList ( prodRuleFileName );
	list < string > nonTermList = getNonTermList( prodRuleFileName );
	// nonTerminals are in list, terminals are in list
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
		list < string > oneRuleTerms;
		unsigned pos = prodRule.find( "-->" );
		if( pos != string::npos ){
			string lhs = prodRule.substr( 0,pos );
			oneRuleTerms.push_back( lhs );
			prodRule = prodRule.substr( pos+3 );
			istringstream iss( prodRule );
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

	}
	return table;
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
	/*
	map < string, list < string > > firstSet = getFirstSet ( argv[2] );
	map < string, list < string > > followSet = getFollowSet ( argv[3] );
	map < string, map < string, string > > table = constructTable ( argv[1] );	
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
