#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "inc/limelib.h"
#include "inc/getline.h"
#include "inc/common.h"
#ifdef __linux__
    #include "inc/linux.h"
#endif

#define HEAP_SIZE 4096                  // 4k ints heap
#define STACK_SIZE 8181                 // 8k ints stack
#define MAX_PROGRAM_SIZE 1024

struct vm
{
    I64Stack* stack;
    I64Stack* call_stack;
    int64_t heap[HEAP_SIZE];
    struct token program[MAX_PROGRAM_SIZE];
    IntHashMap* jump_map;
    char* input;
    size_t ip;
    bool running;
};

size_t instruction_nr = 0;

int64_t bintoint_whitespace(const char* bin)
{
    char c = *bin;
    bool minus = c == ' ' ? false : true;

    int64_t num = 0;

    while ((c = *(++bin)) != '\0')
    {
        num = num << 1;
        if (c == '\t')
            num++;
    }

    return num = minus ? -num : num;
}

enum imp get_imp(struct generator* generator)
{
    char c = generator_get_current(generator);
    switch (c)
    {
        case ' ':
            return STACK;
        case '\n':
            return FLOW;
        case '\t':
            c = generator_get_next(generator);
            switch (c)
            {
                case ' ':
                    return ARITHMETIC;
                case '\n':
                    return IO;
                case '\t':
                    return HEAP;
                default:
                    printf("UNKNOWN CHAR");
                    exit(EXIT_FAILURE);
            }
        default:
            printf("UNKNOWN CHAR");
            exit(EXIT_FAILURE);
    }
}

enum op get_op(struct generator* generator, enum imp type)
{
    char c = generator_get_next(generator);
    switch (type)
    {
        case STACK:
            switch (c)
            {
                case ' ':
                    return PUSH;
                case '\n':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return DUPE;
                        case '\t':
                            return SWAP;
                        case '\n':
                            return DROP;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);
                    }
                case '\t':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return COPY;
                        case '\n':
                            return SLIDE;
                        default:
                            printf("Syntax error SPACE TAB %d", generator->op_index);
                            exit(EXIT_FAILURE);

                    }
                default:
                    printf("UNKNOWN CHAR");
                    exit(EXIT_FAILURE);
            }
        case ARITHMETIC:
            switch (c)
            {
                case ' ':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return ADD;
                        case '\t':
                            return SUB;
                        case '\n':
                            return MUL;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);
                    }
                case '\t':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return DIV;
                        case '\t':
                            return MOD;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);
                    }
                default:
                    printf("Syntax error TAB SPACE LF");
                    exit(EXIT_FAILURE);
            }
        case HEAP:
            switch (c)
            {
                case ' ':
                    return STORE;
                case '\t':
                    return FETCH;
                default:
                    printf("Syntax error TAB TAB LF");
                    exit(EXIT_FAILURE);
            }
        case FLOW:
            switch (c)
            {
                case ' ':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return MARK;
                        case '\t':
                            return CALL;
                        case '\n':
                            return JMP;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);

                    }
                case '\t':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return JZ;
                        case '\t':
                            return JN;
                        case '\n':
                            return RET;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);

                    }
                case '\n':
                    c = generator_get_next(generator);
                    if (c == '\n')
                        return END;

                    printf("UNKNOWN CHAR %d reached", c);
                    exit(EXIT_FAILURE);

                default:
                    printf("UNKNOWN CHAR");
                    exit(EXIT_FAILURE);
            }

        case IO:
            switch (c)
            {
                case ' ':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return PRINTC;
                        case '\t':
                            return PRINTI;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);

                    }
                case '\t':
                    c = generator_get_next(generator);
                    switch (c)
                    {
                        case ' ':
                            return READC;
                        case '\t':
                            return READI;
                        default:
                            printf("UNKNOWN CHAR");
                            exit(EXIT_FAILURE);

                    }
                default:
                    printf("UNKNOWN CHAR");
                    exit(EXIT_FAILURE);
            }
        default:
            printf("UNKNOWN IMP");
            exit(EXIT_FAILURE);
    }
}



