#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/limelib.h"

#define MAX_TOKEN_LEN 64
#define MAX_OPS 10000
#define MAX_TOKENS 10000
#define MAX_MACRO_TOKENS 300

#define WSCHAR " \t\n"

#define NUMCHAR "0123456789"
#define HEXNUMCHAR "0123456789aAbBcCdDeEfF"
#define SPECIAL ".:;[]*/\\'\"#$-"

#pragma GCC diagnostic ignored "-Wformat-extra-args"
#pragma GCC diagnostic ignored "-Wformat"

enum token_type
{
    NUMBER,
    WORD,
    KEYWORD,
    LABEL_MARK,
    LABEL_JUMP,
    IDENTIFIER,
    MACRO,
    TOKEN_LIST_START,
    TOKEN_LIST_END,
    COMMENT_START,
    COMMENT_END,
    LAST_TOKEN,
};

enum op_type
{
    PUSH = 0,
    DUPE,
    COPY,
    SWAP,
    DROP,
    SLIDE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    STORE,
    FETCH,
    MARK,
    CALL,
    JMP,
    JZ,
    JN,
    RET,
    END,
    PRINTC,
    PRINTI,
    READC,
    READI,
    INVALID
};

enum lexer_state
{
    NORMAL,
    COMMENT,
    ML_COMMENT
};

struct loc
{
    size_t col;
    size_t row;
};

struct token
{
    enum token_type type;
    union
    {
        char c;
        char* s;
        int64_t n;
    };
    struct loc loc;
};

struct op
{
    enum op_type op;
    int64_t val;
};

struct lexer
{
    char* c;
    struct loc loc;
    enum lexer_state state;
};


char lexer_get_current(struct lexer* lxr)
{
    return *lxr->c;
}

char lexer_get_next(struct lexer* lxr)
{
    if (lexer_get_current(lxr) == '\n')
    {
        lxr->loc.col = 0;
        lxr->loc.row++;
    }
    lxr->loc.col++;
    return *(++lxr->c);
}

char lexer_peek_next(struct lexer* lxr)
{
    return *(lxr->c + 1);
}

bool word_is_keyword(char* s)
{
    if (!strcmp(s, "PUSH") || !strcmp(s, "push"))
        return true;
        // Compatability with other interpreter
    else if (!strcmp(s, "DUPE") || !strcmp(s, "dupe") ||
             !strcmp(s, "dup") || !strcmp(s, "DUP"))
        return true;
    else if (!strcmp(s, "COPY") || !strcmp(s, "copy"))
        return true;
    else if (!strcmp(s, "SWAP") || !strcmp(s, "swap"))
        return true;
    else if (!strcmp(s, "DROP") || !strcmp(s, "drop"))
        return true;
    else if (!strcmp(s, "SLIDE") || !strcmp(s, "slide"))
        return true;
    else if (!strcmp(s, "ADD") || !strcmp(s, "add"))
        return true;
    else if (!strcmp(s, "SUB") || !strcmp(s, "sub"))
        return true;
    else if (!strcmp(s, "MUL") || !strcmp(s, "mul"))
        return true;
    else if (!strcmp(s, "DIV") || !strcmp(s, "div"))
        return true;
    else if (!strcmp(s, "MOD") || !strcmp(s, "mod"))
        return true;
    else if (!strcmp(s, "STORE") || !strcmp(s, "store"))
        return true;
        // Compatability with other interpreter
    else if (!strcmp(s, "FETCH") || !strcmp(s, "fetch") ||
             !strcmp(s, "RETRIVE") || !strcmp(s, "retrive"))
        return true;
    else if (!strcmp(s, "CALL") || !strcmp(s, "call"))
        return true;
    else if (!strcmp(s, "JMP") || !strcmp(s, "jmp"))
        return true;
    else if (!strcmp(s, "JZ") || !strcmp(s, "jz"))
        return true;
    else if (!strcmp(s, "JN") || !strcmp(s, "jn"))
        return true;
    else if (!strcmp(s, "RET") || !strcmp(s, "ret"))
        return true;
    else if (!strcmp(s, "END") || !strcmp(s, "end"))
        return true;
    else if (!strcmp(s, "PRINTC") || !strcmp(s, "printc"))
        return true;
    else if (!strcmp(s, "PRINTI") || !strcmp(s, "printi"))
        return true;
    else if (!strcmp(s, "READC") || !strcmp(s, "readc"))
        return true;
    else if (!strcmp(s, "READI") || !strcmp(s, "readi"))
        return true;
    else
        return false;
}

