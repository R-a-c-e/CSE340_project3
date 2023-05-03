//
// Created by racec on 4/24/2023.
//

#ifndef PROJECT3_PARSER_H
#define PROJECT3_PARSER_H

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"


class parser {
public:
    struct InstructionNode *parse_generate_intermediate_representation();
    void parse_var_section();
    void parse_id_list();
    struct InstructionNode *parse_body();
    struct InstructionNode *parse_stmt_list();
    struct InstructionNode *parse_stmt();
    struct InstructionNode *parse_assign_stmt();
    struct InstructionNode *parse_expr(struct InstructionNode *stmt);
    Token parse_primary();
    int parse_op();
    struct InstructionNode *parse_output_stmt();
    struct InstructionNode *parse_input_stmt();
    struct InstructionNode *parse_while_stmt(struct StatementNode *stmt);
    struct InstructionNode *parse_if_stmt(struct StatementNode *stmt);
    struct InstructionNode *parse_condition();
    int parse_relop();
    struct InstructionNode *parse_switch_stmt();
    struct InstructionNode *parse_for_stmt();
    struct InstructionNode *parse_case_list(struct IfStatement *switchStmt, struct StatementNode *endStmt);
    struct InstructionNode *parse_case(struct IfStatement *switchStmt, struct StatementNode *endStmt);
    struct InstructionNode *parse_default_case();
    struct InstructionNode *parse_inputs();
    struct InstructionNode *parse_num_list();
    void printVariables();

private:
    LexicalAnalyzer lexer;

    Token expect(TokenType expected_type);
    Token peek();
};


#endif //PROJECT3_PARSER_H
