#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  char bash_cmd[256] = "ls -la";
char buffer[1000];
FILE *pipe;
int len;

pipe = popen(bash_cmd, "r");

if (NULL == pipe) {
    perror("pipe");
    exit(1);
}
while(fgets(buffer, 1000, pipe) != NULL){
  len = strlen(buffer);
  printf("%s", buffer);

}

pclose(pipe);
  return 0;
}
