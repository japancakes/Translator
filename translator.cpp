#include<iostream>
#include<fstream>
#include<string>
#include<map>
using namespace std;


// ----- Utility and Globals -----------------------------------

enum token_type {ERROR, WORD1, WORD2, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR, PERIOD, EOFM};
string token_name[30] = {"ERROR", "WORD1", "WORD2", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR", "PERIOD", "EOFM" }; 


token_type saved_token;
token_type next_token_val;
bool token_available;
string saved_lexeme;
string saved_E_word;

//function prototypes
bool syntaxerror1(token_type nextT, token_type expected);
void syntaxerror2(string, string);
token_type next_token();
void gen(string);
void getEword();
bool match(token_type expected);
bool word_token(string s);
bool period_token(string s);
int scanner(token_type& a, string& w);
void story();
void S();
void after_subject();
void after_noun();
void after_object();
void verb();
void noun();
void be();
void tense();

 
// ** Need syntaxerror1 and syntaxerror2 functions (each takes 2 args)


map<string, token_type> reservedWords = 
{
	{"masu", VERB},
	{"masen", VERBNEG},
	{"mashita", VERBPAST},
	{"masendeshita", VERBPASTNEG},
	{"desu", IS},
	{"deshita", WAS},
	{"o", OBJECT},
	{"wa", SUBJECT},
	{"ni", DESTINATION},
	{"watashi", PRONOUN},
	{"anata", PRONOUN},
	{"kare", PRONOUN},
	{"kanojo", PRONOUN},
	{"sore", PRONOUN},
	{"mata", CONNECTOR},
	{"soshite", CONNECTOR},
	{"shikashi", CONNECTOR},
	{"dakara", CONNECTOR},
	{"eofm", EOFM}
};

map<string, token_type>::iterator it;
ifstream fin; // global stream for reading from the input file
ofstream translated; 
string tword1, tword2;

// Declare dictionary that will hold the contents of lexicon.txt
map<string, string> translatorMap;
ifstream tin;

//getEword function
void getEword() {
	map<string, string>::iterator tMit;
	tMit = translatorMap.find(saved_lexeme);
	if (tMit != translatorMap.end()) {
		saved_E_word = translatorMap.find(saved_lexeme)->second;
	} else {
		saved_E_word = saved_lexeme;
	}
}

// gen func
void gen(string inp) {
	if (inp == "CONNECTOR") {
		translated << "CONNECTOR: " << saved_E_word << endl;
	} else
        if (inp == "SUBJECT") {
                translated << "ACTOR: " << saved_E_word << endl;
        } else
        if (inp == "BE") {
                translated << "DESCRIPTION: " << saved_E_word << endl;
		translated << "TENSE: " << token_name[saved_token] << endl << endl;
        } else
        if (inp == "OBJECT") {
                translated << "OBJECT: " << saved_E_word << endl;
        } else
        if (inp == "DESTINATION") {
                translated << "TO: " << saved_E_word << endl;
        } else
        if (inp == "VERB") {
                translated << "ACTION: " << saved_E_word << endl;
        } else
        if (inp == "TENSE") {
                translated << "TENSE: " << token_name[saved_token] << endl << endl;
        } else {
		translated << "ERROR IN FUNCTION CALL" << endl;
	}
}

//Syntax Error functions

bool syntaxerror1(token_type nextT, token_type expected) {
	cout << "SYNTAX ERROR: expected " << token_name[expected] << " but found " << saved_lexeme << endl;
	exit(1);
}

void syntaxerror2(string s_lex, string func_name) {
	cout << "SYNTAX ERROR: unexpected " << s_lex << " found in " << func_name << endl;
	exit(1);
}




// ** Need the updated match and next_token (with 2 global vars)

token_type next_token() {
	if(!token_available) {
		scanner(saved_token, saved_lexeme);
		token_available = true;
	}
	return saved_token;
}

bool match(token_type expected) {
	next_token_val = next_token();
	if(next_token_val != expected) {
		return syntaxerror1(next_token_val, expected);
	} else {
		cout << "Matched " << token_name[expected] << endl;
		token_available = false;
		return true;
	}

}

// ----- RDP functions - one per non-term -------------------

void story() {
	cout << "Processing <story>" << endl;
	S();
	while (true) {
		switch(next_token()) {
			case CONNECTOR: S(); break;
			case WORD1: S(); break;
			case PRONOUN: S(); break;
			case EOFM: return;
			default: syntaxerror2(saved_lexeme, "story"); return;
		}
	}
}

void S() {
	cout << "Processing <S>" << endl;
	if (next_token() == CONNECTOR) {
		match(CONNECTOR);
		getEword();
		gen("CONNECTOR");
	}
	noun();
	getEword();
	match(SUBJECT);
	gen("SUBJECT");
	after_subject();
}

void after_subject() {
	cout << "Processing <after_subject>" << endl;
	switch(next_token()) {
		case WORD2: verb(); getEword(); gen("VERB"); tense(); gen("TENSE"); match(PERIOD); break;
		case WORD1: noun(); getEword(); after_noun(); break;
		case PRONOUN: noun(); getEword(); after_noun(); break;
		default: syntaxerror2(saved_lexeme, "after_subject");
	}
	
}

void after_noun() {
	cout << "Processing <after_noun>" << endl;
	switch(next_token()) {
		case IS: be(); gen("BE"); match(PERIOD); break;
		case WAS: be(); gen("BE"); match(PERIOD); break;
		case DESTINATION: match(DESTINATION); gen("DESTINATION"); verb(); getEword(); gen("VERB"); tense(); gen("TENSE"); match(PERIOD); break;
		case OBJECT: match(OBJECT); gen("OBJECT"); after_object(); break;
		default: syntaxerror2(saved_lexeme, "after_noun");

	}
}

void after_object() {
	cout << "Processing <after_object>" << endl;
	switch(next_token()) {
		case WORD2: verb(); getEword(); gen("VERB"); tense(); gen("TENSE"); match(PERIOD); break;
		case WORD1: noun(); getEword(); match(DESTINATION); gen("DESTINATION"); verb(); getEword(); gen("VERB"); tense(); gen("TENSE"); match(PERIOD); break;
		case PRONOUN: noun(); getEword();  match(DESTINATION); gen("DESTINATION"); verb(); getEword(); gen("VERB"); tense(); gen("TENSE"); match(PERIOD); break;
		default: syntaxerror2(saved_lexeme, "after_object");
	}
}

void noun() {
	cout << "Processing <noun>" << endl;
	switch(next_token()) {
		case WORD1: match(WORD1); break;
		case PRONOUN: match(PRONOUN); break;
		default: syntaxerror2(saved_lexeme, "noun");
	}
}

void verb() {
	cout << "Processing <verb>" << endl;
	match(WORD2);
}

void be() {
	cout << "Processing <be>" << endl;
	switch(next_token()) {
                case IS: match(IS); break;
                case WAS: match(WAS); break;
                default: syntaxerror2(saved_lexeme, "be");
        }
}

void tense() {
	cout << "Processing <tense>" << endl;
	switch(next_token()) {
		case VERBPAST: match(VERBPAST); break;
		case VERBPASTNEG: match(VERBPASTNEG); break;
		case VERB: match(VERB); break;
		case VERBNEG: match(VERBNEG); break;
		default: syntaxerror2(saved_lexeme, "tense");
	}

}


//---------------------------------------

// The new test driver to start the parser
int main()
{
  tin.open("lexicon.txt");
  while(tin >> tword1 >> tword2) {
	translatorMap[tword1] = tword2;	
  }
  translated.open("translated.txt", ofstream::trunc);

  string filename;
  cout << "Enter the input file name: ";
  cin >> filename;
  fin.open(filename.c_str());

  //** calls the <story> to start parsing
  story();
  //** closes the input file 
  cout << "End of file is encountered" << endl;
  cout << "Sucesfully parsed <story>" << endl;
  tin.close();
  translated.close();
  fin.close();
}// end
//** require no other input files!
//** syntax error EC requires producing errors.text of messages


// Scanner Functions
bool word_token(string s)
{
  int state = 0;
  int charpos = 0;

  while (s[charpos] != '\0') 
    {
      if (state == 0 && s[charpos] == 'b')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'g')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'h')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'k')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'm')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'n')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'p')
      state = 1;
      else
      if (state == 0 && s[charpos] == 'r')
      state = 1;
      else
      if (state == 1 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 1 && s[charpos] == 'E')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'E')
      state = 2;
      else
      if (state == 0 && s[charpos] == 'd')
      state = 3;
      else
      if (state == 0 && s[charpos] == 'j')
      state = 3;
      else
      if (state == 0 && s[charpos] == 'w')
      state = 3;
      else
      if (state == 0 && s[charpos] == 'y')
      state = 3;
      else
      if (state == 0 && s[charpos] == 'z')
      state = 3;
      else
      if (state == 0 && s[charpos] == 's')
      state = 4;
      else
      if (state == 0 && s[charpos] == 't')
      state = 5;
      else
      if (state == 0 && s[charpos] == 'c')
      state = 6;
      else
      if (state == 2 && s[charpos] == 's')
      state = 4;
      else
      if (state == 2 && s[charpos] == 't')
      state = 5;
      else
      if (state == 2 && s[charpos] == 'c')
      state = 6;
      else
      if (state == 2 && s[charpos] == 'b')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'g')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'h')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'k')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'm')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'p')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'r')
      state = 1;
      else
      if (state == 2 && s[charpos] == 'n')
      state = 0;
      else
      if (state == 1 && s[charpos] == 'y')
      state = 3;
      else
      if (state == 2 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'E')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'h')
      state = 3;
      else
      if (state == 5 && s[charpos] == 's')
      state = 3;
      else
      if (state == 6 && s[charpos] == 'h')
      state = 3;
      else
      if (state == 3 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 3 && s[charpos] == 'E')
      state = 2;
      else
      if (state == 2 && s[charpos] == 'd')
      state = 3;
      else
      if (state == 2 && s[charpos] == 'j')
      state = 3;
      else
      if (state == 2 && s[charpos] == 'w')
      state = 3;
      else
      if (state == 2 && s[charpos] == 'y')
      state = 3;
      else
      if (state == 2 && s[charpos] == 'z')
      state = 3;
      else
      if (state == 4 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 4 && s[charpos] == 'E')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'a')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'e')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'i')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'o')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'u')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'I')
      state = 2;
      else
      if (state == 5 && s[charpos] == 'E')
      state = 2;
      else
	  return(false);
      charpos++;
    }//end of while

  // where did I end up????
  if (state == 2 || state == 0) return(true);  // end in a final state
   else return(false);
}

// ** Add the PERIOD DFA here
bool period_token(string s) {
  if (s == ".") {
	return true;
  }

  return false;
}

int scanner(token_type& a, string& w)
{

  // ** Grab the next word from the file via fin
  fin >> w; 
  cout << "Scanner called using word: " << w << endl; 
 /* 
  2. Call the token functions one after another (if-then-else)
     And generate a lexical error message if both DFAs failed.
     Let the token_type be ERROR in that case.
  3. Make sure WORDs are checked against the reservedwords list
     If not reserved, token_type is WORD1 or WORD2.
  4. Return the token type & string  (pass by reference)
  */
  if (word_token(w)) {
	it = reservedWords.find(w);
	if (it != reservedWords.end()) {
	  a = it->second;		
	} else if (w.back() == 'I' || w.back() == 'E') {
	  a = WORD2;
	} else {
	  a = WORD1;
	}
  } else if (period_token(w)) {
	a = PERIOD;
  } else {
	if (w != "eofm") {
	  cout << "Lexical Error: " << w << " is not a valid token" << endl;
	  a = ERROR;
	} else {
	  a = EOFM;
	}
  }

}//the end of scanner
