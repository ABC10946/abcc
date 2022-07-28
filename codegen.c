#include "abcc.h"

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("\tpush %d\n", node->val);
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