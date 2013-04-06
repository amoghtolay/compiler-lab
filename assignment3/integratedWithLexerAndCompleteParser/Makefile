all:
	g++ -Wall regexToNFA.cpp -o regexToNFA
	g++ -Wall nfaToDfa.cpp -o nfaToDfa
	g++ -Wall DFAtoMinDFA.cpp -o minDFA
	g++ -Wall lexicalAnalyser.cpp -o lex
# following are commands for parsing	
	g++ -Wall leftRecursion.cpp -o recursion
	g++ -Wall parsingTableGenerate.cpp -o tableGen
	g++ -Wall first-follow.cpp -o firstFollow
	g++ -Wall leftFactoring.cpp -o factor
	g++ -Wall parse.cpp -o parse
clean:
	rm -rf factor
	rm -rf recursion
	rm -rf firstFollow
	rm -rf tableGen
	rm -rf parse 
# lexer clean command	
	rm -f regexToNFA nfaToDfa minDFA lex
