// File: Sally.cpp
//
//
// Implementation of member functions of Sally Forth interpreter
//

#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <stdexcept>
#include <cstdlib>
using namespace std ;

#include "Sally.h"


// Basic Token constructor. Just assigns values.
//
Token::Token(TokenKind kind, int val, string txt) {
   m_kind = kind ;
   m_value = val ;
   m_text = txt ;
}


// Basic SymTabEntry constructor. Just assigns values.
//
SymTabEntry::SymTabEntry(TokenKind kind, int val, operation_t fptr) {
   m_kind = kind ;
   m_value = val ;
   m_dothis = fptr ;
}


// Constructor for Sally Forth interpreter.
// Adds built-in functions to the symbol table.
//
Sally::Sally(istream& input_stream) :
   istrm(input_stream)  // use member initializer to bind reference
{

   symtab["DUMP"]    =  SymTabEntry(KEYWORD,0,&doDUMP) ;

   symtab["+"]    =  SymTabEntry(KEYWORD,0,&doPlus) ;
   symtab["-"]    =  SymTabEntry(KEYWORD,0,&doMinus) ;
   symtab["*"]    =  SymTabEntry(KEYWORD,0,&doTimes) ;
   symtab["/"]    =  SymTabEntry(KEYWORD,0,&doDivide) ;
   symtab["%"]    =  SymTabEntry(KEYWORD,0,&doMod) ;
   symtab["NEG"]  =  SymTabEntry(KEYWORD,0,&doNEG) ;

   symtab["."]    =  SymTabEntry(KEYWORD,0,&doDot) ;
   symtab["SP"]   =  SymTabEntry(KEYWORD,0,&doSP) ;
   symtab["CR"]   =  SymTabEntry(KEYWORD,0,&doCR) ;
   symtab["DUP"]  =  SymTabEntry(KEYWORD,0,&doDUP) ;
   symtab["DROP"] =  SymTabEntry(KEYWORD,0,&doDROP) ;
   symtab["SWAP"] =  SymTabEntry(KEYWORD,0,&doSWAP) ;
   symtab["ROT"]  =  SymTabEntry(KEYWORD,0,&doROT) ;
   symtab["SET"]  =  SymTabEntry(KEYWORD,0,&doSET) ;
   symtab["@"]  =  SymTabEntry(KEYWORD,0,&doAT) ;
   symtab["!"]  =  SymTabEntry(KEYWORD,0,&doSTORE) ;
   symtab["<"]  =  SymTabEntry(KEYWORD,0,&doLessThan) ;
   symtab["<="]  =  SymTabEntry(KEYWORD,0,&doLessEqTo) ;
   symtab["=="]  =  SymTabEntry(KEYWORD,0,&doEquiv) ;
   symtab["!="]  =  SymTabEntry(KEYWORD,0,&doNotEq) ;
   symtab[">="]  =  SymTabEntry(KEYWORD,0,&doGrtEqTo) ;
   symtab[">"]  =  SymTabEntry(KEYWORD,0,&doGrtThan) ;
   symtab["AND"]  =  SymTabEntry(KEYWORD,0,&doAND) ;
   symtab["OR"]  =  SymTabEntry(KEYWORD,0,&doOR) ;
   symtab["NOT"]  =  SymTabEntry(KEYWORD,0,&doNOT) ;
   symtab["IFTHEN"] = SymTabEntry(KEYWORD,0,&doIFTHEN) ;
   symtab["DO"]  =  SymTabEntry(KEYWORD,0,&DO) ;
   symtab["ELSE"]  =  SymTabEntry(KEYWORD,0) ;
   symtab["ENDIF"]  =  SymTabEntry(KEYWORD,0) ;



}


