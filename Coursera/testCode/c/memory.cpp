#include <stdio.h>
#include <stdlib.h>

int main(){
    int *p = new int(100);
    int *p1 = new int[100];
    int *p2 = (int *) malloc(sizeof(int)*100);
    
    printf("%lu %lu\n", sizeof(p), sizeof(p1));
    printf("%d %d\n", *p, *p1);
    printf("%d %d\n", *p, p1[1]);
    
    
    delete p;
    delete [] p1;
    free(p2);
}