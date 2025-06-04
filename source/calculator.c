#include "calculator.h"

int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

int multiply(int a, int b)
{
    return a * b;
}

int divide(int a, int b)
{
    if (b == 0)
    {
        return 0;
    }
    return a / b;
}

int char_to_int(char ch)
{
    return ch - '0';
}

int calculator_interface(int a, char op, int b)
{
    switch(op)
    {
        case '+': return add(a, b);
        case '-': return subtract(a, b);
        case '*': return multiply(a, b);
        case '/': return divide(a, b);
        default: return 0;
    }
}