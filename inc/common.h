#define WHITESPACE_CHARS " \t\n"
enum imp
{
    STACK,
    ARITHMETIC,
    HEAP,
    FLOW,
    IO
};

enum op
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
    DONE,
    INVALID
};

struct token
{
    enum op op;
    int64_t num;
};

struct generator
{
    char* file_pointer;
    size_t op_index;
};

char generator_get_current(struct generator* generator)
{
    return *generator->file_pointer;
}

char generator_peek_next(struct generator* generator)
{
    return *generator->file_pointer + 1;
}

char generator_get_next(struct generator* generator)
{
    generator->op_index++;
    return *(++generator->file_pointer);
}

bool token_require_num(struct token* token)
{
    if (token->op == PUSH ||
        token->op == COPY ||
        token->op == SLIDE ||
        token->op == MARK ||
        token->op == CALL ||
        token->op == JMP ||
        token->op == JZ ||
        token->op == JN)
        return true;

    return false;
}
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#pragma GCC diagnostic ignored "-Wformat"

void print_token(struct token* token)
{
    switch (token->op)
    {
        case PUSH:
            printf("    PUSH %lld\n", token->num);
            break;
        case DUPE:
            printf("    DUPE\n");
            break;
        case COPY:
            printf("    COPY %lld\n", token->num);
            break;
        case SWAP:
            printf("    SWAP\n");
            break;
        case DROP:
            printf("    DROP\n");
            break;
        case SLIDE:
            printf("    SLIDE %lld\n", token->num);
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
            printf(".%llx:\n", token->num);
            break;
        case CALL:
            printf("    CALL .%llx\n", token->num);
            break;
        case JMP:
            printf("    JMP .%llx\n", token->num);
            break;
        case JZ:
            printf("    JZ .%llx\n", token->num);
            break;
        case JN:
            printf("    JN .%llx\n", token->num);
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