enum op_type keyword_get_optype(char* s)
{
    if (!strcmp(s, "PUSH") || !strcmp(s, "push"))
        return PUSH;
        // Compatability with other interpreter
    else if (!strcmp(s, "DUPE") || !strcmp(s, "dupe") ||
             !strcmp(s, "dup") || !strcmp(s, "DUP"))
        return DUPE;
    else if (!strcmp(s, "COPY") || !strcmp(s, "copy"))
        return COPY;
    else if (!strcmp(s, "SWAP") || !strcmp(s, "swap"))
        return SWAP;
    else if (!strcmp(s, "DROP") || !strcmp(s, "drop"))
        return DROP;
    else if (!strcmp(s, "SLIDE") || !strcmp(s, "slide"))
        return SLIDE;
    else if (!strcmp(s, "ADD") || !strcmp(s, "add"))
        return ADD;
    else if (!strcmp(s, "SUB") || !strcmp(s, "sub"))
        return SUB;
    else if (!strcmp(s, "MUL") || !strcmp(s, "mul"))
        return MUL;
    else if (!strcmp(s, "DIV") || !strcmp(s, "div"))
        return DIV;
    else if (!strcmp(s, "MOD") || !strcmp(s, "mod"))
        return MOD;
    else if (!strcmp(s, "STORE") || !strcmp(s, "store"))
        return STORE;
        // Compatability with other interpreter
    else if (!strcmp(s, "FETCH") || !strcmp(s, "fetch") ||
             !strcmp(s, "RETRIVE") || !strcmp(s, "retrive"))
        return FETCH;
    else if (!strcmp(s, "CALL") || !strcmp(s, "call"))
        return CALL;
    else if (!strcmp(s, "JMP") || !strcmp(s, "jmp"))
        return JMP;
    else if (!strcmp(s, "JZ") || !strcmp(s, "jz"))
        return JZ;
    else if (!strcmp(s, "JN") || !strcmp(s, "jn"))
        return JN;
    else if (!strcmp(s, "RET") || !strcmp(s, "ret"))
        return RET;
    else if (!strcmp(s, "END") || !strcmp(s, "end"))
        return END;
    else if (!strcmp(s, "PRINTC") || !strcmp(s, "printc"))
        return PRINTC;
    else if (!strcmp(s, "PRINTI") || !strcmp(s, "printi"))
        return PRINTI;
    else if (!strcmp(s, "READC") || !strcmp(s, "readc"))
        return READC;
    else if (!strcmp(s, "READI") || !strcmp(s, "readi"))
        return READI;
    else
        return INVALID;
}

bool op_require_num(enum op_type op)
{
    if (op == PUSH ||
        op == COPY ||
        op == SLIDE ||
        op == MARK ||
        op == CALL ||
        op == JMP ||
        op == JZ ||
        op == JN)
        return true;

    return false;
}


void print_token(struct token* token)
{
    switch (token->type)
    {
        case NUMBER:
            printf("NUM: %lld        loc: %d:%d\n", token->n,
                                               token->loc.row,
                                               token->loc.col);
            break;
        case WORD:
            printf("%s          loc: %d:%d\n",  token->s,
                                               token->loc.row,
                                               token->loc.col);
            break;
        case KEYWORD:
            printf("KEY: %s           loc: %d:%d\n", token->s,
                                                token->loc.row,
                                                token->loc.col);
            break;
        case IDENTIFIER:
            printf("Id: %s           loc: %d:%d\n",token->s,
                                                token->loc.row,
                                                token->loc.col);
            break;
        case LABEL_MARK:
            printf("MARK:%llx         loc: %d:%d\n",token->n,
                                                token->loc.row,
                                                token->loc.col);
            break;
        case LABEL_JUMP:
            printf("LAB: %llx         loc: %d:%d\n",token->n,
                                                token->loc.row,
                                                token->loc.col);
            break;
        case MACRO:
            printf("Macro             loc: %d:%d\n",
                                                token->loc.row,
                                                token->loc.col);
            break;
        case TOKEN_LIST_START:
            printf("T List         loc: %d:%d\n",
                                                token->loc.row,
                                                token->loc.col);
            break;
        case TOKEN_LIST_END:
            printf("T List end     loc: %d:%d\n",
                                                token->loc.row,
                                                token->loc.col);
            break;
        case COMMENT_START:
            printf("Comment      loc: %d:%d\n",
                                               token->loc.row,
                                               token->loc.col);
            break;
        case COMMENT_END:
            printf("Comment end  loc: %d:%d\n",
                                              token->loc.row,
                                              token->loc.col);
            break;
        default:
            fprintf(stderr, "Something went wrong, last marker token slipped  through");
            exit(1);
    }
}

