#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

string langName = "LARTOL";
string asm_code = "";
int str_count = 0;

struct Token{
    string str_value = "";
    double num_value = 0;
    bool bool_value = false;
    string type = "???";
};

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    if(token.type == "number"){
        os << "- {type: " << token.type 
        << ", num_value: " << token.num_value
        << "}";
    }
    else if(token.type == "bool"){
        os << "- {type: " << token.type 
        << ", bool_value: " << (token.bool_value ? "true" : "false")
        << "}";
    }
    else if(token.type == "line_num"){
        os << "\033[0;33m{type: " << token.type 
        << ", num_value: " << token.num_value
        << "}\033[0m";
    }
    else{
        os << "- {type: " << token.type 
        << ", str_value: " << token.str_value
        << "}";
    }
    return os;
}

//Char to string conversion
string ctos(char x){ 
    string s(1, x);
    return s;    
}

//Removes all trailing and ending whitespace from a string
void trim(string & line){
    if(line.size() == 0) return;
    if(line.size() == 1 && line[0] != ' ' && line[0] != '\t') return;

    int first = 0;
    int last = 0;
    //Left trim
    for(int i = 0; i < line.size(); ++i){
        if (line[i] != ' ' && line[i] != '\t'){
            first = i;
            break;
        }
    }
    //Right trim
    for(int i = line.size()-1; i > 0; --i){
        if (line[i] != ' ' && line[i] != '\t'){
            last = i+1;
            break;
        }
    }
    line = line.substr(first, last-first);
}

//Removes all comments from a single line
//TODO: This won't work inside a string. Fix.
void uncomment (string & line){
    if(line.size() == 0) return;
    for(int i = 0; i < line.size(); ++i){
        if(line[i] == ';'){
            line = line.substr(0, i);
            break;
        }
    }
}

vector<pair<int, string> > stylize(vector<string> & lines){
    vector<pair<int, string> > new_lines;
    
    //Remove comments
    for(int i = 0; i < lines.size(); ++i){
        //Remove comments
        uncomment(lines[i]);
        //Trim each line
        trim(lines[i]);
        //Remove empty lines
        if(lines[i].size() == 0){
            continue;
        }
        //Add line number to line and EOL
        new_lines.push_back(make_pair(i+1, + " " + lines[i]));
    }
    
    return new_lines;
}

//Takes an array of source lines and splits them into pre-tokens
//(that is strings that will be token str_values)
vector<string> pretokenize(const vector<pair<int, string> > & lines){
    vector<string> tokens;
    string splitters = " ";
    for(int i = 0; i < lines.size(); ++i){
        //Get line number
        tokens.push_back("#ln" + to_string(lines[i].first));
        //Get line
        string line = lines[i].second;
        //Set variables for parsing
        string current_token = "";
        char string_delimiter = 0;
        for(int j = 0; j < line.size(); ++j){
            //If the current character is a string delimiter
            if(string_delimiter == 0 && (line[j] == '"' || line[j] == '\'')){
                //Set current string delimiter
                string_delimiter = line[j];
                //Push current token and clean it
                if(current_token != "")
                    tokens.push_back(current_token);
                tokens.push_back("#string");
                current_token = "";
            }
            //If the current character matches the current string delimiter
            else if(string_delimiter != 0 && line[j] == string_delimiter && line[j-1] != '\\'){
                //Push current token and clean it
                tokens.push_back(string_delimiter + current_token + string_delimiter);
                //Clear string delimiter
                string_delimiter = 0;
                current_token = "";
            }
            //If the current character is an escape character
            else if(line[j] == '\\'){
                //If it was escaped
                if(j > 0 && line[j-1] == '\\'){
                    //If it was escaped within a string
                    if(string_delimiter != 0){
                        //Add to string
                        current_token += "\\";
                    }
                    //If it was escaped outside a string
                    else{
                        cout << "TODO: Complete this error" << endl;
                        exit(1);
                    }
                }
            }
            //If the current character is reserved character
            else if(line[j] == ';'){
                //If it was used within a string
                if(string_delimiter != 0){
                    //Add to string
                    current_token += ";";
                }
                //If it was escaped outside a string
                else{
                    cout << "TODO: Complete this error" << endl;
                    exit(1);
                }
            }
            //If the current character is a splitter
            else if(string_delimiter == 0 && splitters.find(line[j]) != -1){
                //Push current token and clean it
                if(current_token != ""){
                    tokens.push_back(current_token);
                    current_token = "";
                }
                //Push the spliter if it's not whitespace
                if(line[j] != ' ' && line[j] != '\t'){
                    tokens.push_back(ctos(line[j]));
                }
            }
            //If the current character is not a splitter
            else{
                current_token += line[j];
            }
        }
        //Push last token if it wasn't pushed
        if(current_token != "")
            tokens.push_back(current_token);
    }
    //Return our vector of tokens
    return tokens;
}

