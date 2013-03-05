%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	int symTabCount = 0, lineCount = 0;
	char tokenName[1000000] = "";
	char symTabContents[100000] ="";
%}
DIGIT [-+]?[0-9]
LETTER [A-Za-z]
ID [A-Za-z][A-Za-z0-9]*

%% 
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
				sprintf(tokenName,"%s<CONSTANT,%d> ",tokenName,symTabCount);
				sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount++, yytext);
			}
[+][+]		{strcat(tokenName,"<INC> ");}
[-][-]		{strcat(tokenName,"<DEC> ");}
{ID} 		{
				sprintf(tokenName,"%s<ID,%d> ",tokenName,symTabCount);
				sprintf(symTabContents,"%s%d\t%s\n",symTabContents,symTabCount++, yytext);
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
