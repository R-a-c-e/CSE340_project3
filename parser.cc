//
// Created by racec on 4/24/2023.
//

#include "parser.h"

struct Pair {
    string name;
    int addr;
};

vector<Pair> varBuf;
int nextAddy;


//this is the highest level of abstraction, it calls everything else
struct InstructionNode *parse_generate_intermediate_representation(){
    nextAddy = 0;
    //parse through the variable section
    parse_var_section();
    //parse through the body
    //parse_body();
    //parse through the inputs
    //parse_inputs();
}

void parser::parse_var_section() {
    parse_id_list();
    expect(SEMICOLON);
}

//this looks at either a single ID or a list of IDs
void parser::parse_id_list(){
    //create variable from expect and add it to the variable buffer
    Pair addToBuf;
    addToBuf.name = expect(ID).lexeme;
    addToBuf.addr = nextAddy;
    nextAddy++;
    varBuf.push_back(addToBuf);

    // if the next token type is a comma then it's a list,
    // and we must continue through it
    if(lexer.peek(1).token_type == COMMA){
        expect(COMMA);
        parse_id_list();
    }
}

struct InstructionNode parser::parse_body() {

}

struct InstructionNode parser::parse_inputs() {

}


void parser::printVariables() {
    for(int i = 0; i < varBuf.size(); i++){
        cout << "index " << i << ": " << varBuf[i].name << " at address " << varBuf[i].addr << endl;
    }
}

void parser::syntax_error() {
    cout << "Syntax Error\n";
    exit(1);
}