bool is_number (string number) {
  try {
    stod(number);
  }
  catch (const std::invalid_argument& ia) {
	  return false;
  }
  return true;
}

//Takes a list of pre-tokens, gives them str_value and returns a list of
//tokens.
vector<Token> lextokens(const vector<string> & pretokens){
    vector<Token> tokens;
    bool dataSection = false;
    bool procedureSection = false;
    int currentLine = 0;
    
    for(int i = 0; i < pretokens.size(); ++i){
        //Get pretoken and create a token
        Token token;
        string pretoken = pretokens[i];
        
        // === ANY SECTION ===
        if (pretoken.size() > 3 && pretoken.substr(0, 3) == "#ln"){
            //Line number
            token.num_value = stod(pretoken.substr(3));
            currentLine = token.num_value;
            token.type = "line_num";
            tokens.push_back(token);
        }
        
        // === NO SECTION ===
        else if(!dataSection && !procedureSection){
            if(pretoken == "DATA:"){ //Jump to DATA
                dataSection = true;
                token.str_value = pretoken;
                token.type = "data_section";
                tokens.push_back(token);
            }
            else if(pretoken == "PROCEDURE:"){ //Jump to PROCEDURE
                dataSection = true;
                procedureSection = true;
                token.str_value = pretoken;
                token.type = "proc_section";
                tokens.push_back(token);
            }
            else{
                cout << "\033[1;31mError: SECTION expected on line \033[1;37m" << currentLine << "\033[1;31m.\033[0m" << endl;
                exit(1);
            }
        }

        // === DATA SECTION ===
        else if(dataSection && !procedureSection){
            if(pretoken == "PROCEDURE:"){ //Jump to PROCEDURE
                dataSection = true;
                procedureSection = true;
                token.str_value = pretoken;
                token.type = "proc_section";
                tokens.push_back(token);
            }
            else if (pretoken.size() > 1 && pretoken[0] == '$'){
                token.str_value = pretoken;
                token.type = "var_declaration";
                tokens.push_back(token);
            }
            else if (pretoken == "IS"){
                token.str_value = pretoken;
                token.type = "token";
                tokens.push_back(token);
            }
            else if (pretoken == "NUMBER"){
                token.str_value = pretoken;
                token.type = "var_type";
                tokens.push_back(token);
            }
            else if (pretoken == "TEXT"){
                token.str_value = pretoken;
                token.type = "var_type";
                tokens.push_back(token);
            }
            else if (is_number(pretoken)){
                token.num_value = stod(pretoken);
                token.type = "number";
                tokens.push_back(token);
            }
            else{
                cout << "\033[1;31mError: unexpected \033[1;37m" << pretoken << "\033[1;31m in DATA SECTION on line \033[1;37m" << currentLine << "\033[1;31m.\033[0m" << endl;
                exit(1);
            }
        }
        
        // === PROCEDURE SECTION ===
        else if(procedureSection && dataSection){
            if (pretoken == "DISPLAY" || pretoken == "ACCEPT" || pretoken == "JOIN" || pretoken == "STORE" || pretoken == "END"){
                token.str_value = pretoken;
                token.type = "statement";
                tokens.push_back(token);
            }
            else if (pretoken == "IF"){
                token.str_value = pretoken;
                token.type = "if";
                tokens.push_back(token);
            }
            else if (pretoken == "THEN"){
                token.str_value = pretoken;
                token.type = "then";
                tokens.push_back(token);
            }
            else if (pretoken == "ELSE"){
                token.str_value = pretoken;
                token.type = "else";
                tokens.push_back(token);
            }
            else if (pretoken == "END-IF"){
                token.str_value = pretoken;
                token.type = "endif";
                tokens.push_back(token);
            }
            else if (pretoken == "WHILE"){
                token.str_value = pretoken;
                token.type = "while";
                tokens.push_back(token);
            }
            else if (pretoken == "DO"){
                token.str_value = pretoken;
                token.type = "do";
                tokens.push_back(token);
            }
            else if (pretoken == "REPEAT"){
                token.str_value = pretoken;
                token.type = "wend";
                tokens.push_back(token);
            }
            else if(pretoken == "#string"){
                string stringName = "__str" + to_string(++str_count);
                asm_code += stringName + ": " + pretokens[++i] + ", 0\n";
                token.str_value = stringName;
                token.type = "string";
                tokens.push_back(token);
            }
            else if (pretoken.size() > 1 && pretoken[0] == '$'){
                token.str_value = pretoken;
                token.type = "var";
                tokens.push_back(token);
            }
            else if (pretoken == "AND" || pretoken == "INTO" || pretoken == "BY" || pretoken == "IS" || pretoken == "TO"){
                token.str_value = pretoken;
                token.type = "token";
                tokens.push_back(token);
            }
            else if (is_number(pretoken)){
                token.num_value = stod(pretoken);
                token.type = "number";
                tokens.push_back(token);
            }
            else if (pretoken == "PLUS" || pretoken == "MINUS" || pretoken == "TIMES" || pretoken == "DIVIDED" || pretoken == "MODULO"){
                token.str_value = pretoken;
                token.type = "math-op";
                tokens.push_back(token);
            }
            else if (pretoken == "POSITIVE" || pretoken == "NEGATIVE"){
                token.str_value = pretoken;
                token.type = "math-condition";
                tokens.push_back(token);
            }
            else{
                cout << "\033[1;31mError: unexpected \033[1;37m" << pretoken << "\033[1;31m in PROCEDURE SECTION on line \033[1;37m" << currentLine << "\033[1;31m.\033[0m" << endl;
                exit(1);
            }
        }
    }
    
    for(int i = 0; i < tokens.size(); ++i){
        cout << tokens[i] << endl;
    }
    
    return tokens;
}

