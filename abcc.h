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
    ND_NUM,
    ND_EQ,     // equal ==
    ND_NEQ,    // not equal !=
    ND_LTE,    // less than or equal <=
    ND_GTE,    // greater than or equal >=
    ND_LT,     // less than <
    ND_GT,     // greater than >
    ND_ASSIGN, // 代入
    ND_LVAR,   // ローカル変数
    ND_RETURN,
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

// トークンの種類を列挙型にする
typedef enum
{
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数
    TK_EOF,      // 終了
    TK_RETURN,
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

extern char *user_input;
// 今見てるトークン
extern Token *token;
extern Node *code[100];

// プロトタイプ宣言
bool consume(char *);
void expect(char *);
int expect_number();
bool at_eof();
Node *expr();
Node *mul();
Node *primary();
Node *unary();
Node *equality();
Node *relational();
Node *add();
Node *new_node_num(int val);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
void gen(Node *node);
Token *tokenize(char *p);
Token *consume_ident();
void program();