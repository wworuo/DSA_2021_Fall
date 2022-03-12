//
// Created by Runze Wang on 2021/11/17.
//

#ifndef CS_DSA_FALL_2021_LIST_H
#define CS_DSA_FALL_2021_LIST_H


#define Posi(T) ListNode<T>*
#include "typeinfo.h"
template<typename T>
struct ListNode {
    T data=0;//数值
    Posi(T)pred=0;//前驱
    Posi(T)succ=0;//后继

    ListNode() {}

    ListNode(T const& e, Posi(T)p = nullptr, Posi(T)s = nullptr)
            : data(e), pred(p), succ(s) {}

    Posi(T)insertAsPred(T const &e);//前插入
    Posi(T)insertAsSucc(T const &e);//后插入
};

template<typename T>
Posi(T)ListNode<T>::insertAsPred(T const &e) {
    Posi(T)x = new ListNode(e, pred, this);
    pred->succ = x;
    pred = x;
    return x;
}

template<typename T>
Posi(T)ListNode<T>::insertAsSucc(T const &e) {
    Posi(T)x = new ListNode(e, this, succ);
    succ->pred = x;
    succ = x;
    return x;
}

typedef int Rank;


template<typename T>
class List {
private:
    int _size;
    Posi(T)header;
    Posi(T)trailer;

protected:
    void init();

    int clean();

    void copyNodes(Posi(T)p, int n);//从p位置开始往后复制n个元素（包含p）,重新建立头尾节点

    void swap(T &a, T &b) {
        T t;
        t = a;
        a = b;
        b = t;
    }

    void selectionSort(Posi(T)p, int n);//从p起往后[p,p+n) n个数排序，复杂度来自与比较，为O(n^2)
    void insertSort(Posi(T)p, int n);//从p起往后[p,p+n) n个数排序, 复杂度来自于比较，O(n)~O(n^2),平均为O(n^2)
    void mergeSort(Posi(T)&p, int n);//列表的归并排序算法：对起始于位置p的n个元素排序
    Posi(T)merge(Posi(T)p, int n, List<T> &L, Posi(T)q, int m);//有序列表的归并：当前列表中自p起的n个元素，与列表L中自q起的m个元素归并
public:
    List() { //默认
        init();
    }

    List(List<T> const &L) {//拷贝构造
        copyNodes(L.first(), L._size);
    }

    List(List<T> const &L, Rank r, int n) {//复制列表L中自第r项起的n项
        Posi(T)p = L.first();
        while (r--)
            p = p->succ;
        copyNodes(p, n - r);
    }

    List(Posi(T)p, int n) {//复制列表中自位置p起的n项
        copyNodes(p, n);
    }

    ~List() {//析构函数
        clean();
    }

    Rank size() const { return _size; } //规模
    bool empty() const { return _size <= 0; } //判空
    bool valid(Posi(T)p) //判断位置p是否对外合法
    { return p && (trailer != p) && (header != p); } //将头、尾节点等同于NULL
    T &operator[](Rank r) const; //O(n) 重载，支持循秩访问（效率低）
    List<T>&operator=(List<T> const & L){
        if(this==&L)
            return *this;
        clean();
        init();
        copyNodes(L.first(), L._size);
        return *this;
    }

    Posi(T)first() const { return header->succ; } //首节点位置
    Posi(T)last() const { return trailer->pred; } //末节点位置
    Posi(T)head()const{return header;}

    Posi(T)trail()const{return trailer;}
    Posi(T)find(T const &e) const { return find(e, _size, trailer); }//无序列表查找
    Posi(T)search(T const &e) const //有序列表查找
    { return search(e, _size, trailer); }

    Posi(T)find(T const &e, int n, Posi(T)p) const;//从p位置出发(不包含p)向前查找,无序查找，没有找到则返回NULL
    Posi(T)search(T const &e, int n, Posi(T)p) const;//从p位置出发(不包含p)向前查找不大于e的最后者,有序查找
    Posi(T)insertBefore(T const &e, Posi(T)p);//在p位置之前插入一个元素，返回新节点的位置,size增加
    Posi(T)insertAfter(Posi(T)p, T const &e);//在p位置之后插入一个元素，返回新节点的位置,size增加
    Posi(T)insertAsFirst(T const &e);
    Posi(T)insertAsLast(T const &e);

    T remove(Posi(T)p); //删除合法位置p处的节点,返回被删除节点的数值,size减少
    void merge(List<T> &L) { merge(header->succ, _size, L, L.header->succ, L._size); } //全列表归并
    Posi(T)selectMax() { return selectMax(header->succ, _size); } //整体最大者
    Posi(T)selectMax(Posi(T)p, int n);//从p起往后[p,p+n) n个数选择最大的且最末端的数的位置

