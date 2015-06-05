#include<stdio.h>
#include<stdlib.h>

class a{
    public:
    a(){printf("1");}   
    ~a(){printf("2");}
};

class b: a{
    public:
    b(){printf("3");}   
    ~b(){printf("4");}
};

int main(){
    
    b *temp = new b;
    delete temp;
    
}