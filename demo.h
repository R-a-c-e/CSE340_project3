//
// Created by racec on 4/24/2023.
//

#ifndef PROJECT3_DEMO_H
#define PROJECT3_DEMO_H

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "compiler.h"
#include "lexer.h"


class demo {
public:
    struct InstructionNode *parse_program();
    void parse_var_section();
    void parse_id_list();
    struct InstructionNode* parse_body();
    struct InstructionNode* parse_stmt_list();
    struct InstructionNode* parse_stmt();
    struct InstructionNode* parse_assignment_stmt(struct InstructionNode *instruct);
    struct InstructionNode* parse_expr(struct InstructionNode *instruct);
    int parse_primary();
    TokenType parse_op();
    struct InstructionNode* parse_output_stmt(struct InstructionNode *instruct);
    struct InstructionNode* parse_input_stmt(struct InstructionNode *instruct);
    struct InstructionNode* parse_while_stmt();
    struct InstructionNode* parse_if_stmt();
    struct InstructionNode* parse_condition();
    int parse_relop();
    struct InstructionNode* parse_switch_stmt();
    struct InstructionNode* parse_for_stmt();
    struct InstructionNode* parse_case_list(struct IfStatement *switchStmt, struct InstructionNode *endStmt);
    struct InstructionNode* parse_case(struct IfStatement *switchStmt, struct InstructionNode *endStmt);
    struct InstructionNode* parse_default_case();
    void parse_inputs();
    void printVariables();
    void printNodes(struct InstructionNode* instruct);
    int find_index(std::string lex);
    bool inside_buffer(std::string var);
    void append(struct InstructionNode* inst, struct InstructionNode* instl);


private:

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
};


#endif //PROJECT3_DEMO_H
