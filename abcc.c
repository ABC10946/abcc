#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

char *user_input;

// プロトタイプ宣言
bool consume(char *);
void expect(char);
int expect_number();
bool at_eof();
Node *expr();
Node *mul();
Node *primary();
Node *unary();
Node *new_node_num(int val);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// トークンの種類を列挙型にする
typedef enum
{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数
    TK_EOF       // 終了
} TokenKind;

typedef struct Token Token;

// トークン構造体
struct Token
{
    TokenKind kind; // トークンの型 上で作った列挙型
    Token *next;    // 次のトークンへのポインタ
    int val;        // kindがTK_NUMのときの数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *primary()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_node_num(expect_number());
}

// mul = primary ( "*" primary| "/" primary)*
Node *mul() // トークンの長さ
{
    Node *node = unary();

    for (;;)
    {
        if (consume("*"))
        {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, node, unary());
        }
        else
        {
            return node;
        }
    }
}

// expr = mul ("+" mul | "-" mul)*
Node *expr()
{
    Node *node = mul();
    for (;;)
    {
        if (consume("+"))
        {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, node, mul());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume("+"))
    {
        return unary();
    }
    if (consume("-"))
    {
        return new_node(ND_SUB, new_node_num(0), unary());
    }

    return primary();
}

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("	push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("	pop rdi\n");
    printf("	pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("	add rax, rdi\n");
        break;
    case ND_SUB:
        printf("	sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("	imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("	cqo\n");
        printf("	idiv rdi\n");
        break;
    }

    printf("	push rax\n");
}

// 今見てるトークン
Token *token;

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/*
consume, expect, expect_number, at_eof
これらはトークンナイズ後に使われる関数
*/

bool consume(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) // トークンの種類が記号でない & opの長さがトークンの長さと一致しない & トークン文字列とop文字列がtoken->len個の範囲で一致しているか確認(memcmpは一致している場合0を返す)
        return false;

    token = token->next;
    return true;
}

void expect(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "'%c'ではありません", op);

    token = token->next;
}

int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");

    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (strchr("+-*/()", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // 文字列をlong値に変換 (今回の場合10進数) 変換不可能な文字までのポインタを第2引数のendptrに格納する。これにより複数桁数字をvalに格納する。
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
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
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    /*
    printf("	mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("	add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("	sub rax, %d\n", expect_number());
    }
    */

    printf("	pop rax\n");
    printf("	ret\n");
    return 0;
}
