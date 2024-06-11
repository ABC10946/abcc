#include "abcc.h"

void stack_init(Stack *stack) {
    memset(stack, 0, sizeof(Stack));
}

void stack_push_with_inc(Stack *stack) {
    int val = stack->stack_array[stack->stack_num];
    val++;
    stack_push(stack, val);
}

void stack_push(Stack *stack, int id) {
    stack->stack_num++;
    stack->stack_array[stack->stack_num] = id;
}

int stack_pop(Stack *stack) {
    int val = stack->stack_array[stack->stack_num];
    if (stack->stack_num > 0) {
        stack->stack_num--;
    }
    return val;
}

int stack_get_top(Stack *stack) {
    return stack->stack_array[stack->stack_num];
}