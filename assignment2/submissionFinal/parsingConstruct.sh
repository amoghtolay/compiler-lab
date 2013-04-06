#make
./recursion $1
./factor $1.leftFree $1.factored
./firstFollow $1.factored
./tableGen $1.factored $1.factored.first $1.factored.follow parseTable.txt
./parse $2 parseTable.txt $1.factored
rm parseTree.jpg
dot -Tjpg parseTree.dot > parseTree.jpg