// This function should be called when tkBuffer is empty.
// It adds tokens to tkBuffer.
//
// This function returns when an empty line was entered
// or if the end-of-file has been reached.
//
// This function returns false when the end-of-file was encountered.
//
// Processing done by fillBuffer()
//   - detects and ignores comments.
//   - detects string literals and combines as 1 token
//   - detetcs base 10 numbers
//
//
bool Sally::fillBuffer() {
   string line ;     // single line of input
   int pos ;         // current position in the line
   int len ;         // # of char in current token
   long int n ;      // int value of token
   char *endPtr ;    // used with strtol()


   while(true) {    // keep reading until empty line read or eof

      // get one line from standard in
      //
      getline(istrm, line) ;

      // if "normal" empty line encountered, return to mainLoop
      //
      if ( line.empty() && !istrm.eof() ) {
         return true ;
      }

      // if eof encountered, return to mainLoop, but say no more
      // input available
      //
      if ( istrm.eof() )  {
         return false ;
      }


      // Process line read

      pos = 0 ;                      // start from the beginning

      // skip over initial spaces & tabs
      //
      while( line[pos] != '\0' && (line[pos] == ' ' || line[pos] == '\t') ) {
         pos++ ;
      }

      // Keep going until end of line
      //
      while (line[pos] != '\0') {

         // is it a comment?? skip rest of line.
         //
         if (line[pos] == '/' && line[pos+1] == '/') break ;

         // is it a string literal?
         //
         if (line[pos] == '.' && line[pos+1] == '"') {

            pos += 2 ;  // skip over the ."
            len = 0 ;   // track length of literal

            // look for matching quote or end of line
            //
            while(line[pos+len] != '\0' && line[pos+len] != '"') {
               len++ ;
            }

            // make new string with characters from
            // line[pos] to line[pos+len-1]
            string literal(line,pos,len) ;  // copy from pos for len chars

            // Add to token list
            //
            tkBuffer.push_back( Token(STRING,0,literal) ) ;

            // Different update if end reached or " found
            //
            if (line[pos+len] == '\0') {
               pos = pos + len ;
            } else {
               pos = pos + len + 1 ;
            }

         } else {  // otherwise "normal" token

            len = 0 ;  // track length of token

            // line[pos] should be an non-white space character
            // look for end of line or space or tab
            //
            while(line[pos+len] != '\0' && line[pos+len] != ' ' && line[pos+len\
] != '\t') {
               len++ ;
            }

            string literal(line,pos,len) ;   // copy form pos for len chars
            pos = pos + len ;

            // Try to convert to a number
            //
            n = strtol(literal.c_str(), &endPtr, 10) ;

            if (*endPtr == '\0') {
               tkBuffer.push_back( Token(INTEGER,n,literal) ) ;
            } else {
               tkBuffer.push_back( Token(UNKNOWN,0,literal) ) ;
            }
         }

         // skip over trailing spaces & tabs
         //
         while( line[pos] != '\0' && (line[pos] == ' ' || line[pos] == '\t') ) \
{
            pos++ ;
         }

      }
   }
}



// Return next token from tkBuffer.
// Call fillBuffer() if needed.
// Checks for end-of-file and throws exception
//
Token Sally::nextToken() {
      Token tk ;
      bool more = true ;

      while(more && tkBuffer.empty() ) {
         more = fillBuffer() ;
      }

      if ( !more && tkBuffer.empty() ) {
         throw EOProgram("End of Program") ;
      }

      tk = tkBuffer.front() ;
      tkBuffer.pop_front() ;
      return tk ;
}
// The main interpreter loop of the Sally Forth interpreter.
// It gets a token and either push the token onto the parameter
// stack or looks for it in the symbol table.
//
//
void Sally::mainLoop() {

   Token tk ;
   map<string,SymTabEntry>::iterator it ;

   try {
      while( 1 ) {
         tk = nextToken() ;

         if (tk.m_kind == INTEGER || tk.m_kind == STRING) {

            // if INTEGER or STRING just push onto stack
            params.push(tk) ;

         } else {
            it = symtab.find(tk.m_text) ;

            if ( it == symtab.end() )  {   // not in symtab

               params.push(tk) ;

            } else if (it->second.m_kind == KEYWORD)  {

               // invoke the function for this operation
               //

               it->second.m_dothis(this) ;

            } else if (it->second.m_kind == VARIABLE) {

               // variables are pushed as tokens
               //
               tk.m_kind = VARIABLE ;
               params.push(tk) ;

            } else {

               // default action
               //
               params.push(tk) ;

            }
         }
      }

   } catch (EOProgram& e) {

      cerr << "End of Program\n" ;
      if ( params.size() == 0 ) {
         cerr << "Parameter stack empty.\n" ;
      } else {
         cerr << "Parameter stack has " << params.size() << " token(s).\n" ;
      }

   } catch (out_of_range& e) {

      cerr << "Parameter stack underflow??\n" ;

   } catch (...) {

      cerr << "Unexpected exception caught\n" ;

   }
}


