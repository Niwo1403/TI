#include <stdio.h>

void rmLast(char *, char *);

int main(int argc, char *argv[]){
  if (argc < 3)
    return 1;
  return rmLast(argv[1], argv[2]);
}

int rmLast(char *srcp, char *destp){
  FILE *src = fopen(srcp, "r"), *dest = fopen(destp, "w");
  if(src==NULL || dest==NULL)
    return 1;
  char bin[2];
  fread(bin, 1, 2, src);

  char c;
  while (fread(&c, 1, 1, src) > 0){
    fwrite(bin, 1, 1, dest);
    bin[0] = bin[1];
    bin[1] = c;
  }

  fclose(src);
  fclose(dest);

  return 0;
}
