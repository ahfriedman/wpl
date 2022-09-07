/**
 * @file calculator_runtime.c
 * @author your name (you@domain.com)
 * @brief Runtime functions
 * @version 0.1
 * @date 2022-08-18
 */

#include <stdio.h>
#include <stdlib.h>

void print_expr(char *expr, int result){
  printf("Expression: '%s' evaluates to %d\n", expr, result);
}