void Sally::doPlus(Sally *Sptr) {
   Token p1, p2 ;

   if ( Sptr->params.size() < 2 ) {
      throw out_of_range("Need two parameters for +.") ;
   }
   p1 = Sptr->params.top() ;
   Sptr->params.pop() ;
   p2 = Sptr->params.top() ;
   Sptr->params.pop() ;
   int answer = p2.m_value + p1.m_value ;
   Sptr->params.push( Token(INTEGER, answer, "") ) ;
}


void Sally::doMinus(Sally *Sptr) {
   Token p1, p2 ;

   if ( Sptr->params.size() < 2 ) {
      throw out_of_range("Need two parameters for -.") ;
   }
   p1 = Sptr->params.top() ;
   Sptr->params.pop() ;
   p2 = Sptr->params.top() ;
   Sptr->params.pop() ;
   int answer = p2.m_value - p1.m_value ;
   Sptr->params.push( Token(INTEGER, answer, "") ) ;
}


void Sally::doTimes(Sally *Sptr) {
   Token p1, p2 ;

   if ( Sptr->params.size() < 2 ) {
      throw out_of_range("Need two parameters for *.") ;
   }
   p1 = Sptr->params.top() ;
   Sptr->params.pop() ;
   p2 = Sptr->params.top() ;
   Sptr->params.pop() ;
   int answer = p2.m_value * p1.m_value ;
   Sptr->params.push( Token(INTEGER, answer, "") ) ;
}

void Sally::doDivide(Sally *Sptr) {
   Token p1, p2 ;

   if ( Sptr->params.size() < 2 ) {
      throw out_of_range("Need two parameters for /.") ;
   }
   p1 = Sptr->params.top() ;
   Sptr->params.pop() ;
   p2 = Sptr->params.top() ;
   Sptr->params.pop() ;
   int answer = p2.m_value / p1.m_value ;
   Sptr->params.push( Token(INTEGER, answer, "") ) ;
}


void Sally::doMod(Sally *Sptr) {
   Token p1, p2 ;

   if ( Sptr->params.size() < 2 ) {
      throw out_of_range("Need two parameters for %.") ;
   }
   p1 = Sptr->params.top() ;
   Sptr->params.pop() ;
   p2 = Sptr->params.top() ;
   Sptr->params.pop() ;
   int answer = p2.m_value % p1.m_value ;
   Sptr->params.push( Token(INTEGER, answer, "") ) ;
}


void Sally::doNEG(Sally *Sptr) {
   Token p ;

   if ( Sptr->params.size() < 1 ) {
      throw out_of_range("Need one parameter for NEG.") ;
   }
   p = Sptr->params.top() ;
   Sptr->params.pop() ;
   Sptr->params.push( Token(INTEGER, -p.m_value, "") ) ;
}


void Sally::doDot(Sally *Sptr) {

   Token p ;
   if ( Sptr->params.size() < 1 ) {
      throw out_of_range("Need one parameter for .") ;
   }

   p = Sptr->params.top() ;
   Sptr->params.pop() ;

   if (p.m_kind == INTEGER) {
      cout << p.m_value ;
   } else {
      cout << p.m_text ;
   }
}

void Sally::doSP(Sally *Sptr) {
   cout << " " ;
}


void Sally::doCR(Sally *Sptr) {
   cout << endl ;
}

void Sally::doDUMP(Sally *Sptr) {
   // do whatever for debugging
}

void Sally::doDUP(Sally *Sptr) {
  Token p;

  if ( Sptr->params.size() < 1 ){
    throw out_of_range("Need one parameter for DUP");
  }
  p = Sptr->params.top();
  Sptr->params.push( Token(INTEGER, p.m_value, "") ) ;
}

void Sally::doDROP(Sally *Sptr) {
  if ( Sptr->params.size() < 1 ){
     throw out_of_range("Need one parameter for DROP");
  }
  Sptr->params.pop();
}

void Sally::doSWAP(Sally *Sptr) {
  Token p1;
  Token p2;
  Token temp;

  if ( Sptr->params.size() < 2 ){
     throw out_of_range("Need two parameters for SWAP");
  }
  p1 = Sptr->params.top();
  Sptr->params.pop();
  p2 = Sptr->params.top();
  Sptr->params.pop();

  Sptr->params.push( Token(INTEGER, p1.m_value, "") ) ;
  Sptr->params.push( Token(INTEGER, p2.m_value, "") ) ;

}