void print_op(struct op* op)
{
    switch (op->op)
    {
        case PUSH:
            printf("    PUSH %lld\n", op->val);
            break;
        case DUPE:
            printf("    DUPE\n");
            break;
        case COPY:
            printf("    COPY %lld\n", op->val);
            break;
        case SWAP:
            printf("    SWAP\n");
            break;
        case DROP:
            printf("    DROP\n");
            break;
        case SLIDE:
            printf("    SLIDE %lld\n", op->val);
            break;
        case ADD:
            printf("    ADD\n");
            break;
        case SUB:
            printf("    SUB\n");
            break;
        case MUL:
            printf("    MUL\n");
            break;
        case DIV:
            printf("    DIV\n");
            break;
        case MOD:
            printf("    MOD\n");
            break;
        case STORE:
            printf("    STORE\n");
            break;
        case FETCH:
            printf("    FETCH\n");
            break;
        case MARK:
            printf(".%llx:\n", op->val);
            break;
        case CALL:
            printf("    CALL .%llx\n", op->val);
            break;
        case JMP:
            printf("    JMP .%llx\n", op->val);
            break;
        case JZ:
            printf("    JZ .%llx\n", op->val);
            break;
        case JN:
            printf("    JN .%llx\n", op->val);
            break;
        case RET:
            printf("    RET\n");
            break;
        case END:
            printf("    END\n");
            break;
        case PRINTC:
            printf("    PRINTC\n");
            break;
        case PRINTI:
            printf("    PRINTI\n");
            break;
        case READC:
            printf("    READC\n");
            break;
        case READI:
            printf("    READI\n");
            break;
        default:
            printf("UNKNOWN OP\n");
            break;
    }
}

void dec_to_whitespace_bin(char out[65], int64_t num)
{
    char buffer[65];
    if (num < 0)
    {
        num = -num;
        out[0] = '\t';
    }
    else if (num == 0)
    {
        out[0] = ' ';
        out[1] = ' ';
        out[2] = '\0';
        return;
    }
    else
        out[0] = ' ';

    int index = 0;

    while (num > 0)
    {
        if (num % 2)
        {
            buffer[index] = '\t';
        }
        else
        {
            buffer[index] = ' ';
        }

        index++;
        num = num >> 1;
    }
    buffer[index] = '\0';
    int j = index - 1;
    for (int i = 1; i <= index + 1; i++)
    {
        out[i] = buffer[j];
        j--;
    }
}

void write_op(FILE* file, struct op* op)
{
    char number[65];
    switch (op->op)
    {
        case PUSH:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "  %s\n", number);
            break;
        case DUPE:
            fprintf(file, " \n ");
            break;
        case COPY:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, " \t %s\n", number);
            break;
        case SWAP:
            fprintf(file, " \n\t");
            break;
        case DROP:
            fprintf(file, " \n\n");
            break;
        case SLIDE:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, " \t\n%s\n", number);
            break;
        case ADD:
            fprintf(file, "\t   ");
            break;
        case SUB:
            fprintf(file, "\t  \t");
            break;
        case MUL:
            fprintf(file, "\t  \n");
            break;
        case DIV:
            fprintf(file, "\t \t ");
            break;
        case MOD:
            fprintf(file, "\t \t\t");
            break;
        case STORE:
            fprintf(file, "\t\t ");
            break;
        case FETCH:
            fprintf(file, "\t\t\t");
            break;
        case MARK:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "\n  %s\n", number);
            break;
        case CALL:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "\n \t%s\n", number);
            break;
        case JMP:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "\n \n%s\n", number);
            break;
        case JZ:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "\n\t %s\n", number);
            break;
        case JN:
            dec_to_whitespace_bin(number, op->val);
            fprintf(file, "\n\t\t%s\n", number);
            break;
        case RET:
            fprintf(file, "\n\t\n");
            break;
        case END:
            fprintf(file, "\n\n\n");
            break;
        case PRINTC:
            fprintf(file, "\t\n  ");
            break;
        case PRINTI:
            fprintf(file, "\t\n \t");
            break;
        case READC:
            fprintf(file, "\t\n\t ");
            break;
        case READI:
            fprintf(file, "\t\n\t\t");
            break;
        default:
            printf("UNKNOWN OP\n");
            print_op(op);
            break;
    }

}

