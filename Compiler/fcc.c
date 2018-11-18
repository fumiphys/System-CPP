/*
 * simple compiler
 */

#include <stdio.h>


int main(int argc, char const* argv[])
{
  int ret;
  scanf("%d", &ret);
  printf(".intel_syntax_noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");
  printf("  mov rax, %d\n", ret);
  printf("  ret\n");
  return 0;
}
