/*
 FCL COMPILER
 By Martín del Río - 2018
 
 Things to add:
 - Arrays
 - Ways to get a char from a string
 - Ways to get the length of a string
 - Logic operations
 - Text ifs
 - Text whiles
 - User input
 - Subroutines
 - Decimals
 - Ways to open files
 - Allow - in variable names
 - Add support for negative number printing
 - STDin
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include "cpptrim/cpptrim.h"
using namespace std;

string langName = "LARTOL";
string asm_code = "";
string asm_values = ";VALUES SECTION\nsection .data";

vector<pair<string, pair<int, int>>> variables;
int str_count = 0;
int while_count = 0;
stack<int> while_stack;
int if_count = 0;
stack<int> if_stack;

vector<pair<int, string> > stylize(vector<string> & lines){
    vector<pair<int, string> > new_lines;
    
    //Remove comments
    for(int i = 0; i < lines.size(); ++i){
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

void add_asm(string txt){
    asm_code += txt + "\n";
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string add_string(string token){
    string strname = "__str" + to_string(++str_count);
    replaceAll(token, "\\n", "\", 10, \"");
    replaceAll(token, "\\t", "\", 9, \"");
    //Los colores miden 7 bytes cada uno
    replaceAll(token, "\\GRAY", "\033[1;30m");
    replaceAll(token, "\\RED", "\033[1;31m");
    replaceAll(token, "\\GREEN", "\033[1;32m");
    replaceAll(token, "\\YELLOW", "\033[1;33m");
    replaceAll(token, "\\BLUE", "\033[1;34m");
    replaceAll(token, "\\PINK", "\033[1;35m");
    replaceAll(token, "\\CYAN", "\033[1;36m");
    replaceAll(token, "\\WHITE", "\033[1;37m");
    //Reiniciar el estilo mide 4 bytes
    replaceAll(token, "\\NORMAL", "\033[0m");
    asm_values += "\n" + strname + ": db " + token + ", 0";
    return strname;
}

//Takes an array of source lines and splits them into pre-tokens
//(that is strings that will be token str_values)
vector<vector<string>> pretokenize(const vector<pair<int, string> > & lines){
    vector<vector<string>> lines_tokens;
    string splitters = " \t";
    vector<string> tokens;
    for(int i = 0; i < lines.size(); ++i){
        //Get line number
        tokens.push_back(to_string(lines[i].first));
        //Get line
        string line = lines[i].second;
        //Set variables for parsing
        string current_token = "";
        char string_delimiter = 0;
        for(int j = 0; j < line.size(); ++j){
            //If the current character is a string delimiter
            if(string_delimiter == 0 && line[j] == '"'){
                //Set current string delimiter
                string_delimiter = line[j];
                //Push current token and clean it
                if(current_token != "")
                    tokens.push_back(current_token);
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
            //If the current character is a splitter
            else if(string_delimiter == 0 && splitters.find(line[j]) != -1){
                //Push current token and clean it
                if(current_token != ""){
                    tokens.push_back(current_token);
                    current_token = "";
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
        
        //Push current line
        lines_tokens.push_back(tokens);
        tokens.clear();
    }
    //Return our vector of tokens
    return lines_tokens;
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

bool is_variable(string token){
    if(token.size() > 1 && token[0] == '$') return true;
    return false;
}

bool is_string(string token){
    if(token.size() > 5 && token.substr(0, 5) == "__str") return true;
    return false;
}

bool is_subroutine(string token){
    if(token.size() > 1 && token[0] == '@') return true;
    return false;
}

void typeError(int lineNumber){
    cout << "\033[1;31mError: operator \033[1;37mtype mismatch\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    exit(1);
}

void unexpectedError(int lineNumber, string token){
    cout << "\033[1;31mError: unexpected \033[1;37m" << token << "\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    exit(1);
}

void expectedError(int lineNumber, string whatWasExpected){
    cout << "\033[1;31mError: expected \033[1;37m" << whatWasExpected << "\033[1;31m on line \033[1;37m" << lineNumber << "\033[1;31m.\033[0m" << endl;
    exit(1);
}

void checkLineEnding(int line_size, int i, vector<string> line, int line_number){
    if(line_size > i) unexpectedError(line_number, line[i+1]);
    if(line_size < i) expectedError(line_number, "more stuff");
}

void add_var(string name, string type){
    if(type == "txt"){
        variables.push_back(make_pair(name, make_pair(1, 0)));
    }
    if(type == "num"){
        variables.push_back(make_pair(name, make_pair(0, 0)));
    }
    if(type.size() > 4 && type.substr(0, 4) == "dec_"){
        variables.push_back(make_pair(name, make_pair(0, stoi(type.substr(4)))));
    }
}
bool var_is_txt(string name){
    for(auto var : variables){
        if(var.first == name){
            return var.second.first == 1;
        }
    }
    return false;
}
bool var_is_num(string name){
    for(auto var : variables){
        if(var.first == name){
            return var.second.first == 0;
        }
    }
    return false;
}

int get_var_decimals(string name){
    if(var_is_txt(name)) return 0;
    for(auto var : variables){
        if(var.first == name){
            return var.second.second;
        }
    }
    return 0;
}

int get_num_decimals(string num){
    //TODO
}

bool dataSection = false;
bool procedureSection = false;


void compile(vector<string> & line){
    int line_number = stoi(line[0]);
    int line_size = line.size() - 1;
    for(int i = 1; i < line.size(); ++i){
        //Get token
        string token = line[i];
        
        if(token == "DATA:") //DONE
        {
            //Line must only contain this
            if(line_size > 1)
                unexpectedError(line_number, line[i+1]);
            //Must not be already in DATA: nor PROCEDURE:
            if(dataSection || procedureSection)  
                unexpectedError(line_number, token);
            //Execute
            dataSection = true;
            add_asm(";DATA SECTION");
        }
        
        else if(token == "PROCEDURE:") //DONE
        {
            //Line must only contain this
            if(line_size > 1)
                unexpectedError(line_number, line[i+1]);
            //Must not be already in PROCEDURE:
            if(procedureSection)  
                unexpectedError(line_number, token);
            //Execute
            dataSection = true;
            procedureSection = true;
            add_asm(";PROCEDURE SECTION");
            add_asm("section .text");
            add_asm("_start:");
        }
        
        else if(dataSection && is_variable(token)) //DONE
        {
            //Variables in DATA: section
            //Format $VAR IS <TYPE> <LENGTH>
            
            string var_name = token.substr(1);
            
            if(line[++i] == "IS"){
                if(line[++i] == "NUMBER"){
                    int decimals = 0;
                    if(i + 3 == line_size && line[i+1] == "WITH"  && line[i+3] == "DECIMALS" && is_number(line[i+2])){
                        decimals = stoi(line[i+2]);
                        checkLineEnding(line_size, i+3, line, line_number);
                    }
                    else checkLineEnding(line_size, i, line, line_number);
                    add_var(var_name, "dec_"+to_string(decimals));
                    add_asm("__var"+var_name+": dq 0");
                    if(decimals > 0) i+=3;
                }
                else if(line[i] == "TEXT"){
                    //Line must only contain this and a number
                    checkLineEnding(line_size, i+1, line, line_number);
                    if(is_number(line[++i])){
                        add_var(var_name, "txt");
                        add_asm("__var"+var_name+": times " + line[i] + " db 0");
                        add_asm("__var"+var_name+"_len: dq " + line[i]);
                    }
                    else unexpectedError(line_number, line[i]);
                }
                else unexpectedError(line_number, line[i]);
            }
            else expectedError(line_number, "IS");
        }
        
        else if(procedureSection && token == "DISPLAY") //DONE
        {
            //FORMAT:
            //DISPLAY <VAR | STRING | NUMBER>
            if(is_variable(line[++i])){
                checkLineEnding(line_size, i, line, line_number);
                 string var_name = line[i].substr(1);
                if(var_is_num(var_name)){
                    add_asm("mov rax, [__var" + var_name + "]");
                    add_asm("call printnumber");
                }
                else if(var_is_txt(var_name)){
                    add_asm("mov rax, 1 ; write");
                    add_asm("mov rdi, 1 ; STDOUT_FILENO");
                    add_asm("mov rsi, __var" + var_name);
                    add_asm("call strlen");
                    add_asm("syscall");
                }
                else typeError(line_number);
            }
            else if(is_string(line[i])){
                checkLineEnding(line_size, i, line, line_number);
                add_asm("mov rax, 1 ; write");
                add_asm("mov rdi, 1 ; STDOUT_FILENO");
                add_asm("mov rsi, " + line[i]);
                add_asm("call strlen");
                add_asm("syscall");
            }
            else if(is_number(line[i])){
                checkLineEnding(line_size, i, line, line_number);
                add_asm("mov rax, " + line[i]);
                add_asm("call printnumber");
            }
            else expectedError(line_number, "VARIABLE, TEXT OR NUMBER");
        }
        
        else if(procedureSection && token == "ACCEPT")
        {
            //FORMAT:
            //ACCEPT VAR
            if(is_variable(line[++i])){
                checkLineEnding(line_size, i, line, line_number);
                //TODO
            }
            else expectedError(line_number, "VARIABLE");
        }
        
        else if(procedureSection && token == "JOIN")
        {
            //FORMAT:
            //JOIN <VAR | STRING | NUMBER> AND <VAR | STRING | NUMBER> IN VAR
            
            //CHECK AND IN VAR
            if(line[i+2] != "AND") expectedError(line_number, "AND");
            if(line[i+4] != "IN") expectedError(line_number, "IN");
            if(!is_variable(line[i+5])) expectedError(line_number, "VARIABLE");
            
            //Check line length
            checkLineEnding(line_size, i+5, line, line_number);
            
            //CHECK <VAR | STRING | NUMBER> & <VAR | STRING | NUMBER>
            //TODO: Capaz acá debería ver el tipo de la variable aparte
            if(is_variable(line[i+1]) && is_variable(line[i+3])){
                //TODO
            }
            else if(is_variable(line[i+1]) && is_number(line[i+3])){
                //TODO
            }
            else if(is_variable(line[i+1]) && is_string(line[i+3])){
                //TODO
            }
            else if(is_number(line[i+1]) && is_variable(line[i+3])){
                //TODO
            }
            else if(is_number(line[i+1]) && is_number(line[i+3])){
                //TODO
            }
            else if(is_number(line[i+1]) && is_string(line[i+3])){
                //TODO
            }
            else if(is_string(line[i+1]) && is_variable(line[i+3])){
                //TODO
            }
            else if(is_string(line[i+1]) && is_number(line[i+3])){
                //TODO
            }
            else if(is_string(line[i+1]) && is_string(line[i+3])){
                //TODO
            }
            else expectedError(line_number, "VARIABLE, TEXT OR NUMBER");
            
            //Increment i
            i += 5;
        }
        
        else if(procedureSection && token == "CALL")
        {
            //FORMAT:
            //CALL SUBROUTINE *(TEXT | STRING | VARIABLE)
            // TODO ">>>TERMINAR CALL!<<<"
            //TODO
            return;
        }
        
        else if(procedureSection && token == "STORE") //DONE
        {
            //FORMAT:
            //STORE <VAR | STRING | NUMBER> IN VAR
            
            //CHECK AND IN VAR
            if(line[i+2] != "IN") expectedError(line_number, "IN");
            if(!is_variable(line[i+3])) expectedError(line_number, "VARIABLE");
            
            //Check line length
            checkLineEnding(line_size, i+3, line, line_number);
            
            //Get destination variable
            string destination_name = line[i+3].substr(1);
            
            //CHECK <VAR | STRING | NUMBER> & <VAR | STRING | NUMBER>
            if(is_variable(line[i+1])){
                string var_name = line[i+1].substr(1);
                //Check type
                if(var_is_num(var_name) != var_is_num(destination_name))
                    typeError(line_number);
                //Copy values
                if(var_is_num(var_name)){
                    add_asm("mov rax, [__var"+var_name+"]");
                    add_asm("mov qword [__var"+destination_name+"], rax");
                }
                else if(var_is_txt(var_name)){
                    add_asm("mov rax, __var" + destination_name);
                    add_asm("mov rbx, __var" + var_name);
                    add_asm("mov rdx, [__var" + destination_name + "_len]");
                    add_asm("call copystring");
                }
                else typeError(line_number);
            }
            else if(is_number(line[i+1])){
                if(!var_is_num(destination_name)) typeError(line_number);
                add_asm("mov qword [__var"+destination_name+"], " + line[i+1]);
            }
            else if(is_string(line[i+1])){
                if(!var_is_txt(destination_name)) typeError(line_number);
                add_asm("mov rax, __var" + destination_name);
                add_asm("mov rbx, " + line[i+1]);
                add_asm("mov rdx, [__var" + destination_name + "_len]");
                add_asm("call copystring");
            }
            else expectedError(line_number, "VARIABLE, TEXT OR NUMBER");
            
            //Increment i
            i += 3;
        }
        
        else if(procedureSection && token == "WHILE")
        {
            //FORMAT:
            //WHILE <VAR | STRING | NUMBER> <CONDITION> DO
            //CONDITIONS:
            //IS EQUAL TO <VAR | STRING | NUMBER>
            //IS NOT EQUAL TO <VAR | STRING | NUMBER>
            //IS GREATER THAN <VAR | NUMBER>
            //IS LESS THAN <VAR | NUMBER>
            //IS GREATER THAN OR EQUAL TO <VAR | NUMBER>
            //IS LESS THAN OR EQUAL TO <VAR | NUMBER>
            
            ++while_count;
            add_asm("_while" + to_string(while_count) + ":");
            while_stack.push(while_count);
            
            int doPosition = 0;
            for(int j = i; j < line.size(); ++j){
                if(line[j] == "DO"){
                    doPosition = j;
                    break;
                }
            }
            if(doPosition == 0) expectedError(line_number, "DO");
            checkLineEnding(line_size, doPosition, line, line_number);
            
            string condition = "";
            
            //Get condition
            int conditionLength = 0;
            int j = i+1;
            for(; j < doPosition; ++j){
                if(is_variable(line[j]) || is_number(line[j]) || is_string(line[j])){
                    if(condition == "") continue;
                    else break;
                }else{
                    condition += line[j] + " ";
                    ++conditionLength;
                }
            }
            trim(condition);
            
            //Check syntax
            if(!is_variable(line[i+1]) && !is_number(line[i+1]) && !is_string(line[i+1])) expectedError(line_number, "VARIABLE, TEXT or NUMBER (left side)");
            if(!is_variable(line[j]) && !is_number(line[j]) && !is_string(line[j])) expectedError(line_number, "VARIABLE, TEXT or NUMBER (right side)");
            if(doPosition != i+3 + conditionLength)
                    unexpectedError(line_number, "EXTRA VARIABLE, TEXT OR NUMBER");
            
            string first = line[i+1];
            string second = line[j];
            
            //String comparisons
            if(is_string(first) || is_string(second) || var_is_txt(first) || var_is_txt(second)){
                //TODO
            }
            //Numeric comparisons
            else{
                if(is_number(first) && is_number(second)){
                    add_asm("mov rax, " + first);
                    add_asm("mov rbx, " + second);
                }
                else if(is_number(first) && is_variable(second)){
                    add_asm("mov rax, " + first);
                    add_asm("mov rbx, [__var" + second.substr(1) + "]");
                }
                else if(is_variable(first) && is_number(second)){
                    add_asm("mov rax, [__var" + first.substr(1) + "]");
                    add_asm("mov rbx, " + second);
                }
                else if(is_variable(first) && is_variable(second)){
                    add_asm("mov rax, [__var" + first.substr(1) + "]");
                    add_asm("mov rbx, [__var" + second.substr(1) + "]");
                }
                else typeError(line_number);
                //Compare
                add_asm("cmp rax, rbx");
                //Check condition
                if(condition == "IS EQUAL TO"){
                    add_asm("jne _endWhile" + to_string(while_count));
                }
                else if(condition == "IS NOT EQUAL TO"){
                    add_asm("je _endWhile" + to_string(while_count));
                }
                else if(condition == "IS GREATER THAN"){
                    add_asm("jle _endWhile" + to_string(while_count));
                }
                else if(condition == "IS LESS THAN"){
                    add_asm("jge _endWhile" + to_string(while_count));
                }
                else if(condition == "IS GREATER THAN OR EQUAL TO"){
                    add_asm("jl _endWhile" + to_string(while_count));
                }
                else if(condition == "IS LESS THAN OR EQUAL TO"){
                    add_asm("jg _endWhile" + to_string(while_count));
                }
                else expectedError(line_number, "VALID CONDITION");
            }
            break;
        }
        
        else if(procedureSection && token == "REPEAT") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            if(while_stack.empty()) expectedError(line_number, "WHILE for unmatched REPEAT");
            else{
                add_asm("jmp _while"+to_string(while_stack.top()));
                add_asm("_endWhile"+to_string(while_stack.top())+":");
                while_stack.pop();
            }
        }
        
        else if(procedureSection && token == "CONTINUE") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            if(while_stack.empty()) expectedError(line_number, "WHILE for unmatched CONTINUE");
            else{
                add_asm("jmp _while"+to_string(while_stack.top()));
            }
        }
        
        else if(procedureSection && token == "BREAK") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            if(while_stack.empty()) expectedError(line_number, "WHILE for unmatched BREAK");
            else{
                add_asm("jmp _endWhile"+to_string(while_stack.top()));
            }
        }
        
        else if(procedureSection && token == "IF")
        {
            //FORMAT:
            //IF <VAR | STRING | NUMBER> <CONDITION> THEN
            //CONDITIONS:
            //IS EQUAL TO <VAR | STRING | NUMBER>
            //IS NOT EQUAL TO <VAR | STRING | NUMBER>
            //IS GREATER THAN <VAR | NUMBER>
            //IS LESS THAN <VAR | NUMBER>
            //IS GREATER THAN OR EQUAL TO <VAR | NUMBER>
            //IS LESS THAN OR EQUAL TO <VAR | NUMBER>
            
            ++if_count;
            add_asm("_if" + to_string(if_count) + ":");
            if_stack.push(if_count);
            if_stack.push(if_count);
            
            int doPosition = 0;
            for(int j = i; j < line.size(); ++j){
                if(line[j] == "THEN"){
                    doPosition = j;
                    break;
                }
            }
            if(doPosition == 0) expectedError(line_number, "THEN");
            checkLineEnding(line_size, doPosition, line, line_number);
            
            string condition = "";
            
            //Get condition
            int conditionLength = 0;
            int j = i+1;
            for(; j < doPosition; ++j){
                if(is_variable(line[j]) || is_number(line[j]) || is_string(line[j])){
                    if(condition == "") continue;
                    else break;
                }else{
                    condition += line[j] + " ";
                    ++conditionLength;
                }
            }
            trim(condition);
            
            //Check syntax
            if(!is_variable(line[i+1]) && !is_number(line[i+1]) && !is_string(line[i+1])) expectedError(line_number, "VARIABLE, TEXT or NUMBER (left side)");
            if(!is_variable(line[j]) && !is_number(line[j]) && !is_string(line[j])) expectedError(line_number, "VARIABLE, TEXT or NUMBER (right side)");
            if(doPosition != i+3 + conditionLength)
                    unexpectedError(line_number, "EXTRA VARIABLE, TEXT OR NUMBER");
            
            string first = line[i+1];
            string second = line[j];
            
            //String comparisons
            if(is_string(first) || is_string(second) || var_is_txt(first) || var_is_txt(second)){
                //TODO
            }
            //Numeric comparisons
            else{
                if(is_number(first) && is_number(second)){
                    add_asm("mov rax, " + first);
                    add_asm("mov rbx, " + second);
                }
                else if(is_number(first) && is_variable(second)){
                    add_asm("mov rax, " + first);
                    add_asm("mov rbx, [__var" + second.substr(1) + "]");
                }
                else if(is_variable(first) && is_number(second)){
                    add_asm("mov rax, [__var" + first.substr(1) + "]");
                    add_asm("mov rbx, " + second);
                }
                else if(is_variable(first) && is_variable(second)){
                    add_asm("mov rax, [__var" + first.substr(1) + "]");
                    add_asm("mov rbx, [__var" + second.substr(1) + "]");
                }
                else typeError(line_number);
                //Compare
                add_asm("cmp rax, rbx");
                //Check condition
                if(condition == "IS EQUAL TO"){
                    add_asm("jne _elseIf" + to_string(if_count));
                }
                else if(condition == "IS NOT EQUAL TO"){
                    add_asm("je _elseIf" + to_string(if_count));
                }
                else if(condition == "IS GREATER THAN"){
                    add_asm("jle _elseIf" + to_string(if_count));
                }
                else if(condition == "IS LESS THAN"){
                    add_asm("jge _elseIf" + to_string(if_count));
                }
                else if(condition == "IS GREATER THAN OR EQUAL TO"){
                    add_asm("jl _elseIf" + to_string(if_count));
                }
                else if(condition == "IS LESS THAN OR EQUAL TO"){
                    add_asm("jg _elseIf" + to_string(if_count));
                }
                else expectedError(line_number, "VALID CONDITION");
            }
            break;
        }
        
        else if(procedureSection && token == "ELSE") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            if(if_stack.empty()) expectedError(line_number, "IF for unmatched ELSE");
            else{
                add_asm("jmp _endIf"+to_string(if_stack.top()));
                add_asm("_elseIf"+to_string(if_stack.top())+":");
                if_stack.pop();
            }
        }
        
        else if(procedureSection && token == "END-IF") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            if(if_stack.empty()) expectedError(line_number, "IF for unmatched END-IF");
            else{
                int ifNumber = if_stack.top();
                string endifAsm = "_endIf"+to_string(ifNumber)+":";
                if_stack.pop();
                if(!if_stack.empty() && if_stack.top() == ifNumber){
                    if_stack.pop();
                    endifAsm = "_elseIf"+to_string(ifNumber)+":\n" + endifAsm;
                }
                add_asm(endifAsm);
            }
        }
        
        else if(procedureSection && token == "END") //DONE
        {
            checkLineEnding(line_size, i, line, line_number);
            //TODO
            add_asm("mov rax, 60 ;Exit");
            add_asm("mov rdi, 0 ;Exit code (0)");
            add_asm("syscall");
        }
        
        else if(procedureSection && token == "ADD") //DONE
        {
            checkLineEnding(line_size, i+5, line, line_number);
            
            //FORMAT: ADD <VAR | NUM> AND <VAR | NUM> IN <VAR>
            
            //Check format
            if(line[i+2] != "AND") expectedError(line_number, "AND");
            if(line[i+4] != "IN") expectedError(line_number, "IN");
            if(!is_variable(line[i+5]))
                expectedError(line_number, "DESTINATION VARIABLE");
            
            string dest_var = line[i+5].substr(1);
            
            if(is_number(line[i+1]) && is_number(line[i+3])){
                add_asm("mov rax, "+line[i+1]);
                add_asm("add rax, "+line[i+3]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_number(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+3].substr(1);
                add_asm("mov rax, "+line[i+1]);
                add_asm("add rax, [__var"+var1+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_number(line[i+3])){
                string var1 = line[i+1].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("add rax, "+line[i+3]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+1].substr(1);
                string var2 = line[i+3].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("add rax, [__var"+var2+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else typeError(line_number);
            
            i+=5;
        }
        
        else if(procedureSection && token == "SUBTRACT") //DONE
        {
            checkLineEnding(line_size, i+5, line, line_number);
            
            //FORMAT: SUBTRACT <VAR | NUM> FROM <VAR | NUM> IN <VAR>
            
            //Check format
            if(line[i+2] != "FROM") expectedError(line_number, "FROM");
            if(line[i+4] != "IN") expectedError(line_number, "IN");
            if(!is_variable(line[i+5]))
                expectedError(line_number, "DESTINATION VARIABLE");
            
            string dest_var = line[i+5].substr(1);
            
            if(is_number(line[i+1]) && is_number(line[i+3])){
                add_asm("mov rax, "+line[i+3]);
                add_asm("sub rax, "+line[i+1]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_number(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+3].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("sub rax, "+line[i+1]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_number(line[i+3])){
                string var1 = line[i+1].substr(1);
                add_asm("mov rax, "+line[i+3]);
                add_asm("sub rax, [__var"+var1+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+1].substr(1);
                string var2 = line[i+3].substr(1);
                add_asm("mov rax, [__var"+var2+"]");
                add_asm("sub rax, [__var"+var1+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else typeError(line_number);
            
            i+=5;
        }
        
        else if(procedureSection && token == "MULTIPLY") //DONE
        {
            checkLineEnding(line_size, i+5, line, line_number);
            
            //FORMAT: MULTIPLY <VAR | NUM> BY <VAR | NUM> IN <VAR>
            
            //Check format
            if(line[i+2] != "BY") expectedError(line_number, "BY");
            if(line[i+4] != "IN") expectedError(line_number, "IN");
            if(!is_variable(line[i+5]))
                expectedError(line_number, "DESTINATION VARIABLE");
            
            string dest_var = line[i+5].substr(1);
            
            if(is_number(line[i+1]) && is_number(line[i+3])){
                add_asm("mov rax, "+line[i+1]);
                add_asm("imul rax, "+line[i+3]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_number(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+3].substr(1);
                add_asm("mov rax, "+line[i+1]);
                add_asm("imul rax, [__var"+var1+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_number(line[i+3])){
                string var1 = line[i+1].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("imul rax, "+line[i+3]);
                add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+1].substr(1);
                string var2 = line[i+3].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("imul rax, [__var"+var2+"]");
                add_asm("mov [__var"+dest_var+"], rax");
            }else typeError(line_number);
            
            i+=5;
        }
        
        else if(procedureSection && token == "DIVIDE") //DONE
        {
            bool withRemainder = false;
            if(line_size >= 8) withRemainder = (line[i+6] == "REMAINDER");
            
            if(!withRemainder)
                checkLineEnding(line_size, i+5, line, line_number);
            else
                checkLineEnding(line_size, i+8, line, line_number);
            
            //PRIMERO DEBERIA SHIFTEAR EL DIVIDENDO POR LA CANTIDAD DE
            //DECIMALES QUE TIENE + LA CANTIDAD DE DECIMALES NO 0 DEL DIVISOR TODO
            
            //FORMATS:
            //DIVIDE <VAR | NUM> BY <VAR | NUM> IN <VAR>
            //DIVIDE <VAR | NUM> BY <VAR | NUM> IN <VAR> REMAINDER IN <VAR>
            
            //Check format
            if(line[i+2] != "BY") expectedError(line_number, "BY");
            if(line[i+4] != "IN") expectedError(line_number, "IN");
            if(withRemainder)
                if(line[i+7] != "IN") expectedError(line_number, "IN");
            bool noDestVariable = false;
            if(line[i+5] == "_"){
                noDestVariable = true;
            }
            if(!noDestVariable && !is_variable(line[i+5]))
                expectedError(line_number, "DESTINATION VARIABLE");
            if(withRemainder && !is_variable(line[i+8]))
                expectedError(line_number, "REMAINDER DESTINATION VARIABLE");
            
            string dest_var = line[i+5].substr(1);
            
            add_asm("mov rdx, 0");
            
            if(is_number(line[i+1]) && is_number(line[i+3])){
                add_asm("mov rax, "+line[i+1]);
                add_asm("mov rbx, "+line[i+3]);
                add_asm("idiv rbx");
                if(!noDestVariable) 
                    add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_number(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+3].substr(1);
                add_asm("mov rax, "+line[i+1]);
                add_asm("mov rbx, [__var"+var1+"]");
                add_asm("idiv rbx");
                if(!noDestVariable) 
                    add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_number(line[i+3])){
                string var1 = line[i+1].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("mov rbx, "+line[i+3]);
                add_asm("idiv rbx");
                if(!noDestVariable) 
                    add_asm("mov [__var"+dest_var+"], rax");
            }else if(is_variable(line[i+1]) && is_variable(line[i+3])){
                string var1 = line[i+1].substr(1);
                string var2 = line[i+3].substr(1);
                add_asm("mov rax, [__var"+var1+"]");
                add_asm("mov rbx, [__var"+var2+"]");
                add_asm("idiv rbx");
                if(!noDestVariable) 
                    add_asm("mov [__var"+dest_var+"], rax");
            }else typeError(line_number);
            
            if(withRemainder){
                string rem_var = line[i+8].substr(1);
                add_asm("mov [__var"+rem_var+"], rdx");
            }
            
            if(!withRemainder)
                i+=5;
            else
                i+=8;
        }
        
        //Unknown word
        else{
            unexpectedError(line_number, token);
        }
    }
}

void compile_lines(vector<vector<string>> & lines){
    int compiled_lines = 0;
    for(vector<string> line : lines){
        //Remove comments
        vector<string> uncommented_line;
        for(string token : line){
            if(token[0] == ';'){
                break;
            }
            //If token is string not in comment
            if(token.size() > 2 && token[0] == '"'){
                token = add_string(token);
            }
            //If token isn't empty
            if(token.size() > 0)
                uncommented_line.push_back(token);
        }
        //If line si not empty
        if(uncommented_line.size() > 1){
            //Compile line
            compile(uncommented_line);
            ++compiled_lines;
        }
    }
    if(compiled_lines == 0){
        add_asm("_start:");
    }
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
    vector<vector<string>> token_lines = pretokenize(st_lines);
    compile_lines(token_lines);
    
    //Check if everything was closed
    if(!while_stack.empty()){
        cout << "\033[1;31mError: there may be one or more WHILEs without REPEATs\033[0m" << endl;
        exit(1);
    }
    if(!if_stack.empty()){
        cout << "\033[1;31mError: there may be one or more IFs without END-IFs\033[0m" << endl;
        exit(1);
    }
    
    //Add exit point
    add_asm("mov rax, 60 ;Exit");
    add_asm("mov rdi, 0 ;Exit code (0)");
    add_asm("syscall");
    
    //Add data section
    asm_values += "\ndigitSpace: times 100 db 0";
    asm_values += "\ndigitSpacePos times 8 db 0";
    asm_code = asm_values + "\n" + asm_code;
    
    //Add entry point
    asm_code = "global _start\n" + asm_code;
    
    //Add external lib
    add_asm("%include \"coblib.asm\"");
    
    cout << asm_code << endl;
    
    return 0;
}