struct token token_get_hex(struct lexer* lxr)
{
    struct token token = {.type = NUMBER, .loc = lxr->loc};
    char c = lexer_get_current(lxr);
    size_t index = 0;

    char buffer[MAX_TOKEN_LEN + 1];

    buffer[index++] = c;

    while (strchr(HEXNUMCHAR, lexer_peek_next(lxr)) &&
                           lexer_peek_next(lxr) != '\0')
    {
        if (index >= MAX_TOKEN_LEN)
        {
            fprintf(stderr,
                    "[LXR]Number too large at %d:%d\n",
                    lxr->loc.col,
                    lxr->loc.row);

            exit(1);
        }
        c = lexer_get_next(lxr);
        buffer[index] = c;
        index++;
    }

    if (!strchr(SPECIAL , lexer_peek_next(lxr)) &&
        !strchr(WSCHAR, lexer_peek_next(lxr)) &&
        lexer_peek_next(lxr) != '\0')
    {
        fprintf(stderr, "Syntax error at %d:%d\n", lxr->loc.col,
                                                   lxr->loc.row);
                                                   exit(1);
    }

    lexer_get_next(lxr);
    buffer[index] = '\0';

    token.n = strtoll(buffer, NULL, 16);
    return token;
}

struct token token_get_number(struct lexer* lxr)
{
    struct token token = {.type = NUMBER, .loc = lxr->loc};
    char c = lexer_get_current(lxr);
    size_t index = 0;

    if (c == '0')
    {
        if (lexer_peek_next(lxr) == 'x')
        {
            lexer_get_next(lxr);
            lexer_get_next(lxr);
            return token_get_hex(lxr);
        }
        else if (!strchr(WSCHAR, lexer_peek_next(lxr)) &&
                 !strchr(SPECIAL, lexer_peek_next(lxr)))
        {
            fprintf(stderr,
                    "[LXR]Unsupported number base (0%c) at %d:%d\n",
                    lexer_get_next(lxr),
                    lxr->loc.col,
                    lxr->loc.row);

            exit(1);
        }
    }
    char buffer[MAX_TOKEN_LEN + 1];

    buffer[index++] = c;

    while (strchr(NUMCHAR, lexer_peek_next(lxr)) &&
                           lexer_peek_next(lxr) != '\0')
    {
        if (index >= MAX_TOKEN_LEN)
        {
            fprintf(stderr,
                    "[LXR]Number too large at %d:%d\n",
                    lxr->loc.col,
                    lxr->loc.row);

            exit(1);
        }
        c = lexer_get_next(lxr);
        buffer[index] = c;
        index++;
    }

    if (!strchr(SPECIAL , lexer_peek_next(lxr)) &&
        !strchr(WSCHAR, lexer_peek_next(lxr)) &&
        lexer_peek_next(lxr) != '\0')
    {
        fprintf(stderr, "Syntax error at %d:%d\n", lxr->loc.col,
                                                   lxr->loc.row);
                                                   exit(1);
    }

    lexer_get_next(lxr);
    buffer[index] = '\0';

    token.n = atoll(buffer);
    return token;

}
struct token token_get_word(struct lexer* lxr);

struct token token_get_label(struct lexer* lxr)
{
    lexer_get_next(lxr);
    struct token token = token_get_word(lxr);
    if (lexer_get_current(lxr) != ':')
        token.type = LABEL_JUMP;