void Sally::doROT(Sally *Sptr){
  Token p1;
  Token p2;
  Token p3;

 if ( Sptr->params.size() < 3 ){
     throw out_of_range("Need three parameters for ROT");
 }
 p1 = Sptr->params.top();
 Sptr->params.pop();
 p2 = Sptr->params.top();
 Sptr->params.pop();
 p3 = Sptr->params.top();
 Sptr->params.pop();

 Sptr->params.push( Token(INTEGER, p2.m_value, "") ) ;
 Sptr->params.push( Token(INTEGER, p1.m_value, "") ) ;
 Sptr->params.push( Token(INTEGER, p3.m_value, "") ) ;
}

void Sally::doSET(Sally *Sptr){
  Token var;
  Token val;

  if ( Sptr->params.size() < 2 ){
     throw out_of_range("Need two parameters for SET");
  }
  var = Sptr->params.top();
  Sptr->params.pop();
  val = Sptr->params.top();
  Sptr->params.pop();


  map<string, SymTabEntry>::iterator it;

  it = Sptr->symtab.find(var.m_text);
  //if the element does not exist within the map, set the variables value
  if(it == Sptr->symtab.end()){
    Sptr->symtab[var.m_text] = SymTabEntry(VARIABLE, val.m_value);
  }
  else
    throw ("Error! Variable already set to a value.");
}

void Sally::doAT(Sally *Sptr){

  Token var;
  var = Sptr->params.top();
  Sptr->params.pop();

  map<string, SymTabEntry>::iterator it;

  it = Sptr->symtab.find(var.m_text);
  //if the variable exists, push its value onto the stack
  if(it != Sptr->symtab.end()){
    Sptr->params.push(Token (INTEGER, Sptr->symtab[var.m_text].m_value  , ""));
  }

  else
    throw ("Error! Variable does not exist.");
}
void Sally::doSTORE(Sally *Sptr){

  Token var;
  Token val;

  var = Sptr->params.top();
  Sptr->params.pop();
  val = Sptr->params.top();
  Sptr->params.pop();

  map<string, SymTabEntry>::iterator it;

  it = Sptr->symtab.find(var.m_text);
  //if the variable exists, store the value into the variable
  if(it != Sptr->symtab.end()){
    Sptr->symtab[var.m_text] = SymTabEntry(INTEGER, val.m_value);
  }
}

void Sally::doGrtThan(Sally *Sptr){

  if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for >");
  }
  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is greater than p2, return true. Else, false
  if(p1.m_value > p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doGrtEqTo(Sally *Sptr){

   if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for >");
  }
  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is greater than or equal to p2, return true. Else, false
  if(p1.m_value >= p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doEquiv(Sally *Sptr){
 if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for ==");
  }



  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is equivalent to p2, return true. Else, false
  if(p1.m_value == p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doNotEq(Sally *Sptr){
 if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for !=");
  }
  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is not equivalent to p2, return true. Else, false
  if(p1.m_value != p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doLessEqTo(Sally *Sptr){
 if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for <=");
  }
  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is less than or equal to p2, return true. Else, false
  if(p1.m_value <= p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doLessThan(Sally *Sptr){
 if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for <");
  }
  Token p1;
  Token p2;

  p2 = Sptr->params.top();
  Sptr->params.pop();
  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is less than p2, return true. Else, false
  if(p1.m_value < p2.m_value)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doAND(Sally *Sptr){
 if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for AND");
  }
  Token p1;
  Token p2;

  p1 = Sptr->params.top();
  Sptr->params.pop();
  p2 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 and p2 are both equivalent to true
  if(p1.m_value == 1 and p2.m_value == 1)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));

}

void Sally::doOR(Sally *Sptr){
   if ( Sptr->params.size() < 2 ){
    throw out_of_range("Need two parameters for OR");
  }
  Token p1;
  Token p2;

  p1 = Sptr->params.top();
  Sptr->params.pop();
  p2 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 or p2 are equivalent to true
  if(p1.m_value == 1 or p2.m_value == 1)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));
}

