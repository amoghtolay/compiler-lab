#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

#define LINESIZE 10000
#define TOKENSIZE 10000
using namespace std;

void constructDfa ( char DFAinput[1024], vector < vector < map < char, int  > > > *dfa , vector < map < int, string > > *acceptClassType);
pair < int, int > tokenize ( char *token, vector < vector < map < char,int > > > dfa );
int addToSymTab ( char *token, vector < string > *symbolTable);
void constructLexeme ( char *classType, char *lexeme, vector < string > *symbolTable, char *token );
void createSymTabFile (  char * filename, vector < string > *symbolTable );

/* convert integer to string */
/* create full dfa (done)
 * tokenize until accepted (now doing)
 */
string intToStr(int num)
{
	stringstream ss;
	ss<<num;
	return ss.str();
}

int main( int argc, char **argv)
{
	vector < map < int, string > > validTokenType;
	vector < vector < map < char, int  > > > dfa;
	vector < string > symbolTable;
	char DFAinput[1024];
	if (argc < 4 )
	{
		cout<<"provide input and output filenames\n";
		exit(1);
	}
	else
	{
		strcpy (DFAinput, argv[1] );
	}
	
	// Reconstruct DFA
	constructDfa ( DFAinput, &dfa , &validTokenType);
	/* Now Do Lexical Analysis */
	//constructin input and output filenames
	char inputSample[1024];
	char outputSample[1024];
	strcpy ( inputSample, argv[2] );
	strcpy ( outputSample, argv[3] );
	fstream codeInput, codeOutput;
	codeInput.open(inputSample);
	codeOutput.open(outputSample, fstream::out | fstream::trunc );
	
	if ( codeInput.is_open () && codeOutput.is_open() )
	{
		char line[LINESIZE];
		int linenumber = 0;
		while ( codeInput.getline(line, LINESIZE) )
		{
			linenumber++;
			char token[TOKENSIZE];
			char *pch = strtok( line, " \t\n");
			while ( pch != NULL )
			{
				//cout<<pch<<"\n";
				strcpy(token,pch);
				pair < int, int > acceptState = tokenize( token , dfa);	// return type < acceptstate, accepting DFA number >
				//cout<<"TokenId:"<<tokenId<<" ";
				if ( acceptState.first != -1 )
				{
					// add to symbol table
					string tokenType;
					map < int, string >::iterator it = validTokenType[acceptState.second].find(acceptState.first);
					if ( it != validTokenType[acceptState.second].end() )	// if final state is valid
					{
						char lexeme[TOKENSIZE];
						tokenType = it->second;
						constructLexeme( (char*)tokenType.c_str() , lexeme , &symbolTable, token);
						codeOutput<<lexeme;
					}
					else
					{
						//final state is not accepting state
						cout<<"some character missing in token \'"<< pch << "\' found at line "<< linenumber <<"\n";	
					}
				}
				else
				{
					cout<<"Invalid symbol in token \'"<< pch << "\' found at line "<< linenumber <<"\n";
				}
				pch = strtok ( NULL , " \t\n" );
			}
			codeOutput<<endl;
		}
		codeInput.close();
		codeOutput.close();
	}
	// store symbol table in a file
	createSymTabFile( inputSample, &symbolTable );
	return 0;
}

/*------------End of Main----------------------*/

/* Finds matching token ID ( i.e. accepting state of token ) corresponding to token
 * returns pair of  < accepting state, accepting DFAno > on success
 * and < -1, numDFA > on invalid token
 */
pair < int, int > tokenize ( char *token , vector < vector< map < char,int > > > dfa)
{
	pair < int, int > acceptState;
	int numDFA = dfa.size();
	int accept = 0;
	int DFAno = 0;
	int nextState;
	while ( !accept && DFAno < numDFA )
	{
		char *current = token;
		char curinput = current[0];
		int curState = 0;
		while( *current++ )
		{
			//cout<<curState<<"\t";
			//cout<<curinput<<"\t";
			map<char,int>::iterator it = dfa[DFAno].at(curState).find(curinput);
			if ( it == dfa[DFAno][curState].end() )
			{
				//cout<<"No transitions available in DFA "<<DFAno<<" Checking Next DFA.\n";
				nextState = -1;
				break;
			}
			nextState = it->second;
			curState = nextState;
			//cout << nextState << "\n";
			curinput = current[0];
		}
		if ( nextState != -1 )
		{
			accept = 1;
			break;
		}
		DFAno++;
	}
	acceptState = make_pair ( nextState, DFAno );
	return acceptState;
}

