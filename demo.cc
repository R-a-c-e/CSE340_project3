#include <stdlib.h>
#include <iostream>
using namespace std;
#include <string>
#include <vector>
#include "compiler.h"
#include "lexer.h"
//
// Created by racec on 4/24/2023.
//

#include "demo.h"


struct Pair {
    string name;
    int addr;
};


LexicalAnalyzer lexer;

//the vector used to hold variables and their memory locations
vector<Pair> varBuf;

//tracks the next available address
int nextAddy;
int nodeNum = 0;



//this is the highest level of abstraction, it calls everything else.
//returns the head of a linked list of instructions for the compiler to go through.
//this method is called from the main in compiler.cc
struct InstructionNode* parse_generate_intermediate_representation() {
    demo parser;
    struct InstructionNode* program = parser.parse_program();
    return program;
}

//TODO: finish dependencies
//program → var_section body inputs
struct InstructionNode* demo::parse_program() {
    nextAddy = 0;

    //parse through the variable section
    parse_var_section();

    //parse through the body
    struct InstructionNode* instruct = new InstructionNode;
    instruct = parse_body();

    //parse through the inputs
    parse_inputs();
    return instruct;
}


//var_section → id_list SEMICOLON
void demo::parse_var_section() {
    parse_id_list();
    expect(SEMICOLON);
}