void Sally::doNOT(Sally *Sptr){
 if ( Sptr->params.size() < 1 ){
    throw out_of_range("Need one parameter for NOT");
  }
  Token p1;

  p1 = Sptr->params.top();
  Sptr->params.pop();

  //if p1 is equivalent to 0, push true onto params
  if(p1.m_value == 0)
    Sptr->params.push(Token (INTEGER, 1, ""));
  else
    Sptr->params.push(Token (INTEGER, 0, ""));


}

void Sally::doIFTHEN(Sally *Sptr){
  Token tk;
  Token second;
  int val;

  if ( Sptr->params.size() < 1 )
    throw out_of_range("Need one parameter for IFTHEN");

  //if the previous expression evaluated to true, resume
  val = Sptr->params.top().m_value;
  Sptr->params.pop();
  //if the expression evaluates to true, enter the IFTHEN stmnt
  if(val == 1){
    RunToKeyword(Sptr, "ELSE");
    // run code until corresponding else
    // skip over until endif
    SkipToTK(Sptr, "ENDIF", "ELSE");
  }
  //if expression evaluates to false, skip to ELSE stmnt
  else{
    SkipToTK(Sptr, "ELSE", "IFTHEN");
    RunToKeyword(Sptr, "ENDIF");
    // skip over until corresponding else
    // continue until endif
  }
}


void Sally::RunToKeyword(Sally* Sptr, string match){
  Token tk;
  map<string,SymTabEntry>::iterator it ;

  //perform all of the stack operations up until the keyword is found
  while(1){
    tk = Sptr->nextToken();
    if (tk.m_kind == INTEGER || tk.m_kind == STRING) {
      // if INTEGER or STRING just push onto stack
      Sptr->params.push(tk);
    } else {
      it = Sptr->symtab.find(tk.m_text);

      if ( it == Sptr->symtab.end() )  {   // not in symtab
        Sptr->params.push(tk);

      } else if (it->second.m_kind == KEYWORD)  {

        //if we encounter a matching keyword, break out of while loop
        if(tk.m_text == match){
          return;
        }
          // invoke the function for this operation
        it->second.m_dothis(Sptr);

      } else if (it->second.m_kind == VARIABLE) {
        // variables are pushed as tokens
        tk.m_kind = VARIABLE;
        Sptr->params.push(tk);

      } else {
        // default action
        Sptr->params.push(tk);
      }
    }
  }
}

void Sally::SkipToTK(Sally *Sptr, string match, string opp = NULL){
  Token tk;
  int count = 0;
  while(1){
    tk = Sptr->nextToken();
    //once it hits the matching keyword we want to skip to, return
    if (tk.m_text == match){
      if (count > 0){
        --count;
      }
      else
        return;
    }
    //increment for every opposite keyword we find.
    //this accounts for matching the correct IFTHEN with its ELSE
    else if (tk.m_text == opp){
      ++count;
    }
  }
}

void Sally::DO(Sally* Sptr){
  Token tkArr[10000];
  Token tk;
  Token toInc;
  int index = 0;
  int count = 0;
  map<string,SymTabEntry>::iterator it ;

  while(1){
    tk = Sptr->nextToken();

    //populate the token array from the stack once it hits the UNTIL, check if
    //DO has been encountered, then break
    if (tk.m_text == "UNTIL"){
      if (count > 0){
        --count;
      }
      else
        break;
    }
    else if (tk.m_text == "DO"){
      ++count;
    }
    tkArr[index] = tk;
    index++;
  }
  //perform all of the stack operations like normal
  while(1){

    for(int i = 0; i < index; i++){

      tk = tkArr[i];

      if (tk.m_kind == INTEGER || tk.m_kind == STRING) {
        // if INTEGER or STRING just push onto stack
        Sptr->params.push(tk);
      } else {
        it = Sptr->symtab.find(tk.m_text);

        if ( it == Sptr->symtab.end() )  {   // not in symtab
          Sptr->params.push(tk);

        } else if (it->second.m_kind == KEYWORD){
          // invoke the function for this operation
          it->second.m_dothis(Sptr);

        } else if (it->second.m_kind == VARIABLE) {
          // variables are pushed as tokens
          tk.m_kind = VARIABLE;
          Sptr->params.push(tk);

        } else {
          // default action
          Sptr->params.push(tk);
        }
      }
    }
    //check the expression evaluation, if the condition is met, break.
    Token var = Sptr->params.top();
    Sptr->params.pop();
    if(var.m_value == 1){
      break;
    }
  }
}

