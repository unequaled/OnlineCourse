#include <stdio.h>

int main(){
    char a[6] = {1,2,3,4,5,6};
    //char a1[6] = {1,2,0,0,0,0};
    int add = *(int*)a;
    
    printf("%x , %d.%d.%d.%d\n",add, a[0], a[1], a[2], a[3]);
    printf("%x.%x.%x.%x\n", (char)(add),(char)(add>>8) ,(char)(add>>16),(char)(add>>24));
    return 1;
}