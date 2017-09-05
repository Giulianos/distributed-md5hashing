#include <stdio.h>

int main(int argc, char * argv[])
{
  char msg[32];
  char * curr = msg;

  while((*curr=getchar()) && *curr!='\0') { curr++; }

  printf("Received: %.32s", msg);

  return 0;

}