    void sort(){ selectionSort(header->succ,size());}

};


template<typename T>
void List<T>::init() {
    header = new ListNode<T>;
    trailer = new ListNode<T>;
    header->succ = trailer;
    header->pred = nullptr;
    trailer->pred = header;
    trailer->succ = nullptr;
    _size = 0;
}

template<typename T>
int List<T>::clean() {
    int oldSize = _size;
    while (0 < _size)
        remove(first());
    return oldSize;
}

template<typename T>
void List<T>::copyNodes(ListNode<T> *p, int n) {
    init();
    while (n--) {
        insertAsLast(p->data);
        p = p->succ;
    }
}

template<typename T>
T &List<T>::operator[](Rank r) const {
    Posi(T)p = first();
    while (0 < r--) p = p->succ;
    return p->data;
}

template<typename T>
Posi(T)List<T>::find(const T &e, int n, ListNode<T> *p) const {
    while (0 < n--) {//进行n次查找,从[0,n)
        if (e == ((p = p->pred)->data)) return p;
    }
    return nullptr;
}

template<typename T>
Posi(T)List<T>::search(const T &e, int n, ListNode<T> *p) const {
    while (0 < n--) {
        p = p->pred;
        if (p->data <= e) break;
    }
    return p;
}

template<typename T>
Posi(T)List<T>::insertBefore(const T &e, ListNode<T> *p) {
    _size++;
    return p->insertAsPred(e);
}

template<typename T>
Posi(T)List<T>::insertAfter(ListNode<T> *p, const T &e) {
    _size++;
    return p->insertAsSucc(e);
}

template<typename T>
Posi(T)List<T>::insertAsFirst(const T &e) {
    return insertAfter(header, e);
}

template<typename T>
Posi(T)List<T>::insertAsLast(const T &e) {
    return insertBefore(e, trailer);
}

template<typename T>
T List<T>::remove(ListNode<T> *p) {
    T e = p->data;
    p->pred->succ = p->succ;
    p->succ->pred = p->pred;
    delete p;
    _size--;
    return e;
}



template<typename T>
void List<T>::selectionSort(ListNode<T> *p, int n) {
    Posi(T)head = p->pred;
    Posi(T)tail = p;
    for (int i = 0; i < n; ++i)
        tail = tail->succ;
    while (1 < n) {
        Posi(T)q = selectMax(head->succ, n);
        swap(q->data, tail->pred->data);
        tail = tail->pred;
        n--;
    }
}

template<typename T>
ListNode<T> *List<T>::selectMax(ListNode<T> *p, int n) {
    Posi(T)max = p;
    for (Posi(T)cur = p; 1 < n; n--) {
        cur = cur->succ;
        if (cur->data >= max->data)
            max = cur;
    }
    return max;
}

template<typename T>
void List<T>::insertSort(ListNode<T> *p, int n) {
    for (int r = 0; r < n; ++r) {
        Posi(T)q = search(p->data, r, p);//search是寻找p之前r个数中与不大于p->data值的位置
        insertAfter(q, p->data);
        p = p->succ;
        remove(p->pred);
    }
}

template<typename T>
void List<T>::mergeSort(Posi(T)&p, int n) { //valid(p) && rank(p) + n <= size
    if (n < 2) return; //若待排序范围已足够小，则直接返回；否则...
    int m = n >> 1; //以中点为界
    Posi(T)q = p;
    for (int i = 0; i < m; i++) q = q->succ; //找到后子列表起点
    mergeSort(p, m);
    mergeSort(q, n - m); //前、后子列表各分别排序
    p = merge(p, m, *this, q, n - m); //归并
} //注意：排序后，p依然指向归并后区间的（新）起点

template<typename T>
ListNode<T> *List<T>::merge(ListNode<T> *p, int n, List<T> &L, ListNode<T> *q, int m) {
// assert:  this.valid(p) && rank(p) + n <= size && this.sorted(p, n)
//          L.valid(q) && rank(q) + m <= L._size && L.sorted(q, m)
// 注意：在被mergeSort()调用时，this == &L && rank(p) + n = rank(q)
    ListNode<T> *pp = p->pred; //归并之后p可能不再指向首节点，故需先记忆，以便在返回前更新
    while ((0 < m) && (q != p)) //q尚未出界（或在mergeSort()中，p亦尚未出界）之前
        if ((0 < n) && (p->data <= q->data)) //若p尚未出界且v(p) <= v(q)，则
        {
            p = p->succ;
            n--;
        } //p直接后移，即完成归入
        else //否则，将q转移至p之前，以完成归入
        {
            insert(L.remove((q = q->succ)->pred), p);
            m--;
        }
    return pp->succ; //更新的首节点
}

#endif //CS_DSA_FALL_2021_LIST_H
