#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
    int x;
    printf("argc:%d\n", argc);
    for (x=0;x<argc;x++){
        printf("%d:%s\n", x, argv[x]);
    }
    return 0;
}