#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <set>

using namespace std;

void constructDfa ( char DFAinput[1024], vector < vector < map < char, int  > > > *dfa , vector < map < int, string > > *acceptClassType, set < int > *final, set < int > *nonfinal , set < char > *alphabets, vector < set < int > > *tokenStateSets );
pair < int, int > tokenize ( char *token, vector < vector < map < char,int > > > dfa );
int addToSymTab ( char *token, vector < string > *symbolTable);
void constructLexeme ( char *classType, int index ,  char *lexeme );
void createSymTabFile (  vector < string > *symbolTable );

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

void arrayprint ( vector < vector < char > > *Distinct, int numStates)
{
	cout<<"\t";
	for ( int i = 0; i < numStates; i++ )
		cout<<i<<"\t";
	cout<<endl;
	cout<<"\t";
	for ( int i = 0; i < 7*numStates; i++ )
		cout<<"_";
	cout<<endl;
	// main
	for ( int i = 0; i < numStates; i++ )
	{
		cout<<i<<".\t|";
		for ( int j = 0; j < numStates; j++ )
			cout<<(*Distinct)[i][j]<<":\t";
		cout<<endl;
	}
	cout<<"\t|";
	for ( int i = 0; i < 7*numStates; i++ )
		cout<<"_";
	cout<<endl;
}

void printdfa( vector < vector < map <char,int> > > *dfa )
{
	for ( unsigned int i = 0; i < (*dfa)[0].size(); i++ )
	{
		cout<<"current state : "<<i<<"\n";
		for ( map < char, int >::iterator transitionsIt = (*dfa)[0][i].begin(); transitionsIt != (*dfa)[0][i].end(); transitionsIt++ )
		{
			cout << (transitionsIt)->first<<"\t, "<<(transitionsIt)->second<<endl;
		}
	}
}

