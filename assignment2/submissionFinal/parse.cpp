#include<stack>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<stdlib.h>
#include<set>
using namespace std;
#define DOT_FILE_NAME "parseTree.dot"
#define PARSE_TABLE_ROW_SEP '\n'
#define PARSE_TABLE_COL_SEP ','
#define LOG_FILE "logFile"
//#define PROGRAM_FILE "lexOutput"
//#define GRAMMAR_FILE "grammar"
#define END_OF_STACK "$"
#define EPSILON "epsilon"
int getLineNo(ifstream *programFile,string filename);
string populateTermNontermVct(vector<string> *, vector<string> *,string);
vector<vector<vector<string> > > buildParseTable(int,int,string);
bool parseOneToken(stack<string> *,string token);
int main(int argc,char *argv[]){
	stack <string> parseStack;
	parseStack.push(END_OF_STACK);
	ifstream programFile;
	string token;
	vector<string> termVct,nonTermVct;
	string PROGRAM_FILE = argv[1];
	string PARSE_TABLE_FILE = argv[2];
	string GRAMMAR_FILE = argv[3];
	string startSymbol = populateTermNontermVct(&termVct,&nonTermVct,GRAMMAR_FILE);
	programFile.open(PROGRAM_FILE.c_str());
	ofstream outStream, dotFileStream;
	parseStack.push(startSymbol);
    dotFileStream.open(DOT_FILE_NAME);
    dotFileStream<<"digraph parseTree{\n\t";
	vector<vector<vector<string> > > parseTable = buildParseTable(termVct.size(),nonTermVct.size(),PARSE_TABLE_FILE);
	outStream.open(LOG_FILE);
	//intialized everything, real parsing begins
	//algo used :
	// while top != $:
	// 		If top is nonterm, lookup parse table. pop, and push the entry correponding to (prevTop,token in file). 
	// 		If the top is terminal symbol, check whether token in file == top.If true, pop and get next token from input file. else give error msg.
	// Now, if top = $, check for eof. if true, accept, else reject.
	// if eof occurs when stack is not yet empty (inside the while loop), reject.
	int parseTreeNodeCounter = 0;
    stack<int> stackForParseTree;
    dotFileStream<<parseTreeNodeCounter<<"[label = \""<<startSymbol<<"\"];\n\t";
    stackForParseTree.push(parseTreeNodeCounter);
    parseTreeNodeCounter++;
	vector<string>::iterator it;
	vector<string>::reverse_iterator reverseIt;
	int posOfToken = 0; 											//pos of stacktop in non term vct
	//bool isTokenInSymtab;
	//int symtabEntryNo;
	
	//get first token from inputfile
	if(!(programFile>>token))
		token = END_OF_STACK;
	//checking whether this token is of type id etc. by checking for a comma
	unsigned commaLoc = token.find(',');
	if(commaLoc+1 != 0){
		//isTokenInSymtab = true;
		//symtabEntryNo = atoi((token.substr(commaLoc+1)).c_str());
		token = token.substr(0,commaLoc);
        token+=">";
	}
	bool invalidToken = true;
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
	
	////////////////////////////////////////////////////////////////////////////////
	while(parseStack.top()!=END_OF_STACK){		
		bool isNonTerm = false;
		outStream<<"now stack top is "<<parseStack.top()<<endl;
		outStream<<"next token is "<<token<<endl;
		outStream<<"------------------------------------\n";
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
				cout<<"unexpected token '"<<token<<"' found at line no. "<<getLineNo(&programFile,PROGRAM_FILE)<<"\n";
				exit(0);
			}
            string oldTop = parseStack.top();
            int parentNode = stackForParseTree.top();
            stackForParseTree.pop();
			outStream<<"popping nonterm "<<oldTop<<endl;			
			parseStack.pop();
			for(reverseIt = parseTable[posOfStackTop][posOfToken].rbegin();reverseIt!=parseTable[posOfStackTop][posOfToken].rend();reverseIt++){
				parseStack.push(*reverseIt);
				outStream<<"pushing "<<(parseStack.top())<<endl;
                dotFileStream<<parseTreeNodeCounter<<"[label = \""<<(*reverseIt)<<"\"];\n\t";
                stackForParseTree.push(parseTreeNodeCounter);
			    parseTreeNodeCounter ++;
            }
            int counterForInsertingEdgesInDot = parseTreeNodeCounter;
			for(it = parseTable[posOfStackTop][posOfToken].begin();it!=parseTable[posOfStackTop][posOfToken].end();it++){
                counterForInsertingEdgesInDot--;
				dotFileStream<<"\n\t"<<parentNode <<" -> "<<counterForInsertingEdgesInDot<<";";
            }
		}
		else{
            stackForParseTree.pop();
			if(token == parseStack.top()){
				outStream<<"popping term "<<(parseStack.top())<<endl;
				parseStack.pop();
				//get next token
				invalidToken = true;
				if(!(programFile>>token)){
					token = END_OF_STACK;
					invalidToken = false;
				}
				//checking whether this token is of type id etc. by checking for a comma
				unsigned commaLoc = token.find(',');
				if(commaLoc+1 != 0){
					//isTokenInSymtab = true;
					//symtabEntryNo = atoi((token.substr(commaLoc+1)).c_str());
					token = token.substr(0,commaLoc);
                    token+=">";
				}
                //isTokenInSymtab = false;
		    	//cout<<termVct.size()<<'\t'<<parseTable[0].size()<<endl;	
    			posOfToken = 0;
	    		for(it = termVct.begin();it!=termVct.end();it++,posOfToken++){
		    		if(token == *it){
			   			invalidToken = false;
			    		break;		
			    	}
			   	}
				if(token == EPSILON && invalidToken){
					cout<<"you must also include epsilon in your list of ";
					cout<<"nonterminals in the grammar file\n";
					exit(1);
				}
				if(invalidToken){
					cout<<"'"<<token<<"' is not a valid token!!!\n";
					exit(0);
				}
			}
			else if(parseStack.top()==EPSILON){
                outStream<<"popping epsilon\n";
				parseStack.pop();
            }
            else{
                cout<<"unexpected token '"<<token<<"'found at line no. "<<getLineNo(&programFile,PROGRAM_FILE);
                cout<<"\nexpecting '"<<parseStack.top()<<"' here!\n";
                exit(0);
            }
		}
	}
    dotFileStream<<"}";
    cout<<"parsing succesful. See logfile for stack at each step, or see parseTree.jpg for parse tree (mirror image).\n";
}

string populateTermNontermVct(vector<string> * termVct, vector<string> *nonTermVct,string GRAMMAR_FILE){
	//returns the start symbol
	string startSymbol;
	ifstream grammarFile;
	string listOfTerms, listOfNonterms;
	grammarFile.open(GRAMMAR_FILE.c_str());
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
vector<vector<vector<string> > > buildParseTable(int noOfTerms, int noOfNonterms,string PARSE_TABLE_FILE){
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
	parseTableFile.open(PARSE_TABLE_FILE.c_str());
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
