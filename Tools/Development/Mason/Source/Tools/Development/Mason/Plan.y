%code top {
#include <string_view>
}

// Requires features introduced in Bison 3.7.
%require "3.7"

// Use IELR table generation by default.
//
// This isn't needed but is recommended by the Bison manual.
%define lr.type ielr

// Enable lookahead correction to improve error messages.
%define parse.lac full

// Parser generation configuration:
//  - the parser should be C++, not C.
//  - always generate location tracking code.
//  - use std::string_view for wrapping file name strings.
%language "C++"
%locations
%define api.filename.type       {std::string_view}
%define api.namespace           {Tools::Development::Mason}
%define api.parser.class        {PlanParser}
%define api.symbol.prefix       {SYM_}
%define api.token.constructor   true
%define api.token.prefix        {TOK_}
//%define api.value.automove      true
%define api.value.type          variant

// Debugging options:
%define parse.assert            true
%define parse.error             detailed
%define parse.trace             true

// The start symbol for the grammar is a fully-formed plan.
%start plan

// Terminal tokens.
%token ID "identifier"
%token NUMBER_OR_VERSION "number or version"
%token STRING "string"
%token AND_OP "&&"
%token OR_OP "||"
%token AND_KEYWORD "and keyword"
%token OR_KEYWORD "or keyword"
%token IS_LEQ "<="
%token IS_GEQ ">="
%token IS_EQ "=="
%token IS_NEQ "!="
%token IN "in"
%token NOT "not"

// Non-terminal tokens.

// Token associativity and precedence (order of declaration, from lowest to highest).
%left AND_OP OR_OP AND_KEYWORD OR_KEYWORD
%nonassoc IS_LEQ IS_GEQ IS_EQ IS_NEQ '<' '>'
%right '!'

// Semantic types for each of the tokens.

%%


plan:
        %empty
    |   instruction_seq

instruction_seq:
        instruction
    |   instruction_seq instruction

instruction:
        ID optional_id '{' settings_seq '}' ';'

optional_id:
        %empty
    |   ID

settings_seq:
        %empty
    |   settings_seq_content

settings_seq_content:
        setting
    |   settings_seq_content setting

setting:
        ID elaboration '=' setting_value ';'

elaboration:
        %empty
    |   ':' '{' elaboration_content_seq '}' 

elaboration_content_seq:
        %empty
    |   elaboration_content_seq elaboration_content

elaboration_content:
        constraint_expr ';'

constraint_expr:
        '(' constraint_expr ')'
    |   constraint_and_expr
    |   constraint_or_expr
    |   constraint_cmp_expr
    |   constraint_not_expr
    |   constraint_id_or_literal_expr
    |   constraint_in_expr

constraint_and_expr:
        constraint_expr AND_OP constraint_expr
    |   constraint_expr AND_KEYWORD constraint_expr

constraint_or_expr:
        constraint_expr OR_OP constraint_expr
    |   constraint_expr OR_KEYWORD constraint_expr

constraint_cmp_expr:
        constraint_id_or_literal_expr constraint_comparison_op constraint_id_or_literal_expr
    |   constraint_id_or_literal_expr constraint_comparison_op constraint_id_or_literal_expr constraint_comparison_op constraint_id_or_literal_expr

constraint_comparison_op:
        IS_EQ
    |   IS_NEQ
    |   IS_LEQ
    |   IS_GEQ
    |   '<'
    |   '>'

constraint_not_expr:
        '!' constraint_expr

constraint_id_or_literal_expr:
        ID
    |   NUMBER_OR_VERSION
    |   STRING

constraint_in_expr:
        constraint_id_or_literal_expr IN constraint_list_expr
    |   constraint_id_or_literal_expr NOT IN constraint_list_expr

constraint_list_expr:
    '[' constraint_list_item_seq_or_empty ']'

constraint_list_item_seq_or_empty:
        %empty
    |   constraint_list_item_seq optional_comma

constraint_list_item_seq:
        constraint_list_item
    |   constraint_list_item_seq "," constraint_list_item

constraint_list_item:
        constraint_id_or_literal_expr

setting_value:
        setting_literal
    |   setting_list

setting_literal:
        ID
    |   NUMBER_OR_VERSION
    |   STRING

setting_list:
    '[' setting_list_item_seq_or_empty ']' elaboration

setting_list_item_seq_or_empty:
        %empty
    |   setting_list_item_seq optional_comma

setting_list_item_seq:
        setting_list_item
    |   setting_list_item_seq ',' setting_list_item

setting_list_item:
        setting_value

optional_comma:
        %empty
    |   ','

%%
