#include "abcc.h"

typedef struct LVar LVar;

int is_alnum(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset;
};

Node *code[100];
LVar *find_lvar(Token *tok);

LVar *locals = NULL;
// calloc(1, sizeof(LVar));

Node *assign()
{
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *expr()
{
    return assign();
}


/*
stmt    = expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)? 
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
        | "return" expr ";"

*/
Node *stmt()
{
    Node *node;

    if (token->kind == TK_RETURN)
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        token = token->next;
        node->lhs = expr();
    }
    else if (token->kind == TK_WHILE) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        token = token->next;
        if (consume("("))
        {
            node->lhs = expr();
            expect(")");
            node->rhs = stmt();
            return node;
        }
    }
    else if (token->kind == TK_IF) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        token = token->next;
        if (consume("("))
        {
            node->lhs = expr();
            expect(")");
            node->rhs = stmt();
            if (token->kind != TK_EOF) {
                if (token->kind == TK_ELSE) {
                    token = token->next;
                    Node *tmpNode = node->rhs;
                    node->rhs = new_node(ND_ELSE, tmpNode, stmt());
                    return node;
                }
            }
        }
        return node;
    }
    else if (token->kind == TK_FOR) {
        // for (A; B; C) D
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR_HEAD;
        token = token->next;
        if (consume("("))
        {
            node->lhs = new_node(ND_FOR1, NULL, NULL);
            if (strncmp(token->str, ";", 1) != 0)
            { 
                node->lhs->lhs = expr(); // A
            }
            expect(";");
            node->lhs->rhs = new_node(ND_FOR2, NULL, NULL);
            if (strncmp(token->str, ";", 1) != 0)
            { 
                node->lhs->rhs->lhs = expr(); // B
            }
            expect(";");
            if (strncmp(token->str, ")", 1) != 0)
            { 
                node->lhs->rhs->rhs = expr(); // C
            }
            expect(")");
            node->rhs = stmt(); // D
        }
        return node;
    }
    else
    {
        node = expr();
    }

    if (!consume(";"))
        error_at(token->str, "';'ではないトークンです");
    return node;
}

void program()
{
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();

    code[i] = NULL;
}

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
    Token *tok = consume_ident();
    if (tok)
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar)
        {
            node->offset = lvar->offset;
        }
        else
        {
            // localsがNULLの場合構造体を0埋めする。
            if (locals == NULL)
            {
                locals = calloc(1, sizeof(LVar));
            }

            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }

        return node;
    }

    if (consume("("))
    {
        Node *node = expr();
        expect(")");
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

// // expr = mul ("+" mul | "-" mul)*
// Node *expr()
// {
//     equality();
// }

Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (consume("=="))
        {
            node = new_node(ND_EQ, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_NEQ, node, relational());
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();
    for (;;)
    {
        if (consume("<"))
        {
            node = new_node(ND_LT, node, add());
        }
        else if (consume("<="))
        {
            node = new_node(ND_LTE, node, add());
        }
        else if (consume(">"))
        {
            node = new_node(ND_GT, node, add());
        }
        else if (consume(">="))
        {
            node = new_node(ND_GTE, node, add());
        }
        else
        {
            return node;
        }
    }
}

Node *add()
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

Token *consume_ident()
{
    if (token->kind == TK_IDENT)
    {
        Token *t = token;
        token = token->next; // トークンを読み進める
        return t;
    }
    return NULL;
}

void expect(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);

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
        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p);
            p += 6;
            cur->len = 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
        {
            cur = new_token(TK_IF, cur, p);
            p += 2;
            cur->len = 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
        {
            cur = new_token(TK_ELSE, cur, p);
            p += 4;
            cur->len = 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5]))
        {
            cur = new_token(TK_WHILE, cur, p);
            p += 5;
            cur->len = 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
        {
            cur = new_token(TK_FOR, cur, p);
            p += 3;
            cur->len = 3;
            continue;
        }

        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0 || strncmp(p, "<=", 2) == 0 || strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p);
            p += 2;
            cur->len = 2;
            continue;
        }

        if (strchr("+-*/()<>=;", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if ('a' <= *p && *p <= 'z')
        {
            char *q = p;
            size_t len = 0;
            while (is_alnum(*p))
            {
                p++;
                len++;
            }
            char *s = (char *)calloc(1, sizeof(char) * len);
            strncpy(s, q, len);

            cur = new_token(TK_IDENT, cur, s);
            cur->len = len;
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

LVar *find_lvar(Token *tok)
{
    for (LVar *var = locals; var; var = var->next)
    {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}