    else
    {

        token.type = LABEL_MARK;
        lexer_get_next(lxr);
    }

    if (token.type == KEYWORD)
    {

        fprintf(stderr, "Cannot use keyword as lable name! %d:%d \n", lxr->loc.row, lxr->loc.col);
        printf("%s\n", token.s);
        exit(1);
    }

    return token;
}

struct token token_get_char(struct lexer* lxr)
{
    struct token token = {.type = NUMBER, .loc = lxr->loc};
    char c = lexer_get_next(lxr);

    if (lexer_get_next(lxr) != '\'')
    {
        printf("[LXR] Syntax error, unclosed char declaration at %d:%d\n",
                lxr->loc.row,
                lxr->loc.col);
        exit(1);
    }
    lexer_get_next(lxr);
    token.n = (int64_t)c;
    return token;
}


struct token token_get_special(struct lexer* lxr)
{
    char c = lexer_get_current(lxr);

    struct token token;
    switch (c) {
        case '.':
            return token_get_label(lxr);
        case '-':
            return token_get_number(lxr);
        case '\'':
            return token_get_char(lxr);
        case '/':
            if (lexer_peek_next(lxr) == '/')
            {
                token.type = COMMENT_START;
                token.s = "//",
                token.loc = lxr->loc;
                lxr->state = COMMENT;
                lexer_get_next(lxr);
                lexer_get_next(lxr);
                return token;
            }
            if (lexer_peek_next(lxr) == '*')
            {
                token.type = COMMENT_START;
                token.s = "/*",
                token.loc = lxr->loc;
                lxr->state = ML_COMMENT;
                lexer_get_next(lxr);
                lexer_get_next(lxr);
                return token;
            }
            fprintf(stderr, "Syntax error at %d:%d", lxr->loc.row,
                                                     lxr->loc.col);
            exit(1);

        case ';':
            token.type = COMMENT_START;
            token.c = ';';
            token.loc = lxr->loc;
            lxr->state = COMMENT;
            lexer_get_next(lxr);
            return token;

        case '[':
            token.type = TOKEN_LIST_START;
            token.c = '[';
            token.loc = lxr->loc;
            lexer_get_next(lxr);
            return token;

        case ']':
            token.type = TOKEN_LIST_END;
            token.c = ']';
            token.loc = lxr->loc;
            lexer_get_next(lxr);
            return token;
    }
    fprintf(stderr, "[WSL] FOUND UNKNOWN SPECIAL");
    exit(1);
}

struct token token_get_word(struct lexer* lxr)
{
    struct token token = {.type = IDENTIFIER, .loc = lxr->loc };
    char c = lexer_get_current(lxr);

    U8List* c_list = u8_list_create(10);

    u8_list_append(c_list, c);

    while (!strchr(WSCHAR, lexer_peek_next(lxr))  &&
           !strchr(SPECIAL, lexer_peek_next(lxr)) &&
                           lexer_peek_next(lxr)   != '\0')
    {
        c = lexer_get_next(lxr);
        u8_list_append(c_list, (uint8_t)c);
    }

    token.s = u8_list_get_string(c_list);
    lexer_get_next(lxr);


    if (word_is_keyword(token.s))
        token.type = KEYWORD;

    if (!strcmp(token.s, "MACRO") || !strcmp(token.s, "macro"))
        token.type = MACRO;

    if (lxr->state != NORMAL)
        token.type = WORD;
    return token;
}

struct token get_next_token(struct lexer* lxr)
{
    char c = lexer_get_current(lxr);

    if (lxr->state != COMMENT)
    {

        while (strchr(WSCHAR, c) && c != '\0')
            c = lexer_get_next(lxr);
    }
    else
    {
        while (strchr(" \t", c) && c != '\0')
            c = lexer_get_next(lxr);

    }

