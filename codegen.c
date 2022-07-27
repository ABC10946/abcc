#include "abcc.h"

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
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");       // cmpで比較した結果2つの値が等しい場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_NEQ:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");      // cmpで比較した結果2つの値が等しくない場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");       // cmpで比較した結果左辺の値が小さい場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_LTE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");      // cmpで比較した結果左辺の値が小さいか等しい場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_GT:
        printf("    cmp rdi, rax\n");
        printf("    setl al\n");       // cmpで比較した結果左辺の値が大きい場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    case ND_GTE:
        printf("    cmp rdi, rax\n");
        printf("    setle al\n");      // cmpで比較した結果左辺の値が小さいか等しい場合ALに1をセットする
        printf("    movzb rax, al\n"); // raxの上位56bitを0埋めする
        break;
    }

    printf("	push rax\n");
}