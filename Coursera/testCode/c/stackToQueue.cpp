#include <stdio.h>
#include <stack>

template <class T>
class foo{
  private:
    T temp;
    std::stack<T> st,st_out;
    
  public:
    foo() {}
    virtual ~foo() {}
    void push(T input) {st.push(input);}
    T top() {
        while(!st.empty()){
            st_out.push(st.top());
            st.pop();
        }
        temp=st_out.top();
        while(!st_out.empty()){
            st.push(st_out.top());
            st_out.pop();
        }
        return temp;
    }
    void pop() {
        while(!st.empty()){
            st_out.push(st.top());
            st.pop();
        }
        temp=st_out.top();
        st_out.pop();
        while(!st_out.empty()){
            st.push(st_out.top());
            st_out.pop();
        }
    }
    
};

int main(){
    
    foo<int> f;
    
    for(int x=0;x<100;x++)
        f.push(x);
    
    for(int x=0;x<10;x++){
        printf("%d\n",f.top());
        f.pop();
    }
    return 1;
}