    if (c == '\0')
    {
        struct token token = {.type = LAST_TOKEN, .n = 0, .loc = lxr->loc };
        return token;
    }
    if (lxr->state == NORMAL)
    {
        if (strchr(NUMCHAR, c))
        return token_get_number(lxr);

        if (strchr(SPECIAL, c))
        return token_get_special(lxr);
    }
    else
    {
        if (lxr->state == COMMENT)
        {
            if (c == '\n')
            {
                struct token token = {.type = COMMENT_END,
                                      .c = '\n',
                                      .loc = lxr->loc};
                lxr->state = NORMAL;
                lexer_get_next(lxr);
                return token;
            }
        }
        if (lxr->state == ML_COMMENT)
        {
            if (c == '*' && lexer_peek_next(lxr) == '/')
            {
                struct token token = {.type = COMMENT_END,
                                      .s = "*/",
                                      .loc = lxr->loc };
                lxr->state = NORMAL;
                lexer_get_next(lxr);
                lexer_get_next(lxr);
                return token;
            }
        }
    }
    return token_get_word(lxr);
}

size_t lex(struct lexer* lxr, struct token* return_tokens)
{
    size_t index = 0;

    struct token token = get_next_token(lxr);

    while (index < MAX_TOKENS && token.type != LAST_TOKEN)
    {
        return_tokens[index] = token;
        token = get_next_token(lxr);
        index++;
    }

    return index;
}

size_t pre_process(struct token* token_list,
                   size_t token_list_size,
                   struct token* return_tokens)
{
    size_t index = 0;
    struct token token;

    IntHashMap* label_map = int_hashmap_create();
    PtrHashMap* macro_map = ptr_hashmap_create();
    int32_t label_counter = 0x4a00;
    bool in_comment       = false;
    bool in_macro         = false;
    bool in_tokenlist     = false;

    char* current_identifyer = "[WSPP] CONTROLL FLOW BUG";
    struct token macro_buffer_list[MAX_MACRO_TOKENS];
    size_t macro_token_list_index = 0;
    for (size_t i = 0; i < token_list_size; i++)
    {
        token = token_list[i];

        if (in_comment && token.type != COMMENT_END)
            continue;

        if (in_comment && token.type == COMMENT_END)
        {
            in_comment = false;
            continue;
        }

        if (token.type == COMMENT_START)
        {
            in_comment = true;
            continue;
        }

        if (in_tokenlist)
        {
            if (token.type == TOKEN_LIST_END)
            {
                macro_buffer_list[macro_token_list_index++] = token;
                struct token* macro_heap_list = malloc(sizeof(struct token) * macro_token_list_index);
                if (!macro_heap_list)
                {
                    fprintf(stderr, "[WSPP] MACRO MALLOC FAILED\n");
                    exit(1);
                }
                memcpy(macro_heap_list, &macro_buffer_list, sizeof(struct token) * macro_token_list_index);

                if (!ptr_hashmap_add(macro_map, current_identifyer, macro_heap_list))
                {
                    fprintf(stderr, "[WSPP] Macro with name %s has already been declared    loc %d:%d\n", current_identifyer, macro_buffer_list[0].loc.row, macro_buffer_list[0].loc.col);
                    exit(1);
                }

                in_macro = false;
                in_tokenlist = false;

                macro_token_list_index = 0;
                continue;
            }
            if (token.type == IDENTIFIER)
            {
                OptionalPtr opt = ptr_hashmap_get(macro_map, token.s);

                if (!opt.has_value)
                {
                    fprintf(stderr, "[WSPP] Identifier not found     loc: %d:%d\n", token.loc.row, token.loc.col);
                    exit(1);
                }

                struct token* macro = (struct token*)opt.value;

                while (macro->type != TOKEN_LIST_END)
                {
                    macro_buffer_list[macro_token_list_index++] = *macro;
                    macro++;
                }
                continue;
            }
            macro_buffer_list[macro_token_list_index++] = token;
            continue;
        }

        if (token.type == LABEL_MARK)
        {
            if (!int_hashmap_add(label_map, token.s, label_counter))
            {
                fprintf(stderr, "[WSPP] Multiple lables of same name     loc: %d:%d\n", token.loc.row, token.loc.col);
                exit(1);
            }
            token.n = label_counter++;
        }

        if (token.type == MACRO)
        {
            if (token_list[i + 1].type != IDENTIFIER)
            {
                fprintf(stderr, "[WSPP] Macro needs identifier     loc: %d:%d\n", token.loc.row, token.loc.col);
                exit(1);
            }

            in_macro = true;
            continue;
        }

        if (token.type == IDENTIFIER)
        {
            if (in_macro)
            {
                if (token_list[i + 1].type != TOKEN_LIST_START)
                {
                    fprintf(stderr, "[WSPP] Macro needs list     loc: %d:%d\n", token.loc.row, token.loc.col);
                    exit(1);
                }
                current_identifyer = token.s;
                continue;
            }
            OptionalPtr opt = ptr_hashmap_get(macro_map, token.s);

            if (!opt.has_value)
            {
                fprintf(stderr, "[WSPP] Identifier not found     loc: %d:%d\n", token.loc.row, token.loc.col);
                exit(1);
            }

            struct token* macro = (struct token*)opt.value;

            while (macro->type != TOKEN_LIST_END)
            {
                return_tokens[index++] = *macro;
                macro++;
            }
            continue;
        }

        if (token.type == TOKEN_LIST_START)
        {
            if (!in_macro)
            {
                fprintf(stderr, "[WSPP] Token list outside of macro     loc: %d:%d\n", token.loc.row, token.loc.col);
                exit(1);
            }
            in_tokenlist = true;
            continue;
        }

        return_tokens[index++] = token;
    }

