# Shell file to take the input and output the DFA directly
make
./regexToNfa regexSpec.txt NFAout.txt
./nfaToDfa NFAout.txt DFAout.txt
rm NFAout.txt
