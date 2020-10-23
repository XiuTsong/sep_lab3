#include "termio.h"
#include<string>
//#include "stack.h"

char Termio::buffer[Termio::CANVAS_HEIGHT][Termio::CANVAS_WIDTH + 1];

template <class T>
class stack{
private:
    int _size ;
    int _capacity ;
    int number ;
    T* data ;  //0-base
    void doubleSpace(){
        if(_size >= _capacity - 1){
            T* old_data = new T [_capacity] ;
            for(int i = 0 ; i < _capacity ; ++i){
                old_data[i] = data[i] ;
            }
            delete data ;
            _capacity *= 2 ;
            data = new T [_capacity] ;
            for(int i = 0 ; i < _capacity ; ++i){
                data[i] = old_data[i] ;
            }
        }
    }
public:
    stack():_size(0) , _capacity(10) , number(0){
        data = new T [_capacity] ;
    }
    stack(const stack &obj){
        _size = obj._size ;
        number = obj.number ;
        data = new T [_capacity = obj._capacity] ;
        for(int i = 0 ; i < _size ; ++i)
            data[i] = obj.data[i] ;
    }
    ~stack(){ delete data ;}
    void push(T e){
        doubleSpace() ;
        data[_size++] = e ;
    }
    T pop(){
        return data[--_size] ;
    }
    T & top(){
        return data[_size - 1] ;
    }
    bool empty() const { return !_size ;}
    int size() const { return _size ;}
    int & num() { return number ;}
    T & operator[] (int idx){
        return data[idx] ;
    }
    const T operator[] (int idx) const {
        return data[idx] ;
    }
    void clear(){
        delete [] data ;
        data = new T [_capacity = 10] ;
        _size = 0 ;
    }
};

void write_buffer(stack<int> a , stack<int> b , stack<int> c){
    for(int i = 0 ; i < 11; ++i){
        Termio::buffer[i][5] = '|';
        Termio::buffer[i][20] = '|';
        Termio::buffer[i][35] = '|';
    }
    for(int j = 0 ; j < Termio::CANVAS_WIDTH ; ++j){
        if(j != 5 && j != 20 && j != 35)
            Termio::buffer[10][j] = '-' ;
    }
    for(int i = 0 ; i < a.size() ; ++i){
        int i0 = 9 - 2 * i ;
        Termio::buffer[i0][5] = '*' ;
        for(int j = 1 ; j <= a[i] / 2 ; ++j){
            Termio::buffer[i0][5 + j] = '*' ;
            Termio::buffer[i0][5 - j] = '*' ;
        }
    }
    for(int i = 0 ; i < b.size() ; ++i){
        int i0 = 9 - 2 * i ;
        Termio::buffer[i0][20] = '*' ;
        for(int j = 1 ; j <= b[i] / 2 ; ++j){
            Termio::buffer[i0][20 + j] = '*' ;
            Termio::buffer[i0][20 - j] = '*' ;
        }
    }
    for(int i = 0 ; i < c.size() ; ++i){
        int i0 = 9 - 2 * i ;
        Termio::buffer[i0][35] = '*' ;
        for(int j = 1 ; j <= c[i] / 2 ; ++j){
            Termio::buffer[i0][35 + j] = '*' ;
            Termio::buffer[i0][35 - j] = '*' ;
        }
    }
}

bool pop_push(stack<int> &A , stack<int> &B){
    //std::cout << A.empty() << " " << A.top() << " " << B.top() << std::endl ;
    if((B.empty() & !A.empty()) || (!A.empty() && (&A != &B) && A.top() < B.top())){
        B.push(A.pop()) ;
        //std::cout << "B = "<<B.top() << std::endl ;
        return true ;
    }
    else return false ;
}


struct movePair{
    int x ;
    int y ;
    movePair(): x(0), y(0){}
    movePair(int f , int s):x(f) , y(s){}
    int & first() { return x ;}
    int & second() { return y ;}
    void print(){
        std::cout << "Auto moving:" <<x << "->" << y << '\n';
    }
};

stack<int>& match(int n , stack<int> &A, stack<int> &B, stack<int> &C){
    if(n == 1) return A ;
    else if(n == 2) return B ;
    else return C ;
}