    for (size_t i = 0; i < token_list_size; i++)
    {
        token = return_tokens[i];
        if (token.type == LABEL_JUMP)
        {
            OptionalInt opt = int_hashmap_get(label_map, token.s);
            if (!opt.has_value)
            {
                printf("%s\n", token.s);
                fprintf(stderr, "[WSPP] Jump lable not found\n");
                exit(1);
            }
            token.n = opt.value;
        }
        return_tokens[i] = token;
    }
    return index;
}


size_t parse(struct token* token_list,
             size_t token_list_size,
             struct op* return_ops)
{
    size_t index = 0;
    struct token token;
    struct op op;

    for (size_t i = 0; i < token_list_size; i++)
    {
        token = token_list[i];
        switch (token.type)
        {
            case KEYWORD:
                op.op = keyword_get_optype(token.s);
                if (op_require_num(op.op))
                {
                    if (token_list[i + 1].type != NUMBER && token_list[i + 1].type != LABEL_JUMP)
                    {
                        fprintf(stderr, "[WSP] %s requires number %d\n", token.s, token_list[i + 1].type);
                        exit(1);
                    }
                    op.val = token_list[i + 1].n;
                    i++;
                }
                else
                    op.val = 0;

                return_ops[index++] = op;
                break;

            case LABEL_MARK:
                op.op = MARK;
                op.val = token.n;
                return_ops[index++] = op;
                break;

            default:
                fprintf(stderr, "[WSP] Somthing went wrong, encountered token that should have been handled by pre processor at loc: %d:%d\n", token.loc.row, token.loc.col);
                print_token(&token);
                exit(1);
                break;

        }

    }
    return index;
}

int main(int argc, char** argv)
{
    FILE* in_file = fopen("ws/wsa/char.wsa", "r");

    if (!in_file)
    {
        fprintf(stderr, "Could not open file!\n");
        exit(1);
    }

    FILE* out_file= fopen("ws/ws/char.ws", "w");

    fseek(in_file, 0, SEEK_END);
    size_t file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    char code[file_size];

    char c;
    int real_file_size = 0;
    while ((c = fgetc(in_file)) != EOF)
        code[real_file_size++] = (char)c;

    code[real_file_size] = '\0';

    struct token token_list[MAX_TOKENS];

    struct lexer lxr = {.c = code,
                        .loc = {.col = 1, .row = 1},
                        .state = NORMAL};


    size_t token_list_size = lex(&lxr, token_list);

    struct token pre_processed_tokens[MAX_TOKENS];

    size_t pp_token_list_size = pre_process(token_list,
                                             token_list_size,
                                             pre_processed_tokens);



    struct op op_list[pp_token_list_size];

    size_t op_list_size = parse(pre_processed_tokens, pp_token_list_size, op_list);

    // She bang for when lwsvm is in PATH
    fprintf(out_file, "#!lwsvm");

    for (int i = 0; i < op_list_size; i++)
    {
        write_op(out_file, &op_list[i]);
    }

    fclose(in_file);
    fclose(out_file);
}