void unexpectedError(int lineNumber, Token token){
    if(token.type != "number"){
        cout << "\033[1;31mError: unexpected \033[1;37m" << token.str_value << "\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    }else{
        cout << "\033[1;31mError: unexpected \033[1;37m" << token.num_value << "\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    }
    exit(1);
}

void expectedError(int lineNumber, string whatWasExpected){
    cout << "\033[1;31mError: expected \033[1;37m" << whatWasExpected << "\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    exit(1);
}

bool grammar_check(vector<Token> & tokens){
    int lineNumber = 0;
    bool dataSection = false;
    
    cout << endl << " --- Grammar check --- " << endl << endl;
    
    for(int i = 0; i < tokens.size(); ++i)
    {
        //Get line number
        if(tokens[i].type == "line_num")
        {
            lineNumber = tokens[i].num_value;
            cout << "Grammar checking line " << lineNumber << endl;
            continue;
        }
        //Get section type
        if(tokens[i].type == "data_section")
        {
            lineNumber = tokens[i].num_value;
            cout << " - DATA section started." << endl;
            dataSection = true;
            continue;
        }
        else if(tokens[i].type == "proc_section")
        {
            lineNumber = tokens[i].num_value;
            cout << " - PROCEDURE section started." << endl;
            dataSection = false;
            continue;
        }
        
        // === DATA SECTION ===
        else if(dataSection){
            // --- Variable declaration ---
            if(tokens[i].type == "var_declaration"){
                //Get variable name
                string var_name = tokens[i].str_value.substr(1);
                cout << " - Variable declaration: " << var_name << endl;
                //Check if IS has been written
                if(++i < tokens.size() && tokens[i].type == "token" && tokens[i].str_value == "IS"){
                    //Check type: NUMBER
                    if(++i < tokens.size() && tokens[i].type == "var_type" && tokens[i].str_value == "NUMBER"){
                        tokens[i-2].type = "num_var";
                        cout << " - Numeric variable." << endl;
                        asm_code += "__var" + var_name + ": dq 0\n";
                    }
                    //Check type: TEXT
                    else if(tokens[i].type == "var_type" && tokens[i].str_value == "TEXT"){
                        cout << " - Text variable." << endl;
                        tokens[i-2].type = "text_var";
                        if(++i < tokens.size() && tokens[i].type == "number"){
                            int length = tokens[i].num_value;
                            cout << " - Length: " << length << endl;
                            asm_code += "__var" + var_name + ": rb " + to_string(length) + "\n";
                        } else expectedError(lineNumber, "text length numeral");
                    } else unexpectedError(lineNumber, tokens[i]);
                
                } else unexpectedError(lineNumber, tokens[i]);
            }else unexpectedError(lineNumber, tokens[i]);
        }
        
        // === PROCEDURE SECTION ===
        else if(!dataSection){
            //If statement
            if(tokens[i].type == "statement"){
                cout << " - Statement line. Statement: " << tokens[i].str_value << endl;
                // --- DISPLAY statement ---
                if(tokens[i].str_value == "DISPLAY"){
                    if(++i < tokens.size() && tokens[i].type == "string"){
                        cout << " - Will print string: " << tokens[i].str_value << endl;
                    }
                    else if(++i < tokens.size() && tokens[i].type == "number"){
                        cout << " - Will print number: " << tokens[i].num_value << endl;
                    }
                    else if(++i < tokens.size() && tokens[i].type == "text_var"){
                        cout << " - Will print text variable: " << tokens[i].str_value << endl;
                    }
                    else if(++i < tokens.size() && tokens[i].type == "num_var"){
                        cout << " - Will print number variable: " << tokens[i].str_value << endl;
                    }
                    else expectedError(lineNumber, "text");
                    cout << " - ASM TBD" << endl;
                }
                // --- ACCEPT statement ---
                if(tokens[i].str_value == "ACCEPT"){
                    if(++i < tokens.size() && tokens[i].type == "var"){
                        cout << " - Will accept: " << tokens[i].str_value << endl;
                    }
                    else  expectedError(lineNumber, "variable identifier");
                    cout << " - ASM TBD" << endl;
                }
                // --- JOIN statement ---
                if(tokens[i].str_value == "JOIN"){
                    //TODO QUEDÉ ACÁ!
                    cout << " - ASM TBD" << endl;
                }
            }
            else if(tokens[i].type == "if"){
            }
            else if(tokens[i].type == "else"){
            }
            else if(tokens[i].type == "endif"){
            }
            else if(tokens[i].type == "while"){
            }
            else if(tokens[i].type == "wend"){
            }
            else unexpectedError(lineNumber, tokens[i]);
        }
        //Malformed line
        else unexpectedError(lineNumber, tokens[i]);
    }
    
    cout << endl << " --- ASM CODE --- " << endl << endl;
    cout << asm_code << endl;
}

int main (int argc, char** argv){
    //Get command line arguments
    vector<string> args(argv + 1, argv + argc);
    //Fail if file was not passed
    if(args.size() != 1){
        cout << "I expect one and only one source file argument." << endl;
        return 1;
    }
    //Load file
    ifstream file(args[0]);
    //Fail if the file couldn't be loaded
    if(!file.is_open()){
        cout << "I couldn't open the file." << endl;
        return 1;
    }
    //Get file contents
    vector<string> lines;
    string line = "";
    while(getline(file, line)){
        lines.push_back(line);
    }
    
    vector<pair<int, string> > st_lines = stylize(lines);
    vector<string> pre_tokens = pretokenize(st_lines);
    vector<Token> tokens = lextokens(pre_tokens);
    grammar_check(tokens);
    
    return 0;
}
