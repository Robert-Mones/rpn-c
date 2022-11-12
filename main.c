#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct datastack datastack;
struct datastack {
    float* stack;
    uint32_t size;
    uint32_t max;
};

bool isIntlike(float x) {
    return true;
}

void push(datastack* stack, float val) {
    if(stack->size == stack->max) {
        float* tmp = calloc(2 * (stack->max), sizeof(float));
        memcpy(tmp, stack->stack, (stack->max) * sizeof(float));
        stack->max = 2 * (stack->max);

        free(stack->stack);
        stack->stack = tmp;
    }

    (stack->stack)[stack->size] = val;
    (stack->size)++;
}

float pop(datastack* stack) {
    float val = (stack->stack)[(stack->size) - 1];
    (stack->size)--;
    
    return val;
}

uint32_t renderNumber(float x, char* buf, uint32_t width) {
    char fmt[20];
    sprintf(fmt, "%%%df", width);
    return sprintf(buf, fmt, x);
}

void render(datastack* stack) {
    struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    
    for(int i = 0; i < win.ws_col; i++) printf("-");
    printf("\n");

    for(int i = win.ws_row-5; i >= 0; i--) {
        printf("|");
        if(i < stack->size) {
            char pnum[win.ws_col+1];
            renderNumber((stack->stack)[(stack->size) - i - 1], pnum, win.ws_col-2);

            printf("%s", pnum);
        } else {
            for(int j = 0; j < win.ws_col-2; j++) printf(" ");
        }
        printf("|\n");
    }

    for(int i = 0; i < win.ws_col; i++) printf("-");
    printf("\n");
}

bool operate(datastack* stack, char op) {
    float x,y;

    switch(op) {
        case '+':
            if(stack->size < 2) {
                printf("Error: not enough operands");
                break;
            }
            y = pop(stack);
            x = pop(stack);
            push(stack, x + y);
            break;
        case '-':
            if(stack->size < 2) {
                printf("Error: not enough operands");
                break;
            }
            y = pop(stack);
            x = pop(stack);
            push(stack, x - y);
            break;
        case '*':
            if(stack->size < 2) {
                printf("Error: not enough operands");
                break;
            }
            y = pop(stack);
            x = pop(stack);
            push(stack, x * y);
            break;
        case '/':
            if(stack->size < 2) {
                printf("Error: not enough operands");
                break;
            }
            y = pop(stack);
            x = pop(stack);
            push(stack, x / y);
            break;
        case 'd':
            if(stack->size < 1) {
                printf("Error: not enough operands");
                break;
            }
            pop(stack);
            break;
        case 'c':
            while(stack->size > 0) pop(stack);
            break;
        case 'q':
            return true;
        default:
            printf("Error: unknown operator '%c'", op);
    }

    return false;
}

bool input(datastack* stack) {
    char inp[64];
    scanf("%s", inp);

    float x;
    char c;
    if(sscanf(inp, "%f", &x)) {
        push(stack, x);
    } else if(sscanf(inp, "%c", &c)) {
        if(operate(stack, c)) return true;
    } else {
        printf("Error: failed to parse '%s'", inp);
    }

    return false;
}

int main() {
    datastack* stack = malloc(sizeof(datastack));
    stack->size = 0;
    stack->max = 8;
    stack->stack = calloc(stack->max, sizeof(float));

    while(true) {
        render(stack);
        if(input(stack)) break;
    }

    return 0;
}