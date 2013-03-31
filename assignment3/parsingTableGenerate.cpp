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
/*
 * For first and follow sets, use map of lists
 * This allows easier naming and finding
 */

map < string,list < string > > getFirstSet ( char* firstFileName );
map < string, list < string > > getFollowSet ( char* followFileName );
void printTableToFile ( vector< vector<string> > table, char* outputFileName );
list<int> getTerminals ( char*  prodRuleFileName );
list<int> getNonTerminals ( char* prodRuleFileName );
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


int main( int argc, char *argv[] ){
	if ( argc < 5 ) {
		cout<<"ERROR: No production rule file provided or no first set file or follow set file or output file provided.\n";
		cout<<"Usage: ./tableGen prodRule firstFile followFile outputTableFile\n";
		exit ( EXIT_FAILURE );
	}
	map < string, list < string > > firstSet = getFirstSet ( argv[2] );
	map < string, list < string > > followSet = getFollowSet ( argv[3] );
	
	for ( map < string, list <string> >::iterator itMap = firstSet.begin(); itMap != firstSet.end(); ++itMap ){
		cout << itMap->first << " => \n";
		for ( list < string >::iterator itList = firstSet[itMap->first].begin(); itList != firstSet[itMap->first].end(); ++itList )
			cout << ' ' << *itList;
		cout<<"\n";
	}
        return ( EXIT_SUCCESS );
}
