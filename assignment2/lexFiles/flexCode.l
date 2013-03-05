%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	int symTabCount = 0, lineCount = 0;
	char tokenName[1000000] = "";
	char symTabContents[100000] ="";
	int lookupEntrySymTab ( char symTabContents[1000000], char symbol[255] )
	{
		int symTabIndex;
		int matchIndex = -1;		
		char *pch = strtok ( symTabContents, "\t\n" );
		while ( pch != NULL ){
			symTabIndex = (int)(*pch);
			pch = strtok ( NULL, "\t\n" );
			char token[256];
			strcpy ( token, pch );
			if ( strcmp ( token, symbol ) == 0 ){
				matchIndex = symTabIndex;
				printf("found%d\n",symTabIndex);
				break;
			}
			pch = strtok ( NULL, "\t\n" );
		}
		printf("retval%d",matchIndex);
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
					sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount++, yytext);
					sprintf(tokenName,"%s<CONSTANT,%d> ",tokenName,symTabCount);
				}
				else
					sprintf(tokenName,"%s<CONSTANT,%d> ",tokenName,lookupVal);
			}
[+][+]		{strcat(tokenName,"<INC> ");}
[-][-]		{strcat(tokenName,"<DEC> ");}
{ID} 		{
				int lookupVal = lookupEntrySymTab ( symTabContents, yytext );
				if ( lookupVal == -1 ){
					sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount++, yytext);
					sprintf(tokenName,"%s<ID,%d> ",tokenName,symTabCount);
				}
				else
					sprintf(tokenName,"%s<ID,%d> ",tokenName,lookupVal);
			}
[/][/]({DIGIT}|{LETTER})* {strcat(tokenName,"<COMMENT> ");}
\n			{strcat(tokenName, "\n");}


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