stack<int>& match2(int n , stack<int> &A, stack<int> &B, stack<int> &C){
    if (A.num() == n) return A ;
    if (B.num() == n) return B ;
    if (C.num() == n) return C ;
}


void initialPhase(stack<int> &A , stack<int> &B ,stack<int> &C ,int num){
    A.clear() ;
    B.clear() ;
    C.clear() ;
    int* data = new int [num] ;
    for(int i = 0 ; i < num ; ++i){
        data[i] = (i + 1) * 2 + 1 ;
    }
    for(int i = num - 1 ; i >= 0 ; --i){
        A.push(data[i]) ;
    }
    delete [] data  ;
    Termio::ResetBuffer();
    write_buffer(A , B , C) ;
    Termio::Draw();
}

void hanoiSolve(int n , stack<int> &A , stack<int> &B , stack<int> &C){
    if(n == 1){
        pop_push(A , B) ;
        movePair tmp(A.num() , B.num()) ;
        tmp.print() ;
        Termio::ResetBuffer();
        write_buffer(match2(1 , A , B , C) , match2(2 , A , B , C) , match2(3 , A , B , C)) ;
        Termio::Draw();
    }
    else{
        hanoiSolve(n - 1 , A , C , B) ;
        pop_push(A , B) ;
        movePair tmp(A.num() , B.num()) ;
        tmp.print() ;
        Termio::ResetBuffer();
        write_buffer(match2(1 , A , B , C) , match2(2 , A , B , C) , match2(3 , A , B , C)) ;
        Termio::Draw();
        hanoiSolve(n - 1 , C , B , A) ;
    }
}

void autoPlayMode(int num , stack<int> &A , stack<int> &B ,stack<int> &C , stack<movePair> & memoryStack){
    while(!memoryStack.empty()){
        movePair tmp = memoryStack.pop() ;
        pop_push(match(tmp.first() , A , B , C) , match(tmp.second() , A , B , C)) ;
        tmp.print() ;
        Termio::ResetBuffer() ;
        write_buffer(A , B , C) ;
        Termio::Draw();
    }
    hanoiSolve(num , A , B , C) ;
}

void run(){
    stack <int> A , B , C ;
    A.num() = 1 ;
    B.num() = 2 ;
    C.num() = 3 ;
    Termio::Clear();
    while(true){
        std::cout << "How many disks do you want? (1 ~ 5)" << std::endl;
        char cmd ;
        std::string s ;
        std:: cin >> s ;
        if (s.length() != 1) continue;
        cmd = s[0];
        if(cmd == 'Q') break ;
        if(cmd >= '1' && cmd <= '5'){
            int num = cmd - '0' ;
            //std::cout << "num = " << num << std::endl;
            initialPhase(A , B , C , num) ;
            stack <movePair> memoryStack ;
            while(true){
                std::cout << "Move a disk. Format: x y" << std::endl;
                std::string f , t ;
                char from , to ;
                bool flag ;
                cin >> f ;
                if(f.length() != 1) continue;
                from = f[0] ;
                if(from == 'Q') break ;
                cin >> t ;
                if(t.length() != 1) continue ;
                to = t[0] ;
                if(from == '0' && to == '0'){
                    autoPlayMode(num , A , B , C , memoryStack) ;
                }
                else if(from < '1' || from > '3' || to < '1' || to > '3'){
                    Termio::Draw();
                }
                else{
                    //std::cout << from - '0' << " " << to - '0'  << std::endl ;
                    flag = pop_push(match(from - '0' , A , B , C), match(to - '0' , A , B ,C)) ;
                    //std :: cout << flag << std :: endl ;
                    if(flag){
                        movePair p(to - '0' , from - '0') ;
                        memoryStack.push(p) ;
                    }
                    Termio::ResetBuffer() ;
                    write_buffer(A , B , C) ;
                    //std::cout << A.size() << " " << B.size() << std::endl ;
                    Termio::Draw();
                }
                if(B.size() == num){
                    std::cout << "Congratulations! You win!" << std::endl ;
                    break ;
                }
            }
        }
    }
}

int main() {
    run() ;
    return 0;
}
