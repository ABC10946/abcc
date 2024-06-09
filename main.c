#include "abcc.h"

#define TOKENVISUALIZE 0 // 1: トークンの表示、0: コード生成
#define NODEVISUALIZE 1  // 1: ノードの表示、0: コード生成

char *user_input;
Token *token;
labelId = 0;

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        error("引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);


    #if TOKENVISUALIZE
    print_token(token);
    #endif


    #if !TOKENVISUALIZE
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("\tpush rbp\n");
    printf("\tmov rbp, rsp\n");
    printf("\tsub rsp, 208\n");

    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("\tpop rax\n");
    }

    printf("\tmov rsp, rbp\n");
    printf("\tpop rbp\n");
    printf("\tret\n");

    #endif


    return 0;

}
