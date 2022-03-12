//
// Created by Runze Wang on 2021/9/22.
//

#ifndef CS_DSA_FALL_2021_STACK_QUEUE_H
#define CS_DSA_FALL_2021_STACK_QUEUE_H
#define DEFAULT_CAPACITY 32
#include "typeinfo.h"
template<typename T>
class Stack{
protected:
    int _size;  //_elem[Rank r] 可以直接代表[0,_size)之间的元素，_size既可以反应处当前元素个数，也可以用来判断是否溢出
    int _capacity;//_capacity用来判断整体的容量
    T * _elem;// _elem[lo,hi)中，hi-lo可以直接表示出数组的元素个数，同时_elem取值范围也规定为lo到hi-1；当r<=hi时，就代表了越界
    void copyFrom(T *const A, int lo, int hi);//从A的[lo,hi)位置复制给_elem指定的位置

    void expand();//可扩充向量空间

    void shrink();//可缩小向量空间
public:
    Stack():_size(0),_capacity(DEFAULT_CAPACITY){
        _elem=new T[_capacity];
    }
    Stack(Stack<T> const & S){
        copyFrom(S._elem,0,S._size);
    }
    ~Stack(){
        delete [] _elem;
    }

    int size() const { return _size; } //规模
    bool empty() const { return !_size; } //判空

    T pop(){
        _size--;
        T t=_elem[_size];
        shrink();
        return t;
    }
    T &top(){
        return _elem[_size-1];
    }

    void push(T & e){
        expand();
        _elem[_size]=e;
        _size++;
    }
    Stack<T>& reverse();//翻转

    Stack<T>& operator=(const Stack<T> & s ) ;
};

template<typename T>
void Stack<T>::copyFrom(T *const A, int lo, int hi) {
    _elem = new T{_capacity = 2 * (hi - lo)};
    _size = 0;
    while (lo < hi)
        _elem[_size++] = A[lo++];
}

template<typename T>
void Stack<T>::expand() {
    if (_size<_capacity)
        return;
    else{
        T * oldElem=_elem;
        _capacity*=2;
        _elem=new T[_capacity];
        for (int i = 0; i < _size; ++i) {
            _elem[i]=oldElem[i];
            release(oldElem[i]);
        }
        delete[]oldElem;
    }
}

template<typename T>//O（n）大量分配内存的开销，不应该频繁使用
void Stack<T>::shrink() {
    if (_capacity<=DEFAULT_CAPACITY ||_size >= _capacity / 2)
        return;
    else {
        T *oldElem = _elem;
        _capacity/=2;
        _elem = new T[_capacity];
        for (int i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
            release(oldElem[i]);
        }
        delete[]oldElem;
    }
}

template<typename T>
Stack<T> &Stack<T>::reverse() {//O(n)
    for (int i = 0; i < _size/2; ++i) {
        int temp=_elem[i];
        _elem[i]=_elem[_size-i-1];
        _elem[_size-i-1]=temp;
    }
    return *this;
}

template <typename T>
Stack<T> &Stack<T>::operator=(const Stack<T> &s) {
    delete []_elem;
    copyFrom(s._elem,0,s.size());
}
template<typename T>
class Queue {
    struct Node {
        T item;
        struct Node *next;
    };
    Node *head;
    Node *rear;
    int _size;
public:
    Queue() : head(nullptr), rear(nullptr), _size(0) {};

    Queue(const Queue<T> &q);

    ~Queue();

    bool empty() const { return !_size ;}

    int queueCount() const { return _size ;}

    void push_back(const T &x);

    void pop_front();

    T &front(){return head->item;}

    T &back(){return rear->item;}

    Queue<T> & operator=(const Queue<T> & q);
};


template<typename T>
Queue<T>::Queue (const Queue<T> &q):_size(q._size) {
    int i = 0;
    Node *temp = q.head;
    while (i++ < _size) {
        push_back(temp->item);
        temp=temp->next;
    }
}


template<typename T>
Queue<T>::~Queue<T>() {
    Node *temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

template<typename T>
void Queue<T>::push_back(const T &x) {
    Node *add = new Node{x, nullptr};
    _size++;
    if (head == nullptr) {
        head = add;
    }
    else
        rear->next = add;
    rear = add;
}

template<typename T>
void Queue<T>::pop_front() {
    if (empty())
        return ;
    _size--;
    Node *temp = head;
    head = head->next;
    delete temp;
    if (empty())
        rear = nullptr;
}

template<typename T>
Queue<T> &Queue<T>::operator=(const Queue<T> &q) {
    if(this==&q)
        return *this;
    Node * temp;
    while (!empty()) {
        temp = head;
        head = head->next;
        delete temp;
        _size--;
    }
    rear= nullptr;
    temp=q.head;
    while (temp){
        push_back(temp->item);
        temp=temp->next;
    }
    return *this;
}
#undef DEFAULT_CAPACITY
#endif //CS_DSA_FALL_2021_STACK_QUEUE_H
