#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
double result;
const char number='#';
const char prompt='>';
const char quit='e';
const char print='p';
//---------------------------------STRUCT-PAIR----------------------------------------------
struct pair{//in this program, structs are variables with a letter representation and value
	char varLetter;
	double varValue;
};

pair variables[26];

//---------------------------------STORE-VARIABLE----------------------------------------------
void storeVariable(double x){
	char loopCheck='y';
		while(loopCheck=='y'){
			char ch='s';
			bool b=true;
			while(b){
				std::cout << "Select variable character: ";
				std::cin >> ch;
				if(ch!='s' && ch!=print && ch!='u' && ch!='h' && ch!='e')
					b=false;
				else if(b)
					std::cout << "\aCharacter used by a command!\n";
			}
			for (pair& v : variables){
				if (ch==v.varLetter){
					if (v.varValue!=0){
						std::cout << "\aOverwrite? y/n: ";
						char overwrtCheck;
						std::cin >> overwrtCheck;
						if(overwrtCheck=='y'){
							std::cout << "\"" << v.varLetter << "\"" << " overwritten" << std::endl;
						}
						else{
							std::cout << "\"" << char(toupper(v.varLetter)) << "\"\a" << " Write to another another variable? y/n: " << std::endl;
							std::cin >> loopCheck;
							break;
						}
					}
					v.varValue=x;
					std::cout << x << "=>" << char(toupper(v.varLetter)) << std::endl;
					loopCheck='n';
				}
			}
		}
}
//---------------------------------GET-FACTORIAL----------------------------------------------
int getFactorial(int x){
	if (x==1 || x==0)
		return 1;
	else
		return x*getFactorial(x-1);
}

//---------------------------------TOKEN-CLASS----------------------------------------------
class Token{
	public:
		char kind;
		double value;
};

//---------------------------------TOKEN-STREAM----------------------------------------------
class Token_Stream{
	private:
		bool isFull=false;
		Token buffer;

	public:
		Token get();
		void putback(Token t);
		void ignore(char c);

};
//---------------------------------PRINT-TOKEN----------------------------------------------
void printToken(Token t){
	if(t.value)
		std::cout << "Token value: " << t.value << std::endl;
	else
		std::cout << "Token kind: " << t.kind << std::endl;
}
//---------------------------------PUTBACK----------------------------------------------
void Token_Stream::putback(Token t){
	//start error check, checks if buffer is full, if it is, throw error
	if (isFull)//since is full is a boolean, using if(isFull) is the same as if(isFull==true)
		throw std::runtime_error("Buffer is full!");
	buffer=t;
	isFull=true;
}
void Token_Stream::ignore(char c){//function to ignore errors until char c is found
	if (isFull && buffer.kind==c){//
		isFull==false;
		return;
	}
	isFull=false;
	char ch=0;
	while(std::cin>>ch){
		if (ch==c){
			return;
		}
	}
}
//---------------------------------GET----------------------------------------------
Token Token_Stream::get(){
	if (isFull){
		isFull=false;
		return buffer;
	}
	char ch;
	std::cin >> ch;
	ch=tolower(ch);
	switch(ch){
		case quit:
		case print:
		case 'h':
		case 'u':
		case '+':
		case '-':
		case '/':
		case '*':
		case '(':
		case ')':
		case '!':
		case '%':
			return Token{ch};
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': 
		case '.':{
			std::cin.putback(ch);
			double value;
			std::cin >> value;
			return Token{number, value};
			break;
		}

		case 's':
			storeVariable(result);
			break;

		default:{
			for (pair& v : variables){
				if (ch==v.varLetter)
					return Token{number, v.varValue};
			}
			throw std::runtime_error("Bad Token!");
		}
	}
}

Token_Stream ts;

double expression();

double term();

//---------------------------------PRIMARY----------------------------------------------
double primary(){
	Token t;
	t = ts.get();
	while(true){
		switch(t.kind){
			case '(':{//if you wish to initailize a variable within a switch case, you must put it in a block
				double innerDouble = expression();
				Token endParen = ts.get();
				if (endParen.kind!=')')
					throw std::runtime_error("Error: Expected \")\" ");
				else
					return innerDouble;
				break;
			}
			case number:{
				Token factorial = ts.get();
				if (factorial.kind=='!')
					return getFactorial(t.value);
				else
					ts.putback(factorial);
				return t.value;
				break;
			}
			case '-':
				return -primary();
			default:
				throw std::runtime_error("Unrecognized token kind");
		}
	}
}
//---------------------------------HELP----------------------------------------------
inline void help(){
	std::cout << "To use the calculator:\nInput an expression, then press enter.\np->print\ne->quit\ns->save variable after printing\nh->help\nu->print used variables\ng->generate random number\na->advanced mathematics help\n";
}
//---------------------------------USED-VARIABLES----------------------------------------------
void usedVariables(){
	std::cout << "Used variables:\n";
	for (pair& v : variables){
		if (v.varValue!=0){
			std::cout << char(toupper(v.varLetter)) << "\n";
		}
	}
}
//---------------------------------FILL-ARRAY----------------------------------------------
void fillArray(){	
	char ch ='a';
	int i =0;
	while(ch <= 'z'){
		pair myPair;
		myPair.varLetter=ch;
		myPair.varValue=0;
		variables[i]=myPair;
		ch++;
		i++;
	}
}
void calculate(){
	std::cout << "Calculator Program: press \'h\' for commands\n";
	double value=0;
	while(true){
		try{
			Token t = ts.get();
			switch(t.kind){
				case quit:
					std::cout << "goodbye";
					return;
				case print:
					std::cout << "= " << value << std::endl;
					result=value;
					break;
				case 'h':
					help();
					break;
				case 'u':
					usedVariables();
					break;
				default:
					ts.putback(t);
					value=expression();
			}
		}catch(std::runtime_error &myError){
			std::cerr << myError.what() << std::endl;
			ts.ignore(print);//ignore errors until print is read
		}
	}
}
//---------------------------------#-@-#--MAIN--#-@-#----------------------------------------------
int main(){
	fillArray();
	calculate();
}//end main

//---------------------------------TERM----------------------------------------------
double term(){
	double left = primary();
	Token t = ts.get();
	while(true){
		switch(t.kind){
			case '*':
				left *=primary();
				t = ts.get();
				break;
			case '/':{
				double nonZeroInt = primary();
				if (nonZeroInt==0)
					throw std::runtime_error("Cannot divide by zero!");
				else
					left /= nonZeroInt;
					t = ts.get();
					break;
			}
			case '%':{
				double nonZeroInt = primary();
				if (nonZeroInt==0){
					throw std::runtime_error("Cannot divide by zero!");
				}
				else
					left = fmod(left, nonZeroInt);
					t=ts.get();
					break;
			}
			default:
				ts.putback(t);
				return left;
		}
	}
}
//---------------------------------EXPRESSION----------------------------------------------
double expression(){
	double left = term();
	Token t = ts.get();
	while(true){
		switch(t.kind){
			case '+':
				left += term();
				t = ts.get();
				break;
			case '-':
				left -= term();
				t = ts.get();
				break;
			default:
				ts.putback(t);
				return left;
		}//end switch
	}//end while
}