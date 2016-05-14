#include <iostream>
#include <vector>
#include <stdexcept>
double result;
//---------------------------------STRUCT-PAIR----------------------------------------------
struct pair{
	char varLetter;
	double varValue;
};

pair variables[26];

//---------------------------------STORE-VARIABLE----------------------------------------------
void storeVariable(double x){
	char loopCheck='y';
		while(loopCheck=='y'){
			char ch='s';
			while(ch=='s' || ch=='p' || ch=='u' || ch=='h'){
				bool b=true;
				std::cout << "Elija caracter para variable: ";
				std::cin >> ch;
				if(ch!='s' && ch!='p' && ch!='u' && ch!='h')
					b=false;
				if(b)
					std::cout << "\aCaracter ya en uso por un commando!\n";
			}
			for (int i = 0; i < 26; ++i){
				if (ch==variables[i].varLetter){
					if (variables[i].varValue!=0){
						std::cout << "\aSobreescribir? y/n: ";
						char overwrtCheck;
						std::cin >> overwrtCheck;
						if(overwrtCheck=='y'){
							std::cout << "\"" << variables[i].varLetter << "\"" << " sobreescrita" << std::endl;
						}
						else{
							std::cout << "\"" << char(toupper(variables[i].varLetter)) << "\"\a" << " Guardar en otra variable? y/n: " << std::endl;
							std::cin >> loopCheck;
							break;
						}
					}
					variables[i].varValue=x;
					std::cout << x << "=>" << char(toupper(variables[i].varLetter)) << std::endl;
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
		throw std::runtime_error("Buffer esta lleno!");
	buffer=t;
	isFull=true;
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

		case 'e': case 'p': case 'h': case 'u':
		case '+': case '-': case '/': case '*': case '(': case ')': case '!':
			return Token{ch};
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': 
		case '.':{
			std::cin.putback(ch);
			double value;
			std::cin >> value;
			return Token{'#', value};
			break;
		}

		case 's':
			storeVariable(result);
			break;

		default:{
			for (int i = 0; i <= 26; ++i){
				if (ch==variables[i].varLetter)
					return Token{'#', variables[i].varValue};
			}
			throw std::runtime_error("Entrada Invalida!");
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
					throw std::runtime_error("Error: esperaba \")\" ");
				else
					return innerDouble;
				break;
			}
			case '#':{
				Token factorial = ts.get();
				if (factorial.kind=='!')
					return getFactorial(t.value);
				else
					ts.putback(factorial);
				return t.value;
				break;
			}
			default:
				throw std::runtime_error("Entrada no reconocida");
		}
	}
}
//---------------------------------HELP----------------------------------------------
inline void help(){
	std::cout << "Para usar:\nIntroduzca expresion, luego oprima enter.\np->imprimir resultado\ne->exit\ns->guardar variable\nh->aydua\nu->imprimir variables utilizadas\n";
}
//---------------------------------USED-VARIABLES----------------------------------------------
void usedVariables(){
	std::cout << "Variables utilizadas:\n";
	for (int i = 0; i < 26; ++i){
		if (variables[i].varValue!=0){
			std::cout << char(toupper(variables[i].varLetter)) << "\n";
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
//---------------------------------#-@-#--MAIN--#-@-#----------------------------------------------
int main(){
	fillArray();
	double value=0;
	std::cout << "Calculadora: oprima \'h\' para aydua\n";
	try{
		while(true){
			Token t = ts.get();
			switch(t.kind){
				case 'e':
					std::cout << "adios";
					return 0;
				case 'p':
					std::cout << "resultado: " << value << std::endl;
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
		}
	}
	catch(std::runtime_error &error1){
		std::cerr << error1.what() << std::endl;
		return -1;
	}
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
					throw std::runtime_error("No se puede dividir entre cero!");
				else
					left /= nonZeroInt;
					t = ts.get();
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