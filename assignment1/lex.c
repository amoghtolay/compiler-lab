#include "lex.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char* yytext = ""; /* Lexeme (not '\0'
                      terminated)              */
int yyleng   = 0;  /* Lexeme length.           */
int yylineno = 0;  /* Input line number        */

int lex(void){

   static char input_buffer[1024];
   char        *current;

   current = yytext + yyleng; /* Skip current
                                 lexeme        */

   while(1){       /* Get the next one         */
      while(!*current ){
         /* Get new lines, skipping any leading
         * white space on the line,
         * until a nonblank line is found.
         */

         current = input_buffer;
         if(!gets(input_buffer)){
            *current = '\0' ;

            return EOI;
         }
         ++yylineno;
         while(isspace(*current))
            ++current;
      }
      for(; *current; ++current){
         /* Get the next token */
         yytext = current;
         yyleng = 1;
         switch( *current ){
           case ';':
            return SEMI;
           case '+':
            return PLUS;
           case '-':
            return MINUS;
           case '*':
            return TIMES;
           case '/':
            return DIV;
           case '(':
            return LP;
           case ')':
            return RP;
           case '<':
			return LESS;
		   case '>':
			return MORE;
		   case '=':
			return RELEQUAL;
		   
           case '\n':
           case '\t':
           case ' ' :
            break;
           default:
            if(!isalnum(*current))
               fprintf(stderr, "Invalid Symbol found: <%c>\n", *current); // Changed the printf statement
            else{
               while(isalnum(*current))
                  ++current;
              
               yyleng = current - yytext;
               /*
				* Added for if-then, and do-while and begin-end 
				* statements
				*/
			   char *keyword;
			   keyword = (char*)( malloc ( sizeof(char)*yyleng ) );
			   strncpy( keyword, yytext, yyleng );
			   if ( !strcmp( keyword, "if" ) )
				return IF;
			   if ( !strcmp( keyword, "then" ) )
				return THEN;
			   if ( !strcmp( keyword, "do" ) )
				return DO;
			   if ( !strcmp( keyword, "while" ) )
				return WHILE;
			   if ( !strcmp( keyword, "begin" ) )
				return BEGIN;
			   if ( !strcmp( keyword, "end" ) )
				return END;
			  
			  return NUM_OR_ID;
            }
            break;
         }
      }
   }
}


static int Lookahead = -1; /* Lookahead token  */

int match(int token){
   /* Return true if "token" matches the
      current lookahead symbol.                */

   if(Lookahead == -1)
      Lookahead = lex();

   return token == Lookahead;
}

void advance(void){
/* Advance the lookahead to the next
   input symbol.                               */

    Lookahead = lex();
}
