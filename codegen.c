#include "abcc.h"

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR)
    {
        error("代入の左辺値が変数ではありません\n");
    }
    printf("\tmov rax, rbp\n");
    printf("\tsub rax, %d\n", node->offset);
    printf("\tpush rax\n");
}

void gen(Node *node)
{
    switch (node->kind)
    {
    case ND_NUM:
        printf("\tpush %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("\tpop rax\n");
        printf("\tmov rax, [rax]\n");
        printf("\tpush rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("\tpop rdi\n");
        printf("\tpop rax\n");
        printf("\tmov [rax], rdi\n");
        printf("\tpush rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("\tpop rax\n");
        printf("\tmov rsp, rbp\n");
        printf("\tpop rbp\n");
        printf("\tret\n");
        return;
    case ND_WHILE:
        labelId++;
        printf(".Lbegin%d:\n", labelId);
        gen(node->lhs);
        printf("\tpop rax\n");
        printf("\tcmp rax, 0\n");
        printf("\tje .Lend%d\n", labelId);
        gen(node->rhs);
        printf("\tjmp .Lbegin%d\n", labelId);
        printf(".Lend%d:\n", labelId);
        return;
    case ND_IF:
        labelId++;
        gen(node->lhs);
        printf("\tpop rax\n");
        printf("\tcmp rax, 0\n");
        if (node->rhs->kind != TK_ELSE) {
            printf("\tje .Lend%d\n", labelId);
            gen(node->rhs);
            printf(".Lend%d:\n", labelId);
            return;
        }
        printf("\tje .Lelse%d\n", labelId);
        gen(node->rhs->lhs);
        printf("\tjmp .Lend%d\n", labelId);
        printf(".Lelse%d:\n", labelId);
        gen(node->rhs->rhs);
        printf(".Lend%d:\n", labelId);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("\tpop rdi\n");
    printf("\tpop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("\tadd rax, rdi\n");
        break;
    case ND_SUB:
        printf("\tsub rax, rdi\n");
        break;
    case ND_MUL:
        printf("\timul rax, rdi\n");
        break;
    case ND_DIV:
        printf("\tcqo\n");
        printf("\tidiv rdi\n");
        break;
    case ND_EQ:
        printf("\tcmp rax, rdi\n");
        printf("\tsete al\n");       // cmpで比較した結果2つの値が等しい場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_NEQ:
        printf("\tcmp rax, rdi\n");
        printf("\tsetne al\n");      // cmpで比較した結果2つの値が等しくない場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_LT:
        printf("\tcmp rax, rdi\n");
        printf("\tsetl al\n");       // cmpで比較した結果左辺の値が小さい場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_LTE:
        printf("\tcmp rax, rdi\n");
        printf("\tsetle al\n");      // cmpで比較した結果左辺の値が小さいか等しい場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_GT:
        printf("\tcmp rdi, rax\n");
        printf("\tsetl al\n");       // cmpで比較した結果左辺の値が大きい場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_GTE:
        printf("\tcmp rdi, rax\n");
        printf("\tsetle al\n");      // cmpで比較した結果左辺の値が小さいか等しい場合ALに1をセットする
        printf("\tmovzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    }

    printf("\tpush rax\n");
}