/* Add token to symbol Table */
int addToSymTab ( char *token,  vector < string > *symbolTable )
{
	int index = 0;
	int size = (*symbolTable).size();
	//cout<<"Size :"<<size;
	for (int i = 0; i < size; i++ )
	{
		if ( (*symbolTable)[i] == string( token ) ) 
		{
			index = i;
			break;
		}
	}
	if ( index == 0 )
	{
		index = size;
		(*symbolTable).push_back( token );
	}
	return index+1;
}

/* Bring DFA to memory from given DFAfile
 * current delimiter for different DFA is : '!'
 */
void constructDfa ( char DFAinput[1024], vector < vector < map < char, int  > > > *dfa , vector < map < int, string > > *validTokenType)
{
	fstream dfainput;
	dfainput.open(DFAinput);
	int DFAno = -1;	

	if ( dfainput.is_open() )
	{
		char curState[1024];
		char line[LINESIZE];
		int MaxState;
		while ( dfainput.getline ( line, LINESIZE ) )
		{
			//if ( !line )
			//	continue;
			//cout<<"LINE:"<<line<<"\n";
			if ( line[0] == '<' )	//replace '<' with '!' for running multiple DFAs
			{
				DFAno++;
				(*dfa).resize( DFAno+1 );
				(*validTokenType).resize( DFAno+1 );
				MaxState = 0;
				// mapping between final state and token class
				char state[1024];
				char tokenType[1024];
				char *pch;
				pch = strtok ( line, "<,>" );
				//cout<<"Final states:\n";
				while ( pch != NULL )
				{
					/*uncomment this for running multiple DFAs
					if ( strcmp (pch, "!") == 0 )
					{
						pch = strtok ( NULL, "<,>" );
						continue;
					}*/
					strcpy (state, pch);
					//printf ( "%s  ", state );	//state
					pch = strtok ( NULL, "<,>" );
					strcpy (tokenType, pch);
					//printf ( "%s\n", tokenType );	//accepting token type
					(*validTokenType)[DFAno][atoi(state)] = tokenType;	//add to acceptClassType
					pch = strtok ( NULL, "<,>" );
					//cout<<"PCH"<<pch[0]<<"\n";
				}
				//cout<<"Transition functions:\n";
			}
			else
			{
				// transition function
				if ( line[0] == '#' )
				{
					//state changed
					char *pch = strtok ( line, "#,:" );
					strcpy( curState, pch );
					if ( MaxState < atoi(curState) + 1 )
					{
						MaxState = atoi(curState) +1;
						(*dfa)[DFAno].resize(MaxState);
					}
					//cout<<"curstate : "<<curState<<"\n";
				}
				else
				{
					//transition for a state continued
					char input[1024],nextState[1024];
					char *pch = strtok ( line, "," );
					//cout<<pch<<"  ";	//current input
					strcpy ( input, pch );
					pch = strtok ( NULL, "");
					strcpy ( nextState, pch );
					//cout<<nextState<<"\n";	//next state
					// Resize dfa to maximum possible states
					if ( MaxState < (atoi(nextState) + 1 ) )
					{
						MaxState = atoi(nextState) +1;
						(*dfa)[DFAno].resize(MaxState);
					}
					(*dfa)[DFAno][atoi(curState)][input[0]] = atoi(nextState);
				}
			}
			
		}
		dfainput.close();
	}
}

/* Determines Format of the lexemes to be added in the output file of source code
 * FORMAT : '< TOKEN_TYPE, INDEX >'
 */
void constructLexeme ( char *tokenType, char *lexeme, vector < string > *symbolTable , char *token )
{
	strcpy ( lexeme, "< " );
	strcat (lexeme, tokenType );
	if ( strcmp ( tokenType, "ID" ) == 0 || strcmp (tokenType, "CONSTANT" ) == 0)
	{
		int index = addToSymTab ( token, symbolTable);
		strcat ( lexeme, ", " );
		strcat ( lexeme, (char*)intToStr(index).c_str() );
	}
	strcat ( lexeme, " > " );
	//cout<<"LEXEME:"<<lexeme<<"\n";
}


void createSymTabFile (  char *filename, vector < string > *symbolTable )
{
	char symTabFile[1024];
	strcpy (symTabFile, filename );
	strcat ( symTabFile, ".symtab" );
	fstream symTabPtr ;
	symTabPtr.open(symTabFile , fstream::out | fstream::trunc );
	if ( symTabPtr.is_open () )
	{
		for ( unsigned int i = 0; i < (*symbolTable).size(); i++ )
		{
			string entry = intToStr ( i+1 ) + ", " + (*symbolTable)[i] + "\n";
			symTabPtr << entry;
		}
		symTabPtr.close();
	}
}
