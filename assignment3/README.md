This is the part of the compiler that deals with parsing. In this part, the first and follow sets for the grammar are computed automatically. Then, using these sets, the parsing table is constructed. We're implementing a LL(1) parser, so there won't be any duplicate entries in the parse table. After the parse table is constructed, actual parsing takes place with appropriate parse stack, and finally a parse tree is generated.

Bugs:

