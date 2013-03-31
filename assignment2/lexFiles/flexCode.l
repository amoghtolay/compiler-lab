%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	
	int symTabCount = 0, lineCount = 1;
	char tokenName[1000000] = "";
	char symTabContents[100000] ="";
	
	int lookupEntrySymTab ( char* symTabContents, char symbol[256] ){	
		int matchIndex = -1;
		char *pch = strstr ( symTabContents, symbol );
		if(pch){
			if(pch-symTabContents ==0)
				pch = strstr ( pch, symbol );
			while(*(pch-1)!='\t'){	
				pch = strstr ( pch+1, symbol );
				if(!pch)
					break;
			}				
		}
		if ( pch != NULL ){
				//printf("found%d\n",matchIndex);
				pch-=2;
				while(isdigit(*pch)){
					pch--;
				}
				matchIndex = atoi(pch);
			}
		return matchIndex;
	}
%}
DIGIT [0-9]
LETTER [A-Za-z]
ID [A-Za-z][A-Za-z0-9]*

%%
[;]			{strcat(tokenName,"<SEMI> ");}
[\-]		{strcat(tokenName,"<MINUS> ");}
[+]			{strcat(tokenName,"<PLUS> ");}
[/]			{strcat(tokenName,"<DIV> ");}
[*]			{strcat(tokenName,"<MULT> ");}
[=] 		{strcat(tokenName,"<ASSIGN> ");}
[{]			{strcat(tokenName,"<LBRACE> ");}
[}]			{strcat(tokenName,"<RBRACE> ");}
[(] 		{strcat(tokenName,"<LPAREN> ");}
[)]			{strcat(tokenName,"<RPAREN> ");}
if			{strcat(tokenName,"<KEYWORD_IF> ");}
while		{strcat(tokenName,"<KEYWORD_WHILE> ");}
break		{strcat(tokenName,"<KEYWORD_BREAK> ");}
continue	{strcat(tokenName,"<KEYWORD_CONT> ");}
return		{strcat(tokenName,"<KEYWORD_RET> ");}
void 		{strcat(tokenName,"<VOID> ");}
int 		{strcat(tokenName,"<INT>");}
char 		{strcat(tokenName,"<CHAR> ");}
float 		{strcat(tokenName,"<FLOAT> ");}
{DIGIT}+	{
				// Check if already in symTab
				int lookupVal = lookupEntrySymTab ( symTabContents, yytext );
				if ( lookupVal == -1 ){
					sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount, yytext);
					sprintf(tokenName,"%s<CONSTANT,%d> ",tokenName,symTabCount);
					symTabCount++;
				}
				else
					sprintf(tokenName,"%s<CONSTANT,%d> ",tokenName,lookupVal);
			}
[+][+]		{strcat(tokenName,"<INC> ");}
[-][-]		{strcat(tokenName,"<DEC> ");}
{ID} 		{
				
				int lookupVal = lookupEntrySymTab ( symTabContents, yytext );
				if ( lookupVal == -1 ){
					sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount, yytext);
					sprintf(tokenName,"%s<ID,%d> ",tokenName,symTabCount);
					symTabCount++;
				}
				else
					sprintf(tokenName,"%s<ID,%d> ",tokenName,lookupVal);
			}
[/][/]({DIGIT}|{LETTER})* {strcat(tokenName,"<COMMENT> ");}
\n			{
				strcat(tokenName, "\n");
				lineCount++;
			}
[ ]			{};
\t			{};
.			{
				printf("Error on line number: %d", lineCount );
			};

%%
// Code section
int main()
{
	yylex();
	FILE *fp;
	fp = fopen("lexOutput","w");
	fprintf(fp,"%s",tokenName);
	fclose(fp);
	FILE *fpSymTab;
	fpSymTab = fopen("symTab","w");
	fprintf(fpSymTab,"%s",symTabContents);
	fclose(fpSymTab);
	return 0;
}