int64_t get_num(struct generator* generator)
{
    char buffer[65];
    int i = 0;

    char c = generator_get_next(generator);

    for (; i < 64; i++)
    {
        if (c != '\n')
            buffer[i] = c;
        else
            break;

        c = generator_get_next(generator);
    }

    if (c != '\n')
        printf("NUMBER TO BIG on intsruction %d\n", instruction_nr);

    buffer[i++] = '\0';

    return bintoint_whitespace(buffer);

}

struct token gen_token(struct generator* generator)
{
    instruction_nr++;
    struct token token;
    enum imp imp = get_imp(generator);

    token.op = get_op(generator, imp);

    if (token_require_num(&token))
        token.num = get_num(generator);

    generator_get_next(generator);
    return token;

}


void run_vm(struct vm* vm)
{
    vm->running = true;

    struct token exec_token;

    OptionalInt opt;

    char buffer[20];

    int64_t a;
    int64_t b;

    int64_t value;
    int64_t index;

    size_t input_size;

    while (vm->running)
    {
        exec_token = vm->program[vm->ip];

        //if (vm->ip == 52)
        //{
        //    for (int i = 100; i < 130; i++)
        //        printf("%d:  %lld\n", i, vm->heap[i]);
        //    getchar();
        //}

        //print_token(&exec_token);
        switch (exec_token.op)
        {
            case PUSH:
                i64_stack_push(vm->stack, exec_token.num);
                break;
            case DUPE:
                i64_stack_dupe(vm->stack);
                break;
            case COPY:
                i64_stack_copy(vm->stack, exec_token.num);
                break;
            case SWAP:
                i64_stack_swap(vm->stack);
                break;
            case DROP:
                i64_stack_pop(vm->stack);
                break;
            case SLIDE:
                value = i64_stack_pop(vm->stack);
                for (int i = 0; i <= exec_token.num; i++)
                    i64_stack_pop(vm->stack);
                i64_stack_push(vm->stack, value);
                break;
            case ADD:
                b = i64_stack_pop(vm->stack);
                a = i64_stack_pop(vm->stack);
                i64_stack_push(vm->stack, a+b);
                break;
            case SUB:
                b = i64_stack_pop(vm->stack);
                a = i64_stack_pop(vm->stack);
                i64_stack_push(vm->stack, a-b);
                break;
            case MUL:
                b = i64_stack_pop(vm->stack);
                a = i64_stack_pop(vm->stack);
                i64_stack_push(vm->stack, a*b);
                break;
            case DIV:
                b = i64_stack_pop(vm->stack);
                a = i64_stack_pop(vm->stack);
                if (b <= 0)
                {
                    printf("[WSVM]: CANNOT DIV BY 0");
                    vm->running = false;
                    break;
                }
                i64_stack_push(vm->stack, a/b);
                break;
            case MOD:
                b = i64_stack_pop(vm->stack);
                a = i64_stack_pop(vm->stack);
                if (b <= 0)
                {
                    printf("[WSVM]: CANNOT MOD BY 0");
                    vm->running = false;
                    break;
                }
                i64_stack_push(vm->stack, a%b);
                break;
            case STORE:
                value = i64_stack_pop(vm->stack);
                index = i64_stack_pop(vm->stack);
                if (index >= HEAP_SIZE || index < 0)
                {
                    printf("[WSVM]: INDEX %d OUTSIDE OF HEAP", index);
                    vm->running = false;
                    break;
                }
                vm->heap[index] = value;
                break;
            case FETCH:
                index = i64_stack_pop(vm->stack);
                if (index >= HEAP_SIZE || index < 0)
                {
                    printf("[WSVM]: INDEX %d OUTSIDE OF HEAP", index);
                    vm->running = false;
                    break;
                }
                value = vm->heap[index];
                i64_stack_push(vm->stack, value);
                break;
            case MARK:
                break;
            case CALL:
                lltoa(exec_token.num, buffer, 16);
                opt = int_hashmap_get(vm->jump_map, buffer);
                if (!opt.has_value)
                {
                    printf("[WSVM]: CALLING WRONG LABEL");
                    vm->running = false;
                    break;
                }
                if (i64_stack_push(vm->call_stack, vm->ip))
                {
                    printf("[WSVM]: CALL-STACK OVERFLOW");
                    vm->running = false;
                }
                vm->ip = opt.value;
                continue;
            case JMP:
                lltoa(exec_token.num, buffer, 16);
                opt = int_hashmap_get(vm->jump_map, buffer);
                if (!opt.has_value)
                {
                    printf("[WSVM]: JUMPING WRONG LABEL");
                    vm->running = false;
                    break;
                }
                vm->ip = opt.value;
                continue;
            case JZ:
                lltoa(exec_token.num, buffer, 16);
                opt = int_hashmap_get(vm->jump_map, buffer);
                if (!opt.has_value)
                {
                    printf("[WSVM]: JUMPING WRONG LABEL");
                    vm->running = false;
                    break;
                }
                if (i64_stack_pop(vm->stack) == 0)
                {
                    vm->ip = opt.value;
                    continue;
                }
                break;
            case JN:
                itoa(exec_token.num, buffer, 16);
                opt = int_hashmap_get(vm->jump_map, buffer);
                if (!opt.has_value)
                {
                    printf("[WSVM]: JUMPING WRONG LABEL");
                    vm->running = false;
                    break;
                }
                if (i64_stack_pop(vm->stack) < 0)
                {
                    vm->ip = opt.value;
                    continue;
                }
                break;
            case RET:
                vm->ip = i64_stack_pop(vm->call_stack);
                break;
            case END:
                printf("\n[WSVM]: FINNISHED SUCCESSFULLY");
                vm->running = false;
                break;
            case PRINTC:
                putchar(i64_stack_pop(vm->stack));
                fflush(stdout);
                break;
            case PRINTI:
                printf("%lld", i64_stack_pop(vm->stack));
                break;
            case READC:
                index = i64_stack_pop(vm->stack);
                if (vm->input != NULL)
                {
                    vm->heap[index] = *vm->input;
                    vm->input++;
                    if (*vm->input == '\0')
                        vm->input = NULL;

                    break;
                }
                if (getline(&vm->input, &input_size, stdin) == -1)
                {
                    printf("[WSVM]: NO LINE");
                    vm->running = false;
                }
                vm->heap[index] = *vm->input;
                vm->input++;
                break;
            case READI:
                index = i64_stack_pop(vm->stack);
                if (getline(&vm->input, &input_size, stdin) == -1)
                {
                    printf("[WSVM]: NO LINE");
                    vm->running = false;
                }
                value = atoi(vm->input);
                vm->heap[index] = value;
                break;
            default:
                printf("[WSVM]: UNKNOWN OP %d\n", exec_token.op);
                break;
        }
        vm->ip++;
        //getchar();
    }
}



