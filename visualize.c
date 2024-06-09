#include <stdio.h>
#include "abcc.h"


void print_token(Token *tok)
{
    Token *cur = tok;
    while (cur)
    {
        // tokentext = cur->str[0:cur->len]
        char *tokentext = malloc(cur->len + 1);
        strncpy(tokentext, cur->str, cur->len);

        char *tokenKindText;

        switch(cur->kind) {
            case TK_RESERVED:
                tokenKindText = "TK_RESERVED";
                break;
            case TK_IDENT:
                tokenKindText = "TK_IDENT";
                break;
            case TK_NUM:
                tokenKindText = "TK_NUM";
                break;
            case TK_EOF:
                tokenKindText = "TK_EOF";
                break;
            case TK_RETURN:
                tokenKindText = "TK_RETURN";
                break;
            case TK_IF:
                tokenKindText = "TK_IF";
                break;
            case TK_ELSE:
                tokenKindText = "TK_ELSE";
                break;
            case TK_WHILE:
                tokenKindText = "TK_WHILE";
                break;
            case TK_FOR:
                tokenKindText = "TK_FOR";
                break;
            default:
                tokenKindText = "UNKNOWN";
                break;
        }
        if (cur->kind == TK_NUM) {
            printf("kind: %s, len: %d,  val: %d\n", tokenKindText, cur->len, cur->val);

        } else {
            printf("kind: %s, len: %d,  str: %s\n", tokenKindText, cur->len, tokentext);
        }
        cur = cur->next;
    }
}