int main(int argc, char **argv)
{
	vector < map < int, string > > validTokenType;
	vector < vector < map < char, int  > > > dfa;
	dfa.resize(1);
	vector < vector < map < char, int  > > > mindfa;
	mindfa.resize(1);
	vector < string > symbolTable;
	vector < set < int > > uniqueStates;
	vector < set < int > > tokenStateSets;
	set < int > final;
	set < int > nonfinal;
	set < char > alphabets;
	char DFAinput[1024];
	if (argc < 3 )
	{
		cout<<"provide input and output filenames\n";
		exit(1);
	}
	else
	{
		strcpy (DFAinput, argv[1] );
	}
	
	// Reconstruct DFA
	// DFA should not modified later
	constructDfa ( DFAinput, &dfa , &validTokenType, &final, &nonfinal , &alphabets, &tokenStateSets );
	/* Now Do DFA Minimization */
	int numStates = dfa[0].size();	//number of states in the dfa
	//cout<<numStates<<"\n";
	// Declare 'Distinct' vector
	vector < vector < char > > Distinct;
	Distinct.resize(numStates);
	for ( int i =0; i < numStates; i++ )
		Distinct[i].resize(numStates);
	
	for ( int i = 0; i < numStates ; i++ )
	{
		for ( int j = 0; j < numStates ; j++ )
			if ( i != j )
				Distinct[i][j] = '$';
			else
				Distinct[i][j] = '\0';
	}
	//arrayprint(&Distinct, numStates);
	for ( vector < set < int > >::iterator it = tokenStateSets.begin(); it != tokenStateSets.end(); it++ )
	{
		for ( set < int >::iterator itF = (*it).begin(); itF != (*it).end(); itF++ )
		{
			for ( set < int >::iterator itNF = (*it).begin(); itNF != (*it).end() ; itNF++ )
			{
				Distinct[*itF][*itNF] = '\0';
				Distinct[*itNF][*itF] = '\0';
			}
		}
	}

	//arrayprint(&Distinct, numStates);

	// create Distinct table
	bool tableChanged = true;
	while ( tableChanged )
	{
		tableChanged = false;
		for ( int i = 0; i < numStates; i++ )
		{
			for ( int j = 0; j < numStates; j++ )
			{
				for ( set < char >::iterator it = alphabets.begin(); it != alphabets.end(); it++ )
				{
					int p, q;
					p = dfa[0][i][*it];
					q = dfa[0][j][*it];
					if ( Distinct[i][j] == '\0' && Distinct[p][q] != '\0' )
					{
						Distinct[i][j] = *it;
						tableChanged = true;
						//cout<<"I:"<<i<<"\tJ:"<<j<<"\t"<<Distinct[i][j]<<endl;
					}
				}
			}
		}
	}
	//arrayprint(&Distinct, numStates);

	//cout<<"\nconstructing unique sets of state....\n";
	// create minimal DFA
	for ( int i = 0; i < numStates; i++ )
	{
		bool stateUsed = false;
		for ( vector < set < int > >::iterator it = uniqueStates.begin(); it != uniqueStates.end(); it++ )
		{
			if ( (*it).find( i ) != (*it).end() )
			{
				stateUsed = true;
				continue;
			}
		}
		if (stateUsed == true )
			continue;
		// create set i;
		set < int > similarStates;
		similarStates.insert(i);
		for ( int j = i+1; j < numStates; j++ )
		{
			if ( Distinct[i][j] == '\0' )
			{
				//add to set i
				similarStates.insert(j);
			}
		}
		uniqueStates.push_back(similarStates);
	}
	
	//cout<<"\ncreating transitions\n";
	// format of dfa : vector < vector < map < char, int  > > > Mindfa;
	//create minimal DFA ( transitions ) using unique sets 
	
	vector < map < int, string > > newFinalStateMapping;
	newFinalStateMapping.resize(1);
	
	mindfa[0].resize( uniqueStates.size() );
	int curStateno = -1;
	for ( vector < set < int > >::iterator it = uniqueStates.begin(); it != uniqueStates.end(); it++ )
	{
		curStateno++;
		//cout<<"stateno : "<<curStateno<<endl;
		set < int >::iterator minDFAstateSetIt = (*it).begin();
		for ( map < char, int >::iterator prodRuleIt = dfa[0][*minDFAstateSetIt].begin(); prodRuleIt != dfa[0][*minDFAstateSetIt].end(); prodRuleIt++ )
		{
			int nextState = (*prodRuleIt).second;
			char curInputSymbol  = (*prodRuleIt).first;
			cout<<(*prodRuleIt).first<<" : "<<nextState<<"\t" ;
			
			int newNextState = -1;
			for (vector <set<int > >::iterator uniqueStateSetIt = (uniqueStates).begin(); uniqueStateSetIt != (uniqueStates).end(); uniqueStateSetIt++ )
			{
				newNextState++;
				set < int >::iterator isFound = (*uniqueStateSetIt).find(nextState);
				if(isFound!=(*uniqueStateSetIt).end())
				{
					// add transition to new dfa
					//found! found!! found!!!
					mindfa[0][curStateno][curInputSymbol]  = newNextState;
					map < int, string >::iterator isFinal = (validTokenType[0]).find(nextState);
					if ( isFinal != validTokenType[0].end() )
						newFinalStateMapping[0][newNextState] = (*isFinal).second ;
				}
				
			}
			
		}
		cout<<endl;
	}
	
	//cout<<"original dfa\n";
	printdfa(&dfa);
	cout<<endl;
	// printing minimal dfa
	//cout<<"minimal dfa\n";
	printdfa(&mindfa);
	//cout<<endl;
	cout<<"all final states:\n";
	
	//writing in output file
	char mindfafile[1024];
	strcpy ( mindfafile, argv[2] );

	fstream dfaOutput;
	dfaOutput.open(mindfafile, fstream::out | fstream::trunc );
	if ( dfaOutput.is_open() )
	{
		string line="";
		for ( map < int, string >::iterator it = newFinalStateMapping[0].begin(); it != newFinalStateMapping[0].end(); it++ )
		{
			line=line+"<";
			line =line + intToStr((*it).first);
			line=line+",";
			line=line+(*it).second;
			line=line+">";
			//cout<<(*it).first<<","<<(*it).second<<endl;
		}
		line=line+"\n";
		dfaOutput<<line;
		for ( unsigned int i = 0; i < mindfa[0].size(); i++ )
		{
			line = "#"+intToStr(i)+":\n";
			dfaOutput<<line;
			
			for ( map < char, int >::iterator transitionsIt = mindfa[0][i].begin(); transitionsIt != mindfa[0][i].end(); transitionsIt++ )
			{
				line =  transitionsIt->first;
				line = line + ",";
				line = line + intToStr(transitionsIt->second) + "\n";
				dfaOutput<<line;
				//cout << (transitionsIt)->first<<","<<(transitionsIt)->second<<endl;
			}
		}
	}

	return 0;
}

