#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <set>
#include <utility>
#define EPSILON (char)20
#define TRANSITION_TABLE_SEP ' '

using namespace std;

void buildNfa(vector<pair<string,int> > *, vector<vector<int> > *,char*);
void getEpsClosure(vector<vector<int> > ,vector<vector<int> > *);
void initialStepEpsClosure(vector<vector<int> > ,vector<vector<int> > *);
int nextStepEpsClosure(vector<vector<int> > *);
void getFinalStates(vector <pair<string,int> > *, vector<set<int> > *,vector<pair<string,vector <int> > > *);
int buildDfa(vector<vector<int> >*,vector<vector<int> >*,vector<vector<int> >*,vector<set<int> >*); 
void generateOutputFileMinimal(vector<vector<int> > *, vector<pair<string,vector<int> > > *,string );
void generateOutputFileNonMinimal(vector<vector<int> > *, vector<pair<string,vector<int> > > *,string );
void generateTransitionTable(vector<vector<int> >*);
int main(int argc, char *argv[]){
	vector<vector<int> > nfa,epsClosureContainer,dfaProdRules;
	vector<pair<string,int> > acceptingStates;
	vector<set<int> > dfaStateSets;
	if (argc < 3) {
		cout<<"ERROR: No NFA file provided";
		exit(1);
	}

	char *fileName, *outputFile;
	fileName = argv[1];
	outputFile = argv[2];
	buildNfa(&acceptingStates,&nfa, fileName);
	getEpsClosure(nfa, &epsClosureContainer);
	set<int>::iterator dfaStateSetIt;
	while( buildDfa(&nfa,&epsClosureContainer,&dfaProdRules,&dfaStateSets)!=0 );
	
	vector<pair<string,vector <int> > > dfaAcceptingStates;
	getFinalStates(&acceptingStates,&dfaStateSets,&dfaAcceptingStates);
	generateOutputFileMinimal(&dfaProdRules,&dfaAcceptingStates,outputFile);
	generateTransitionTable(&dfaProdRules);
}
void buildNfa(vector<pair<string,int> > *acceptingStates,vector<vector<int> > *nfa,char* fileName){

	ifstream fp;
	string line;
	fp.open (fileName);
	char comma;
	int noOfStates;
	pair <string,int> thisAcceptingState;
	
	//getting the list of accepting states
	
	while(fp>>line){
		if(!isalpha(line[0]))
			break;
		thisAcceptingState.first = line;
		fp>>thisAcceptingState.second;
		fp>>comma;
		(*acceptingStates).push_back(thisAcceptingState);
	}
	noOfStates = atoi(line.c_str());
	(*nfa).resize(noOfStates);
	int character,startState,endState;
	while(fp>>character){
		fp>>startState;
		fp>>endState;
		(*nfa)[startState].push_back(character);
		(*nfa)[startState].push_back(endState);
	}
}
void getEpsClosure(vector<vector<int> > nfa, vector<vector<int> > *epsClosureContainer){
	initialStepEpsClosure(nfa,epsClosureContainer);
	while(nextStepEpsClosure(epsClosureContainer)!=0);
}
void initialStepEpsClosure(vector<vector<int> > nfa, vector<vector<int> > *epsClosureContainer){
	vector<int> epsClosureOfState;
	
	for(unsigned i=0;i<nfa.size();i++){
		epsClosureOfState.clear();
		epsClosureOfState.push_back(i);
		for( unsigned int j=0; j<nfa[i].size(); j+=2){
			if((char)nfa[i][j]==EPSILON)
				epsClosureOfState.push_back(nfa[i][j+1]);
		}
		(*epsClosureContainer).push_back(epsClosureOfState);
	}
}
int nextStepEpsClosure(vector<vector<int> > *epsClosureContainer){
	unsigned int i,j,k,l;
	int isPresent;
	int didAnyClosureChange=0;
	vector<int> epsClosureOfState;
	vector<int> addedElementsInClosure;
	addedElementsInClosure.clear();
	int epsClosuredState,epsOfepsClosuredState;
	for(i=0;i<(*epsClosureContainer).size();i++){
		addedElementsInClosure.clear();
		epsClosureOfState.clear();
		epsClosureOfState = (*epsClosureContainer)[i];
		
		for(j=0;j<epsClosureOfState.size();j++){
			epsClosuredState = epsClosureOfState[j];
			for(k=0;k<(*epsClosureContainer)[epsClosuredState].size();k++){
				isPresent=0;
				epsOfepsClosuredState = (*epsClosureContainer)[epsClosuredState][k];
				for(l=0;l<epsClosureOfState.size();l++){
					if(epsClosureOfState[l]==epsOfepsClosuredState){
						isPresent=1;
					}
				}
				for(l=0;l<addedElementsInClosure.size();l++){
					if(addedElementsInClosure[l]==epsOfepsClosuredState){
						isPresent=1;
					}
				}
				if(isPresent!=1){
					addedElementsInClosure.push_back(epsOfepsClosuredState);
				}
			}
		}
		if(addedElementsInClosure.size()>0){
			didAnyClosureChange=1;
			for(k=0;k<addedElementsInClosure.size();k++){
				(*epsClosureContainer)[i].push_back(addedElementsInClosure[k]);
			}
		}
	}
	return didAnyClosureChange;
}
int buildDfa(vector<vector<int> > *nfa, vector<vector<int> > *epsClosureContainer,vector<vector<int> >* dfaProdRules, vector<set<int> > *dfaStateSets){
	unsigned int i,j;
	if((*dfaStateSets).size()==0){
		set<int> state0;
		for(i=0;i<(*epsClosureContainer)[0].size();i++)
			state0.insert((*epsClosureContainer)[0][i]);
		(*dfaStateSets).push_back(state0);
		return 1;
	}
	int curDfaState,curNfaState,nfaStateTobeAdded;		// the state of dfa of which prod rules will be determined in this step
	set<int> followSet;			//follow set of this dfa state: set of char which have a prod rule from some nfa state in this state to some other nfa state
	set<int>::iterator dfaStateSetIt,followSetIt;
	curDfaState = (*dfaProdRules).size();
	set<int> curStateSet = (*dfaStateSets)[curDfaState];
	for(dfaStateSetIt = curStateSet.begin();dfaStateSetIt!= curStateSet.end();dfaStateSetIt++){
		curNfaState=*dfaStateSetIt;
		for(i=0;i<(*nfa)[curNfaState].size();i+=2){
			if((*nfa)[curNfaState][i]!=EPSILON){
				followSet.insert((*nfa)[curNfaState][i]);
			}
		}
	}
	set<int> nextStateSet;
	for(followSetIt = followSet.begin();followSetIt!= followSet.end();followSetIt++){
		nextStateSet.clear();
		//looking at each char in follow set
		//state set to which transitions will occur if this char is found
		for(dfaStateSetIt = curStateSet.begin(); dfaStateSetIt!=curStateSet.end();dfaStateSetIt++){
			curNfaState=*dfaStateSetIt;
			for(i=0;i<(*nfa)[curNfaState].size();i+=2){
				if((*nfa)[curNfaState][i]==*followSetIt){
					//insert the eps- closure that nfa state set to next state set
					nfaStateTobeAdded = (*nfa)[curNfaState][i+1];
					for(j=0;j<(*epsClosureContainer)[nfaStateTobeAdded].size();j++){
						nextStateSet.insert((*epsClosureContainer)[nfaStateTobeAdded][j]);
					}
				}
			}
		}
		int nextStateSetNo=-1;
		//check if this state set exists: if not add it to the vector of final state sets
		for(i=0;i<(*dfaStateSets).size();i++){
			if((*dfaStateSets)[i]==nextStateSet){
				//found!!
				nextStateSetNo=i;
				break;
			}
		}
		if(nextStateSetNo==-1){
			nextStateSetNo = (*dfaStateSets).size();
			(*dfaStateSets).push_back(nextStateSet);
		}
		//adding an entry in prod rules
		if( unsigned (curDfaState) == (*dfaProdRules).size()){
			//first prod rule
			vector<int> temp;
			temp.push_back(*followSetIt);
			temp.push_back(nextStateSetNo);
			(*dfaProdRules).push_back(temp);			
		}
		else{
			(*dfaProdRules)[curDfaState].push_back(*followSetIt);
			(*dfaProdRules)[curDfaState].push_back(nextStateSetNo);
		}
	}
	if(followSet.size()==0){
		//no outward transition from this dfa state
		//adding empty prod rule
		vector<int> temp;
		(*dfaProdRules).push_back(temp);
	}
	if((*dfaProdRules).size()==(*dfaStateSets).size())
		return 0;
	else 
		return 1;
}
void getFinalStates(vector <pair<string,int> > *nfaAcceptingStates, vector<set<int> > *dfaStateSet,vector<pair<string,vector <int> > > *dfaAcceptingStates){
	unsigned int i,j;
	int noOfDfaStates;
	noOfDfaStates = (*dfaStateSet).size();
	bool isStateBooked[noOfDfaStates];
	for(i=0;i<(unsigned)noOfDfaStates;i++)
		isStateBooked[i]=false;
	pair<string,vector<int> > thisKeywordAcceptingState;
	vector<int> acceptingDfaStateSet;
	string keyword;
	set<int>::iterator it;
	for(i=0;i<(*nfaAcceptingStates).size();i++){
		acceptingDfaStateSet.clear();
		keyword.clear();
		keyword = (*nfaAcceptingStates)[i].first;
		for(j=0;j<(*dfaStateSet).size();j++){
			it = (*dfaStateSet)[j].find((*nfaAcceptingStates)[i].second);
			if(it!=(*dfaStateSet)[j].end())
				//found!!
				if(!isStateBooked[j]){
					acceptingDfaStateSet.push_back(j);
					isStateBooked[j] = true;
				}
		}
		thisKeywordAcceptingState.first = keyword;
		thisKeywordAcceptingState.second=acceptingDfaStateSet;
		(*dfaAcceptingStates).push_back(thisKeywordAcceptingState);
	}
}
void generateOutputFileMinimal(vector<vector<int> > *dfaProdRules, vector<pair<string,vector<int> > > *dfaAcceptingStates,string fileName){
	ofstream fp;
	unsigned int i,j;
	fp.open (fileName.c_str(), ios::out); 
	//outputting first line:final states
	for(i=0;i<(*dfaAcceptingStates).size();i++){
		fp<<"!<";
		fp<<(*dfaAcceptingStates)[i].first;
		fp<<',';
		for(j=0;j<(*dfaAcceptingStates)[i].second.size();j++){
			fp<<(*dfaAcceptingStates)[i].second[j];
			fp<<',';
		}
		fp<<'>';
	}
	fp<<"!\n";
	//outputting subsequent lines: production rules
	for(i=0;i<(*dfaProdRules).size();i++){
		fp<<'#'<<i<<":\n";
		for(j=0;j<(*dfaProdRules)[i].size();j+=2)
			fp<<(char)(*dfaProdRules)[i][j]<<','<<(*dfaProdRules)[i][j+1]<<'\n';
	}
}
void generateOutputFileNonMinimal(vector<vector<int> > *dfaProdRules, vector<pair<string,vector<int> > > *dfaAcceptingStates,string fileName){
	ofstream fp;
	unsigned int i,j;
	fp.open (fileName.c_str(), ios::out); 
	//outputting first line:final states
	for(i=0;i<(*dfaAcceptingStates).size();i++){
		for(j=0;j<(*dfaAcceptingStates)[i].second.size();j++){
			fp<<'<'<<(*dfaAcceptingStates)[i].second[j]<<','<<(*dfaAcceptingStates)[i].first<<'>';
		}
	}
	fp<<"\n";
	//outputting subsequent lines: production rules
	for(i=0;i<(*dfaProdRules).size();i++){
		fp<<'#'<<i<<":\n";
		for(j=0;j<(*dfaProdRules)[i].size();j+=2)
			fp<<(char)(*dfaProdRules)[i][j]<<','<<(*dfaProdRules)[i][j+1]<<'\n';
	}
}


void generateTransitionTable(vector<vector<int> >* dfa){
	ofstream fp;
	fp.open ("transitionTable", ios::out); 
	unsigned int i,j;
	unsigned int noOfStates = (*dfa).size();
	string transitionTable[noOfStates][noOfStates];
	int inputSymbol,nextState;
	
	for(i=0;i<noOfStates;i++){
		for(j=0;j<(*dfa)[i].size();j+=2){
			inputSymbol = (*dfa)[i][j];
			nextState = (*dfa)[i][j+1];
			transitionTable[i][nextState] += (char)inputSymbol;
		}
	}
	for(i=0;i<noOfStates;i++)
		fp<<i<<TRANSITION_TABLE_SEP;
	fp<<endl;
	for(i=0;i<noOfStates;i++){
		fp<<i<<TRANSITION_TABLE_SEP;
		for(j=0;j<noOfStates;j++)
			fp<<transitionTable[i][j]<<TRANSITION_TABLE_SEP;
		fp<<endl;
	}
	fp.close();
}
