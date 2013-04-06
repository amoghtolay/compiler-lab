/**************************************************
 * Name: Eliminating Left Recursion
 * By: Saurav Kumar Patel
 *************************************************/
/**************************************
* Input file format:
* first line contains list of terminals
* second line contains list of all non-Terminals ( first symbol is START symbol )
* Rest file contains production rules
* For example (A-->bB | d), we write
* A-->bB
*     |d
**************************************/


#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>

#define LINESIZE 100000
#define DOLLAR "$"

using namespace std;

void printProdRules( map< string, vector< vector< string > > > prodRules )
{
    for( map< string, vector< vector< string > > >::iterator itNonTerm = prodRules.begin(); itNonTerm != prodRules.end(); itNonTerm++ ){
      cout << itNonTerm->first << " \t: ";
      for( vector< vector< string > >::iterator itoneRule = itNonTerm->second.begin(); itoneRule != itNonTerm->second.end(); itoneRule++ ){
        for( vector< string >::iterator it=(*itoneRule).begin(); it != (*itoneRule).end(); ++it)
          cout << ' ' << *it;
        cout << " \n\t| ";
      }
      cout << endl;
    }
}


map< string, vector< vector< string > > > immediateLeft( string LHS, vector< vector< string > > oneRule )
{
  map< string, vector< vector< string > > > twoRules;
  vector< vector< string > > recursive, nonRecursive, newRule;
  for( vector< vector< string > >::iterator itRule = oneRule.begin(); itRule != oneRule.end(); itRule++ ){
    //cout << " Comparator :" << (*itRule)[0] << endl;
    if( (*itRule)[0] == LHS ){
      recursive.push_back( *itRule );
      //cout << (*itRule)[1] << "EE1    ";
    }
    else{
      nonRecursive.push_back( *itRule );
      //cout << (*itRule)[0] << " :EE2    ";
    }
  }
  if( recursive.size() == 0 ){
    twoRules[ LHS ] = oneRule;
  }
  else{
    string newLHS = LHS + '1';
    cout << "New LHS : " << newLHS <<endl;
    for( vector< vector< string > >::iterator itnonRecur = nonRecursive.begin(); itnonRecur != nonRecursive.end(); itnonRecur++ ){
      //cout << "non recurrent " << (*itnonRecur)[0] << endl;
      // A-->A t | b | epsilon, then instead of A-->epsilon A1 | b A1 and A1-->epsilon | t A1, we need A-->A1 | b A1 and A1-->epsilon | t A1
      // basically, preventing epsilon to occur as first terminal of any RHS
      if( (*itnonRecur)[0] == "epsilon" )
          (*itnonRecur).clear();
      (*itnonRecur).push_back( newLHS );
      newRule.push_back( *itnonRecur );
    }
    if( newRule.size() != 0 )
      twoRules[ LHS ] = newRule;
    newRule.clear();
    for( vector< vector< string > >::iterator itRecur = recursive.begin(); itRecur != recursive.end(); itRecur++ ){
      (*itRecur).erase( (*itRecur).begin() );
      //cout << "recurrent " << (*itRecur)[0] << endl;
      (*itRecur).push_back( newLHS );
      newRule.push_back( *itRecur );
    }
    vector< string > eps;
    eps.push_back( "epsilon" );
    newRule.push_back( eps );
    twoRules[ newLHS ] = newRule;
  }
  return twoRules;
}