int main(int argc, char* argv[])
{
    struct vm vm = {.stack = i64_stack_create(STACK_SIZE),
                    .call_stack = i64_stack_create(20),
                    .jump_map = int_hashmap_create(),
                    .input = NULL,
                    .ip = 0
                    };



    if (argc < 2)
    {
        puts("File not provided");
        puts("Usage: ./lwsvm <file>");
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Could not open file %s\n", argv[1]);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char code[file_size];

    char c;
    int index = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if (strchr(WHITESPACE_CHARS, c))
            code[index++] = (char)c;
    }

    code[index] = '\0';

    struct generator generator = {.file_pointer = code, .op_index = 0};

    int i = 0;
    for (; i < MAX_PROGRAM_SIZE && generator_peek_next(&generator) > 1; i++)
    {
        struct token temp_token = gen_token(&generator);
        if (temp_token.op == MARK)
        {
            char buffer[20];
            lltoa(temp_token.num, buffer, 16);
            if (!int_hashmap_add(vm.jump_map, buffer, i))
                printf("[WST]: DUPLICATE LABLE\n");

            i--;
        }
        else
            vm.program[i] = temp_token;
        generator.op_index++;
    }
    /*
        for (int j = 0; j < i; j++)
        {
            printf("%d: ", j);
            print_token(&vm.program[j]);
        }
    */
    run_vm(&vm);

    i64_stack_destoy(vm.stack);
    i64_stack_destoy(vm.call_stack);

    return 0;
}
