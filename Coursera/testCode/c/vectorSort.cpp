#include <algorithm>
#include <vector>
#include <stdio.h>

class entry{
    public:
        int id;
        short port;
        
        entry(int id, short port){
            this->id = id;
            this->port = port;
        };
        entry(int id){
            this->id = id;
            this->port = 0;
        };
        entry(): id(0),port(0) {}
        
};

bool sortid(entry& e1, entry& e2){ return e1.id<e2.id;}

int main(){
    
    std::vector<entry> list;
    list.insert(list.end(), entry(10));
    list.insert(list.end(), entry(2));
    list.insert(list.end(), entry(3));
    
    sort(list.begin(),list.end(), sortid);
    
    std::vector<entry>::iterator itr = list.begin();
    
    for(;itr != list.end();itr++){
       printf("%d ",itr->id);
    }
    
    
    return 0;
}