int main( int argc, char **argv )
{
  if( argc < 2  ){
    cout << "Invalid Usage:\n";
    cout << "./program inputGrammariFileame outputFilename( optional )\n";
    exit(0);
  }
  set <string> terms, nonTerms;
  int lineNumber=0;  // For error reporting during reading input grammar file
  fstream readGrammar, outputfile;
  string line="";
  string start="";  // For start symbol
  map< string, vector< vector< string > > > prodRules;  // First entry is LHS and rest is part of multiple RHS in a vector of list

  readGrammar.open( argv[1] );
  if( readGrammar.is_open() ){
    /* Get Terminals and non-Terminals list from first two lines */
    if( getline( readGrammar, line ) ){
      lineNumber++;
      string terminal;
      istringstream iss( line );
      //cout << "List of all Terminals are: ";
      while( iss ){
	iss >> terminal;
	//cout << terminal << ", ";
	terms.insert( terminal );
      }
      //cout<<endl;
      terms.insert( "epsilon" );  // epsilon is a terminal
	//terms.insert( "$" );	// $ is a special terminal( EOF )
    }
    else{
      cout << "Missing list of terminals from file" << endl;
      exit(0);
    }
    if( getline( readGrammar, line ) ){
      lineNumber++;
      string nonTerminal;
      //cout << "List of all Non-terminals are: ";
      istringstream iss( line );
      iss >> start;  // first non terminal should be start symbol
      nonTerms.insert( start );
      //cout << start << ", ";
      while( iss ){
	iss >> nonTerminal;
	//cout << nonTerminal << ", ";
	nonTerms.insert( nonTerminal );
      }
      //cout<<endl;
    }
    else{
      cout << "Missing list of non terminals from file. Please give input file in corect format." << endl;
      exit(0);
    }
    /* Now comes the time for reading production rules */
    //cout << "Production rules are:" << endl;
    string LHS = "";
    while( getline( readGrammar, line ) ){
      lineNumber++;
      vector< string > oneRule;
      unsigned pos = line.find( "-->" );
      if( pos+1 != 0 ){
        LHS = line.substr( 0,pos );
        // remoove all trailing whitespaces from LHS
        LHS.erase( LHS.find_last_not_of( " \n\r\t" ) + 1 );
        line = line.substr( pos+3 );
      }
      else{
        pos = line.find( "|" );
	    if( pos+1 == 0 ){
          cout << "Invalid format for production rule in line number " << lineNumber << endl;
	      exit(0);
	    }
        line = line.substr( pos + 1 );
      }
      //cout << "LHS is : " << LHS << "\t";
      //oneRule.push_back( prodRules[ LHS ] );
      istringstream iss( line );
      string RHScomponent="";
      while( iss ){
        iss >> RHScomponent;
        //cout << RHScomponent << "\t";
        oneRule.push_back( RHScomponent );
      }
      //cout << endl;
      oneRule.pop_back();
      if( oneRule.size() > 0 ){
        prodRules[ LHS ].push_back( oneRule );
      }
      else{
        cout << "RHS missing for production rule in line number " << lineNumber << endl;
        exit(0);
      }
    }
    // Print rules as in vector prodRules
    /*for( map< string, vector< vector< string > > >::iterator itNonTerm = prodRules.begin(); itNonTerm != prodRules.end(); itNonTerm++ ){
      cout << itNonTerm->first << " \t: ";
      for( vector< vector< string > >::iterator itoneRule = itNonTerm->second.begin(); itoneRule != itNonTerm->second.end(); itoneRule++ ){
        for( vector< string >::iterator it=(*itoneRule).begin(); it != (*itoneRule).end(); ++it)
          cout << ' ' << *it;
        cout << " \n\t| ";
      }
      cout << endl;
    }*/
  }
  else{
    cout << "Error in opening file: " << argv[1] << endl;
    exit(0);
  }
  // Now the production rules are in memory


  // grammar must be without epsilon-production and without cycles
  //int loop=0;
  //unsigned numNonTerms = prodRules.size();
  //for i := 1 to n do
  //printProdRules( prodRules );
  for( map< string, vector< vector< string > > >::iterator iti= prodRules.begin(); iti!= prodRules.end(); iti++ ){
      //for j := 1 to i − 1 do
    for( map< string, vector< vector< string > > >::iterator itj = prodRules.begin(); itj != iti; itj++ ){
        //Let Aj → δ1 | · · · | δm be all the current Aj -productions.
      for( vector< vector< string > >::iterator itOneRule = itj->second.begin(); itOneRule != itj->second.end(); itOneRule++ ){
          //for each production Ai → Aj γ
          if( (*itOneRule)[0] == itj->first ){
              // pop vector containing itj->first
              vector< string > AY = *itOneRule;
              //cout << AY[0] << " : " << AY [1]<<endl;
              prodRules[ iti->first ].erase( itOneRule );
              // extract Y and store
              AY.erase( AY.begin() );
              vector< string > Y = AY;
              // replace Ai --> Aj Y by Ai → δ1 γ | · · · | δm γ.
              for( vector< vector< string > >::iterator it = prodRules[ itj->first ].begin(); it != prodRules[ itj->first ].end(); it++ ){
                  vector< string > newRule;
                  newRule = *it;
                  if( newRule[0] == "epsilon" ){
                      newRule.clear();
                  }
                  for( vector< string >::iterator itY = Y.begin(); itY != Y.end(); itY++ ){
                      newRule.push_back( *itY );
                  }
                  prodRules[ iti->first ].push_back( newRule );
              }
          }
      }
    }
    // Eliminate the immediate left recursion from Ai -productions, use A−i .
    // eliminate immediate left recursion
    map< string, vector< vector< string > > > newProdRules;
    newProdRules = immediateLeft( iti->first, iti->second );
    prodRules.erase( iti->first );
    for( map< string, vector< vector< string > > >::iterator itnewProdRules = newProdRules.begin(); itnewProdRules != newProdRules.end(); itnewProdRules++ ){
      prodRules[ itnewProdRules->first ] = itnewProdRules->second;
      // adding NEW non Terminals to list of non Terminals 
      nonTerms.insert( itnewProdRules->first );
    }
  }
  //printProdRules( prodRules );
  //checking if output file name is provided
  string leftFreeFile;
  if( argc == 3 )
      leftFreeFile = string( argv[2] );
  else
      leftFreeFile = string( argv[1] ) + ".leftFree";
  fstream ofptr;
  ofptr.open( leftFreeFile.c_str(), fstream::out );
  if( ofptr.is_open() ){
      //write list of terminals
      for( set< string >::iterator itTerms = terms.begin(); itTerms != terms.end(); itTerms++ ){
          // not printing epsilon in terminals list only for the shake of amogh's implementation of parseing table generation
          if( *itTerms != "epsilon" )
            ofptr << *itTerms << " ";
      }
	ofptr << DOLLAR;	// appending dollar to terminals for use in next iterations
      ofptr << endl;
      //write list of non terminals
	ofptr << start << " ";
    // removing start from list of nonterminals
    nonTerms.erase( nonTerms.find( start ) );
      for( set< string >::iterator itNonTerms = nonTerms.begin(); itNonTerms != nonTerms.end(); itNonTerms++ )
          ofptr << *itNonTerms << " ";
      ofptr << endl;
      //write left recursion free grammar to file
    for( map< string, vector< vector< string > > >::iterator itNonTerm = prodRules.begin(); itNonTerm != prodRules.end(); itNonTerm++ ){
      for( vector< vector< string > >::iterator itOneRule = itNonTerm->second.begin(); itOneRule != itNonTerm->second.end(); itOneRule++ ){
        ofptr << itNonTerm->first << "-->";
        for( vector< string >::iterator it=(*itOneRule).begin(); it != (*itOneRule).end(); ++it)
          ofptr << *it << " ";
        if( itOneRule+1 != itNonTerm->second.end() )
            ofptr << " \n";
      }
      ofptr << endl;
    }
  }
  else{
      cout << "Unable to open output file.\n";
      exit(0);
  }
  cout << "Left Recursion Removed Successfully.\n";
}
