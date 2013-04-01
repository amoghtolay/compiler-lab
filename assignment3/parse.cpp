#include<stack>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<stdlib.h>
#include<set>
using namespace std;

#define PARSE_TABLE_ROW_SEP '\n'
#define PARSE_TABLE_COL_SEP ','
#define PARSE_TABLE_FILE "parseTable"
#define PROGRAM_FILE "lexOutput"
#define GRAMMAR_FILE "grammar"
#define END_OF_STACK "$"
#define EPSILON "epsilon"
int getLineNo(ifstream *programFile,string filename);
string populateTermNontermVct(vector<string> *, vector<string> *);
vector<vector<vector<string> > > buildParseTable(int,int);
bool parseOneToken(stack<string> *,string token);
int main(){
	stack <string> parseStack;
	parseStack.push(END_OF_STACK);
	ifstream programFile;
	programFile.open(PROGRAM_FILE);
	string token;
	vector<string> termVct,nonTermVct;

	string startSymbol = populateTermNontermVct(&termVct,&nonTermVct);
	parseStack.push(startSymbol);
	vector<vector<vector<string> > > parseTable = buildParseTable(termVct.size(),nonTermVct.size());
	
	//intialized everything, real parsing begins
	//algo used :
	// while top != $:
	// 		If top is nonterm, lookup parse table. pop, and push the entry correponding to (prevTop,token in file). 
	// 		If the top is terminal symbol, check whether token in file == top.If true, pop and get next token from input file. else give error msg.
	// Now, if top = $, check for eof. if true, accept, else reject.
	// if eof occurs when stack is not yet empty (inside the while loop), reject.
	
	vector<string>::iterator it;
	vector<string>::reverse_iterator reverseIt;
	bool unexpectedToken= false;
	int posOfToken = 0; 											//pos of stacktop in non term vct
	bool isTokenInSymtab;
	int symtabEntryNo;
	
	//get first token from inputfile
	if(!(programFile>>token))
		token = EPSILON;
	//checking whether this token is of type id etc. by checking for a comma
	unsigned commaLoc = token.find(',');
	if(commaLoc != string::npos){
		isTokenInSymtab = true;
		symtabEntryNo = atoi((token.substr(commaLoc+1)).c_str());
		token = token.substr(0,commaLoc);
	}
	bool invalidToken = true;
	for(it = termVct.begin();it!=termVct.end();it++,posOfToken++){
		if(token == *it){
			invalidToken = false;
			break;
		}
	}
	
	if(invalidToken){
		cout<<token<<"'"<<"' is not a valid token!!!\n";
		exit(0);
	}
	
	////////////////////////////////////////////////////////////////////////////////
	while(parseStack.top()!=END_OF_STACK){		
		bool isNonTerm = false;
		cout<<"now stact top is "<<parseStack.top()<<endl;
		cout<<"next token is "<<token<<endl;
		cout<<"------------------------------------\n";
		//search for the sybol @ top in nonterm vct
		int posOfStackTop = 0; 											//pos of stacktop in non term vct
		for(it = nonTermVct.begin();it!=nonTermVct.end();it++,posOfStackTop++){
			if(parseStack.top() == *it){
				//nonterm!!!!
				isNonTerm = true;
				break;
			}
		}
		if(isNonTerm){
			//looking up in parsetable, pushing all the symbols in stack in reverse order
			
		
			if(parseTable[posOfStackTop][posOfToken].size()==0){
				//this means there is error.
				unexpectedToken = true;
				cout<<"unexpected token"<<token<<" found at line no. "<<getLineNo(&programFile,PROGRAM_FILE)<<"\n";
				exit(0);
			}
			cout<<"popping nonterm "<<(parseStack.top())<<endl;			
			parseStack.pop();
			for(reverseIt = parseTable[posOfStackTop][posOfToken].rbegin();reverseIt!=parseTable[posOfStackTop][posOfToken].rend();reverseIt++){
				
				parseStack.push(*reverseIt);
				cout<<"pushing "<<(parseStack.top())<<endl;
			}
		}
		else{
			if(token == parseStack.top()){
				cout<<"popping term "<<(parseStack.top())<<endl;
				parseStack.pop();
				//get next token
				if(!(programFile>>token))
					token = EPSILON;
				//checking whether this token is of type id etc. by checking for a comma
				unsigned commaLoc = token.find(',');
				if(commaLoc != string::npos){
					isTokenInSymtab = true;
					symtabEntryNo = atoi((token.substr(commaLoc+1)).c_str());
					token = token.substr(0,commaLoc);
				}
					
				posOfToken = 0;
				invalidToken = true;
				for(it = termVct.begin();it!=termVct.end();it++,posOfToken++){
					if(token == *it){
						invalidToken = false;
						break;		
					}
				}
				if(invalidToken){
					cout<<"'"<<token<<"' is not a valid token!!!\n";
					exit(0);
				}
			}
			else if(parseStack.top()==EPSILON)
				parseStack.pop();
				else{
					unexpectedToken = true;
					cout<<"unexpected token"<<token<<" found at line no. "<<getLineNo<<"\n";
					cout<<"expecting "<<parseStack.top()<<endl;
					exit(1);
				}
		}
	}
}