/*------------End of Main----------------------*/

/* Bring DFA to memory from given DFAfile
 * current delimiter for different DFA is : '!'
 */
void constructDfa ( char DFAinput[1024], vector < vector < map < char, int  > > > *dfa , vector < map < int, string > > *validTokenType, set < int > *final, set < int > *nonfinal, set < char > *alphabets , vector < set < int > > *tokenStateSets )
{
	fstream dfainput;
	dfainput.open(DFAinput);
	int DFAno = -1;	
	if ( dfainput.is_open() )
	{
		char curState[1024];
		char line[1024];
		int MaxState;
		while ( dfainput.getline ( line, 1024 ) )
		{
			//if ( !line )
			//	continue;
			//cout<<"LINE:"<<line<<"\n";
			if ( line[0] == '!' )
			{
				DFAno++;
				(*dfa).resize( DFAno+1 );
				(*validTokenType).resize( DFAno+1 );
				MaxState = 0;
				// mapping between final state and token class
				char state[1024];
				char tokenType[1024];
				char *pch;
				set < int > acceptStateSets;
				pch = strtok ( line, "<,>" );
				//cout<<"Final states:";
				while ( pch != NULL )
				{
					if ( strcmp (pch, "!") == 0 )
					{
						(*tokenStateSets).push_back ( acceptStateSets );
						acceptStateSets.clear();
						pch = strtok ( NULL, "<,>" );
						if ( pch == NULL )
							break;
						strcpy (tokenType, pch);
						printf ( "\n%s\t", tokenType );	//accepting token type
						pch = strtok ( NULL, "<,>" );
					}
					strcpy (state, pch);
					printf ( "%s\t", state );	//state
					(*final).insert(atoi(state));
					acceptStateSets.insert(atoi(state));
					(*validTokenType)[DFAno][atoi(state)] = tokenType;	//add to acceptClassType
					pch = strtok ( NULL, "<,>" );
					//cout<<"PCH"<<pch[0]<<"\n";
				}
				cout<<"\nTransition functions:\n";
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
					(*alphabets).insert(input[0]);	//add all alphabets
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
		set < int > finalStates;
		for ( vector < set < int > >::iterator it = (*tokenStateSets).begin(); it != (*tokenStateSets).end(); it++ )
		{
			for ( set < int >::iterator itF = (*it).begin(); itF != (*it).end(); itF++ )
			{
				finalStates.insert(*itF);
			}
		}
		// create non final state set
		set < int > acceptStateSets;
		for ( int i = 0; i < MaxState; i++ )
		{
			set < int >::iterator it = finalStates.find(i);
			if(  it == finalStates.end() )
			{
				acceptStateSets.insert(i);
				//cout<<i<<" *)+ ";
			}
		}
		finalStates.clear();
		//cout<<endl;
		(*tokenStateSets).push_back(acceptStateSets);
	}
	else
	{
		cout<<"Input file not found.\n";
		exit(1);
	}
}
