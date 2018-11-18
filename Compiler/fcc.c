/*
 * simple compiler
 */

#include <stdio.h>


int main(int argc, char const* argv[])
{
  char exp[256];
  scanf("%s", exp);
  char *s = exp;
  printf(".intel_syntax_noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");
  printf("  mov rax, %ld\n", strtol(s, &s, 10));
  while(*s){
    if(*s == '+'){
      s++;
      printf("  add rax, %ld\n", strtol(s, &s, 10));
    }else if(*s == '-'){
      s++;
      printf("  sub rax, %ld\n", strtol(s, &s, 10));
    }else{
      fprintf(stderr, "undefined char sequence\n");
      return 1;
    }
  }
  printf("  ret\n");
  return 0;
}