string populateTermNontermVct(vector<string> * termVct, vector<string> *nonTermVct){
	//returns the start symbol
	string startSymbol;
	ifstream grammarFile;
	string listOfTerms, listOfNonterms;
	grammarFile.open(GRAMMAR_FILE);
	getline(grammarFile, listOfTerms);
	getline(grammarFile, listOfNonterms);
	stringstream termStream, nontermStream;
	termStream<<listOfTerms;
	nontermStream<<listOfNonterms;
	string singleSymbol;
	while(termStream>>singleSymbol)
		(*termVct).push_back(singleSymbol);
	
	nontermStream>>startSymbol;
	(*nonTermVct).push_back(startSymbol);
	while(nontermStream>>singleSymbol)
		(*nonTermVct).push_back(singleSymbol);
	
	grammarFile.close();
	return startSymbol;
}
vector<vector<vector<string> > > buildParseTable(int noOfTerms, int noOfNonterms){
	ifstream parseTableFile;
	
	
	vector<vector<vector<string> > > parseTable;
	vector<string> prodRuleVct;
	prodRuleVct.resize(1," ");
	vector<vector<string> > rowVct;
	rowVct.resize(noOfTerms,prodRuleVct);
	parseTable.resize(noOfNonterms,rowVct);
	parseTable.clear();
	
	string rrow,prodRule,symbol;
	stringstream rowStream, prodRuleStream;
	parseTableFile.open(PARSE_TABLE_FILE);
	while(getline(parseTableFile,rrow,PARSE_TABLE_ROW_SEP)){
		rowStream.clear();
		rowStream<<rrow;
		rowVct.clear();
		
		while(getline(rowStream,prodRule,PARSE_TABLE_COL_SEP)){
			prodRuleStream.clear();
			prodRuleStream<<prodRule;
			prodRuleVct.clear();
			while(prodRuleStream>>symbol){
				
				if(symbol=="<"){
					//inside terminal symbol
					//irrespective of space, one single symbol until ">" occurs
					string termSymbol;
					prodRuleStream>>symbol;
					while(symbol!=">"){
						termSymbol+=symbol;
						prodRuleStream>>symbol;
					}
					symbol = termSymbol;
				}

				//otherwise, symbol = nonterminal symbol
				
				prodRuleVct.push_back(symbol);
			}
			rowVct.push_back(prodRuleVct);
		}
		parseTable.push_back(rowVct);
	}
	return(parseTable);
}

int getLineNo(ifstream *programFile,string filename){
	//USE CAREFULLY:POSSIBLY MODIFIES THE STREAM PASSED:
	int counter = 0;
	string garbage;
	while(getline((*programFile),garbage))
		counter++;
	int totalNoOfLines = 0;
	ifstream anotherStream;
	anotherStream.open(filename.c_str());
	while(getline(anotherStream,garbage)){
		totalNoOfLines++;
	}
	anotherStream.close();
	
	return(totalNoOfLines-counter+1);
}