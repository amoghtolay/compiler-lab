# Shell file to take the input and output the DFA directly
make
./regexToNFA regexSpec.txt NFAout.txt
./nfaToDfa NFAout.txt DFAout.txt
./minDFA DFAout.txt MinDfa.txt
./lex MinDfa.txt sampleInput tokenized