//id_list → ID COMMA id_list | ID
void demo::parse_id_list(){
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

//TODO: finish dependencies, this method done
//body → LBRACE stmt_list RBRACE
struct InstructionNode* demo::parse_body() {

    expect(LBRACE);
    struct InstructionNode* instruct = parse_stmt_list();
    expect(RBRACE);

    return instruct;
}

//stmt_list → stmt stmt_list | stmt
struct InstructionNode* demo::parse_stmt_list(){
    //stmt
    struct InstructionNode* instruct = parse_stmt();

    Token tok = lexer.peek(1);
    //if it is of a valid type: stmt_list
    if (tok.token_type == ID || tok.token_type == WHILE || tok.token_type == IF || tok.token_type == SWITCH ||
            tok.token_type == FOR || tok.token_type == OUTPUT || tok.token_type == INPUT) {
        struct InstructionNode* instruct1 = parse_stmt_list();
        append(instruct, instruct1);
    }
    return instruct;
}


//TODO: finish dependencies, but no more work needs to be done here
//stmt → assign_stmt | while_stmt | if_stmt | switch_stmt | for_stmt | output_stmt | input_stmt
struct InstructionNode* demo::parse_stmt() {
    //cout << "inside: parse_stmt" << endl;
    struct InstructionNode* instruct = new InstructionNode;
    instruct->next = nullptr;

    TokenType tok = lexer.peek(1).token_type;
    switch(tok){
        case 2:     //FOR
            instruct = parse_for_stmt();
            break;

        case 3:     //IF
            instruct = parse_if_stmt();
            instruct->type = CJMP;
            break;

        case 4:     //WHILE
            instruct = parse_while_stmt();
            instruct->type = CJMP;
            break;

        case 5:     //SWITCH
            instruct = parse_switch_stmt();
            instruct->type = CJMP;
            break;

        case 8:     //INPUT
            instruct = parse_input_stmt(instruct);
            instruct->type = IN;
            //cout << instruct->type << endl;
            break;

        case 9:     //OUTPUT
            instruct = parse_output_stmt(instruct);
            instruct->type = OUT;
            //cout << instruct->type << endl;
            break;

        case 29:    //ASSIGNMENT (ID)
            instruct = parse_assignment_stmt(instruct);
            instruct->type = ASSIGN;
            //cout << instruct->type << endl;
            break;
    }
    //cout << "parse_stmt: about to return" << endl;
    return instruct;
}

//TODO
struct InstructionNode* demo::parse_switch_stmt(){
    //cout << "\n\ninside: parse_switch_stmt" << endl;
    return nullptr;
}

//TODO
struct InstructionNode* demo::parse_for_stmt(){
    expect(FOR);
    expect(LPAREN);

    struct InstructionNode *assign1 = new InstructionNode;
    assign1->assign_inst = parse_assignment_stmt(assign1);
    assign1->type = ASSIGN;

    // This is going to be used to hold the whileStmt
    struct InstructionNode* condition = new InstructionNode;
    consition = parse_condition();
    struct InstructionNode* whileStmt = new InstructionNode;
    condition->next = whileStmt;
    condition->type = CJMP;

    // Chain the next instruction
    assign1->next = condition;

    expect(SEMICOLON);

    // This assign will happen after every iteration of the while statement
    struct InstructionNode* assign2 = new InstructionNode;
    assign2->type = ASSIGN;
    assign2->assign_inst = parse_assignment_stmt();
    expect(RPAREN);

    // Finished setting up the structure, now to parse the body
    // The body is basically the same structure as the while loop, but with assign2 being the next of the whileStmt

    // While Statement = whatever the body afterward is
    whileStmt = parse_body();

    // Create a new StatementNode to hold a GotoStatement
    struct InstructionNode* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;
    struct InstructionNode* jump = new InstructionNode;
    jump->type = JMP;
    jump->next = noop;
    jump->jmp_inst.target = assign1;

    append(whileStmt, assign2);


    struct StatementNode *noop = new StatementNode;
    noop->type = NOOP_STMT;
    noop->next = NULL;

    whileStmt->false_branch = noop;

    condition->if_stmt = whileStmt;

    GotoNode->next = noop;

    condition->next = noop;

    return assign1;
}


//TODO
//while_stmt → WHILE condition body
struct InstructionNode* demo::parse_while_stmt(){
    //WHILE
    expect(WHILE);

    //condition
    struct InstructionNode* instruct = new InstructionNode;
    instruct->type = CJMP;
    instruct = parse_condition();

    //body
    instruct->next = parse_body();

    //set up jump back
    struct InstructionNode* jump = new InstructionNode;
    jump->type = JMP;
    jump->jmp_inst.target = instruct;

    //append jump to end of body
    append(instruct->next,jump);


    //set up WHILE target;
    struct InstructionNode* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;

    //jump needs to point at noop for when the WHILE condition is not met
    jump->next = noop;

    //make the noop the WHILE statements target
    instruct->cjmp_inst.target = noop;

    return instruct;
}


//if_stmt → IF condition body
struct InstructionNode* demo::parse_if_stmt(){
    //IF
    expect(IF);

    //condition
    struct InstructionNode* instruct = parse_condition();

    //set up target
    struct InstructionNode* noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;

    //body
    instruct->next = parse_body();

    //add the noop to the end of the if statement
    append(instruct->next, noop);

    //make the noop the IF statements target
    instruct->cjmp_inst.target = noop;


    return instruct;
}

//condition → primary relop primary
struct InstructionNode* demo::parse_condition(){
    //primary
    struct InstructionNode* instruct = new InstructionNode;
    instruct->cjmp_inst.operand1_index = parse_primary();

    //relop
    switch (parse_relop()){
        case 25:
            instruct->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
            break;
        case 26:
            instruct->cjmp_inst.condition_op = CONDITION_GREATER;
            break;
        case 27:
            instruct->cjmp_inst.condition_op = CONDITION_LESS;
            break;
    }

    //primary
    instruct->cjmp_inst.operand2_index = parse_primary();
    return instruct;
}

//relop → GREATER | LESS | NOTEQUAL
int demo::parse_relop() {
    Token op = lexer.GetToken();
    return op.token_type;
}

//input_stmt → input ID SEMICOLON
struct InstructionNode* demo::parse_input_stmt(struct InstructionNode *instruct){

    //input
    expect(INPUT);

    //set ID location
    Token tok = expect(ID);
    if(inside_buffer(tok.lexeme) == true) {
        instruct->input_inst.var_index = find_index(tok.lexeme);
    }
    else {
        syntax_error();
    }
    //SEMICOLON
    expect(SEMICOLON);
    //cout << "parse_input_stmt: found the semicolon!" << endl;

    return instruct;
}


//output_stmt → output ID SEMICOLON
struct InstructionNode* demo::parse_output_stmt(struct InstructionNode *instruct){
    //cout << "\n\ninside: parse_output_stmt" << endl;
    instruct->type = OUT;
    //output
    expect(OUTPUT);
    //ID
    Token tok = expect(ID);
    //set ID location
    if(inside_buffer(tok.lexeme) == true) {
        instruct->output_inst.var_index = find_index(tok.lexeme);
        //cout << "parse_output_stmt: setting " << tok.lexeme << "'s index to " << instruct->output_inst.var_index << endl;
    }
    else {
        //cout << "parse_output_stmt: calling syntax error" << endl;
        syntax_error();
    }
    //SEMICOLON

    expect(SEMICOLON);
    //cout << "parse_output_stmt: found a semicolon" << endl;
    return instruct;
}


//assign_stmt → ID EQUAL primary SEMICOLON  1
//assign_stmt → ID EQUAL expr SEMICOLON     2
struct InstructionNode* demo::parse_assignment_stmt(struct InstructionNode* instruct){

    // find and set the left hand side
    Token tok = expect(ID);
    instruct->assign_inst.left_hand_side_index = find_index(tok.lexeme);

    //move passed the EQUAL sign
    expect(EQUAL);

    TokenType type = lexer.peek(2).token_type;
    //if the 2nd token is a semicolon we are doing assign_stmt 1
    if(type == 18){

        instruct->assign_inst.operand1_index = parse_primary();
        instruct->assign_inst.op = OPERATOR_NONE;
    }
    //else were doing assign_stmt 2
    else {
        instruct = parse_expr(instruct);
    }

    //find the semicolon and return
    expect(SEMICOLON);
    return instruct;
}


//expr → primary op primary
struct InstructionNode* demo::parse_expr(InstructionNode *instruct) {

    //primary
    instruct->assign_inst.operand1_index = parse_primary();
    //op
    TokenType type = parse_op();
    if(type == 11){
        instruct->assign_inst.op = OPERATOR_PLUS; //PLUS
    }
    else if(type == 12){
        instruct->assign_inst.op = OPERATOR_MINUS; //MINUS
    }
    else if(type == 13){
        instruct->assign_inst.op = OPERATOR_DIV; //DIV
    }
    else if(type == 14){
        instruct->assign_inst.op = OPERATOR_MULT; //MULT
    }
    //primary
    instruct->assign_inst.operand2_index = parse_primary();

    return instruct;
}


//op → PLUS | MINUS | MULT | DIV
TokenType demo::parse_op() {
    //cout << "\ninside: parse_op" << endl;
    Token tok = lexer.GetToken();
    return tok.token_type;
}


//primary → ID | NUM
int demo::parse_primary() {

    Token tok = lexer.peek(1);
    //first we see what lied ahead and if it's a NUM we do the following
    if(tok.token_type == NUM){
        tok = expect(NUM);

        //if the number doesn't exist we need to create it
        if(inside_buffer(tok.lexeme) == false){
            //add value and address into temporary buffer
            Pair addToBuf;
            addToBuf.name = tok.lexeme;
            addToBuf.addr = nextAddy;
            varBuf.push_back(addToBuf);

            //now add the umber into the mem[] buffer
            mem[nextAddy] = stoi(tok.lexeme);
            nextAddy++;
        }
        //now we return the address of the token
        return find_index(tok.lexeme);
    }


    //if what lies ahead is an ID then we go here
    else if(tok.token_type == ID){

        tok = expect(ID);
        //only return the address if ID exists in the buffer
        if(inside_buffer(tok.lexeme) == true){
            return find_index(tok.lexeme);
        }
    }

    //if neither then it's a syntax error
    else{
        syntax_error();
    }
}


//inputs → num_list
//num_list → NUM | NUM num_list
void demo::parse_inputs() {
    //cout << "\n\n\n\ninside: parse_inputs" << endl;
    Token tok = expect(NUM);
    inputs.push_back(stoi(tok.lexeme));
    if(lexer.peek(1).token_type == 28){
        parse_inputs();
    }
}

//adds a node to the end of the linked list
void demo::append(struct InstructionNode* instruct, struct InstructionNode* instruct1) {
    if (!instruct) {
        return;
    }
    struct InstructionNode* curr = instruct;
    while (curr->next != nullptr) {
        curr = curr->next;
    }
    curr->next = instruct1;
}


//calls getToken and checks if the token type is expected
Token demo::expect(TokenType type){
    //cout << "inside: expect";
    Token tok = lexer.GetToken();
    //cout << "     found:" << tok.lexeme << endl;
    if (tok.token_type != type){
        //cout << "expect: calling syntax error" << endl;
        syntax_error();
    }
    return tok;
}

//DUMB
void demo::syntax_error() {
    //cout << "\ninside: syntax_error" << endl;
    cout << "Syntax Error\n";
    exit(1);
}

//finds the value in the variables buffer and returns its memory location
int demo::find_index(string lex){
    //cout << "\ninside: find_index" << endl;
    //cout << "find_index: looking for: " << lex << endl;
    for(int i = 0; i < varBuf.size(); i++){
        //cout << "find_index: loop " << i << endl;
        if(varBuf[i].name == lex){
            //cout << "find_index: returning " << i << endl;
            return i;
        }
    }
    return -1;
}

//returns true if the lexeme exists inside the varBuf
bool demo::inside_buffer(std::string var){
    for(int i = 0; i < varBuf.size(); i++){
        if(varBuf[i].name == var){
            return true;
        }
    }
    return false;
}

void demo::printVariables() {
    for(int i = 0; i < varBuf.size(); i++){
        cout << "index " << i << ": " << varBuf[i].name << " at address " << varBuf[i].addr << endl;
    }
}

void demo::printNodes(struct InstructionNode* instruct) {
    struct InstructionNode* curr = new InstructionNode;
    curr = instruct;
    if (curr == nullptr) {
        cout << "printNodes: empty instructionNode" << endl;
    } else {
        while (curr != nullptr) {
            cout << curr->type << endl;
            curr = curr->next;
        }

    }
}

/*
    // a, b, c, d;
    // {
    //     input a;
    //     input b;
    //     c = 10;
    // 
    //     IF c <> a
    //     {
    //         output b;
    //     }
    // 
    //     IF c > 1
    //     {
    //         a = b + 900;
    //         input d;
    //         IF a > 10
    //         {
    //             output d;
    //         }
    //     }
    // 
    //     d = 0;
    //     WHILE d < 4
    //     {
    //         c = a + d;
    //         IF d > 1
    //         {
    //             output d;
    //         }
    //         d = d + 1;
    //     }
    // }
    // 1 2 3 4 5 6

    // Assigning location for variable "a"
    int address_a = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "b"
    int address_b = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "c"
    int address_c = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "d"
    int address_d = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for constant 10
    int address_ten = next_available;
    mem[next_available] = 10;
    next_available++;

    // Assigning location for constant 1
    int address_one = next_available;
    mem[next_available] = 1;
    next_available++;

    // Assigning location for constant 900
    int address_ninehundred = next_available;
    mem[next_available] = 900;
    next_available++;

    // Assigning location for constant 3
    int address_three = next_available;
    mem[next_available] = 3;
    next_available++;

    // Assigning location for constant 0
    int address_zero = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for constant 4
    int address_four = next_available;
    mem[next_available] = 4;
    next_available++;

    struct InstructionNode * i1 = new InstructionNode;
    struct InstructionNode * i2 = new InstructionNode;
    struct InstructionNode * i3 = new InstructionNode;
    struct InstructionNode * i4 = new InstructionNode;
    struct InstructionNode * i5 = new InstructionNode;
    struct InstructionNode * i6 = new InstructionNode;
    struct InstructionNode * i7 = new InstructionNode;
    struct InstructionNode * i8 = new InstructionNode;
    struct InstructionNode * i9 = new InstructionNode;
    struct InstructionNode * i10 = new InstructionNode;
    struct InstructionNode * i11 = new InstructionNode;
    struct InstructionNode * i12 = new InstructionNode;
    struct InstructionNode * i13 = new InstructionNode;
    struct InstructionNode * i14 = new InstructionNode;
    struct InstructionNode * i15 = new InstructionNode;
    struct InstructionNode * i16 = new InstructionNode;
    struct InstructionNode * i17 = new InstructionNode;
    struct InstructionNode * i18 = new InstructionNode;
    struct InstructionNode * i19 = new InstructionNode;
    struct InstructionNode * i20 = new InstructionNode;
    struct InstructionNode * i21 = new InstructionNode;
    struct InstructionNode * i22 = new InstructionNode;

    i1->type = IN;                                      // input a
    i1->input_inst.var_index = address_a;
    i1->next = i2;

    i2->type = IN;                                      // input b
    i2->input_inst.var_index = address_b;
    i2->next = i3;

    i3->type = ASSIGN;                                  // c = 10
    i3->assign_inst.left_hand_side_index = address_c;
    i3->assign_inst.op = OPERATOR_NONE;
    i3->assign_inst.operand1_index = address_ten;
    i3->next = i4;

    i4->type = CJMP;                                    // if c <> a
    i4->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    i4->cjmp_inst.operand1_index = address_c;
    i4->cjmp_inst.operand2_index = address_a;
    i4->cjmp_inst.target = i6;                          // if not (c <> a) skip forward to NOOP
    i4->next = i5;

    i5->type = OUT;                                     // output b
    i5->output_inst.var_index = address_b;
    i5->next = i6;

    i6->type = NOOP;                                    // NOOP after IF
    i6->next = i7;

    i7->type = CJMP;                                    // if c > 1
    i7->cjmp_inst.condition_op = CONDITION_GREATER;
    i7->cjmp_inst.operand1_index = address_c;
    i7->cjmp_inst.operand2_index = address_one;
    i7->cjmp_inst.target = i13;                         // if not (c > 1) skip forward to NOOP (way down)
    i7->next = i8;

    i8->type = ASSIGN;                                  // a = b + 900
    i8->assign_inst.left_hand_side_index = address_a;
    i8->assign_inst.op = OPERATOR_PLUS;
    i8->assign_inst.operand1_index = address_b;
    i8->assign_inst.operand2_index = address_ninehundred;
    i8->next = i9;

    i9->type = IN;                                      // input d
    i9->input_inst.var_index = address_d;
    i9->next = i10;

    i10->type = CJMP;                                   // if a > 10
    i10->cjmp_inst.condition_op = CONDITION_GREATER;
    i10->cjmp_inst.operand1_index = address_a;
    i10->cjmp_inst.operand2_index = address_ten;
    i10->cjmp_inst.target = i12;                        // if not (a > 10) skipp forward to NOOP
    i10->next = i11;

    i11->type = OUT;                                    // output d
    i11->output_inst.var_index = address_d;
    i11->next = i12;

    i12->type = NOOP;                                   // NOOP after inner IF
    i12->next = i13;

    i13->type = NOOP;                                   // NOOP after outer IF
    i13->next = i14;

    i14->type = ASSIGN;                                 // d = 0
    i14->assign_inst.left_hand_side_index = address_d;
    i14->assign_inst.op = OPERATOR_NONE;
    i14->assign_inst.operand1_index = address_zero;
    i14->next = i15;

    i15->type = CJMP;                                   // if d < 4
    i15->cjmp_inst.condition_op = CONDITION_LESS;
    i15->cjmp_inst.operand1_index = address_d;
    i15->cjmp_inst.operand2_index = address_four;
    i15->cjmp_inst.target = i22;                        // if not (d < 4) skip whole WHILE body
    i15->next = i16;

    i16->type = ASSIGN;                                 // c = a + d
    i16->assign_inst.left_hand_side_index = address_c;
    i16->assign_inst.op = OPERATOR_PLUS;
    i16->assign_inst.operand1_index = address_a;
    i16->assign_inst.operand2_index = address_d;
    i16->next = i17;

    i17->type = CJMP;                                   // if d > 1
    i17->cjmp_inst.condition_op = CONDITION_GREATER;
    i17->cjmp_inst.operand1_index = address_d;
    i17->cjmp_inst.operand2_index = address_one;
    i17->cjmp_inst.target = i19;                        // if not (d > 1) skip body of IF
    i17->next = i18;

    i18->type = OUT;                                    // output d
    i18->output_inst.var_index = address_d;
    i18->next = i19;

    i19->type = NOOP;                                   // NOOP after body of IF
    i19->next = i20;

    i20->type = ASSIGN;                                 // d = d + 1
    i20->assign_inst.left_hand_side_index = address_d;
    i20->assign_inst.op = OPERATOR_PLUS;
    i20->assign_inst.operand1_index = address_d;
    i20->assign_inst.operand2_index = address_one;
    i20->next = i21;

    i21->type = JMP;
    i21->jmp_inst.target = i15;
    i21->next = i22;

    i22->type = NOOP;                                   // NOOP after body of WHILE
    i22->next = NULL;

    // Inputs
    inputs.push_back(1);
    inputs.push_back(2);
    inputs.push_back(3);
    inputs.push_back(4);
    inputs.push_back(5);
    inputs.push_back(6);

    return i1;
    */



