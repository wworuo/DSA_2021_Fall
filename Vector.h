//
// Created by Runze Wang on 2021/11/17.
//

#ifndef CS_DSA_FALL_2021_VECTOR_H
#define CS_DSA_FALL_2021_VECTOR_H


#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include "typeinfo.h"

typedef int Rank;
#define DEFAULT_CAPACITY 3

template<typename T>
class Vector {//最大特点在于可以寻秩访问，_elem + r * s 代表了秩为r的地址
protected:
    Rank _size;  //_elem[Rank r] 可以直接代表[0,_size)之间的元素，_size既可以反应处当前元素个数，也可以用来判断是否溢出
    int _capacity;//_capacity用来判断整体的容量
    T *_elem;// _elem[lo,hi)中，hi-lo可以直接表示出数组的元素个数，同时_elem取值范围也规定为lo到hi-1；当r<=hi时，就代表了越界

    void copyFrom(T *const A, Rank lo, Rank hi);//从A的[lo,hi)位置复制给_elem指定的位置

    void expand();//可扩充向量空间

    void shrink();//可缩小向量空间


    void mergeSort(Rank lo, Rank hi) {//归并排序，最坏O(n*log n)
        if (hi - lo < 2) return;
        int mi = (lo + hi) >> 1;
        mergeSort(lo, mi);
        mergeSort(mi, hi);
        merge(lo, mi, hi);
    }

    void merge(Rank lo, Rank mi, Rank hi);//处理两端以排好序的子列顺序
    Rank binSearch(T *S, T const &e, Rank lo, Rank hi); //在有序向量的[lo,hi)范围内查找是否有与e相同值的秩
    //如果有，返回不大于该元素e的最高位置；如果没有，返回lo-1或hi-1；
    //平均为O(log n)

public:
    //构造函数
    Vector(int c = DEFAULT_CAPACITY) {
        _capacity = c;
        _elem = new T[_capacity];
        _size = 0;
    }

    Vector(T const *A, Rank lo, Rank hi) {
        copyFrom(A, lo, hi);
    }

    Vector(Vector<T> const &V, Rank lo, Rank hi) {
        copyFrom(V._elem, lo, hi);
    }

    //拷贝构造函数
    Vector(Vector<T> const &V) {
        copyFrom(V._elem, 0, V._size);
    }

    Vector(std::initializer_list<T> &il) {
        _size = il.size();
        _capacity = _size * 2;
        _elem = new T[_capacity];
        std::copy(il.begin(), il.end(), _elem);
    }

    //析构函数
    virtual ~Vector() {
        delete[]_elem;
    }

    // 只读访问接口
    Rank size() const { return _size; } //规模
    bool empty() const { return !_size; } //判空

    Rank find(T const &e) const { return find(e, 0, _size); } //无序向量整体查找

    Rank find(T const &e, Rank lo, Rank hi) const;//在[lo,hi)范围内查找是否有与e相同值的秩
    // 如果有，返回其位置；如果没有，则返回值<lo;
    //最小为O(n),最大为O(hi-lo)
    Rank search(T const &e) const { return (0 >= _size) ? -1 : binsearch(e, 0, _size); } //有序向量整体查找


    // 可写访问接口
    T &operator[](Rank r);//重载[] r<=_size
    Vector<T> &operator=(const Vector<T> &v) {
        if (this == &v) {
            return *this;
        }
        for (int i = 0; i < _size; ++i) {
            release(_elem[i]);
        }
        if (_elem)
            delete[]_elem;
        copyFrom(v._elem, 0, v.size());
        return *this;
    }

    Rank insert(Rank r, T const &e);//e作为秩为r的元素插入，r<=size；返回插入数值的秩
    Rank insert(T const &e); //默认作为末元素插入
    int remove(Rank lo, Rank hi);//删除[lo,hi)区间的数值，lo<=hi<=size;返回被删除元素的数目
    T remove(Rank r);//删除特定位置的数值  O(n-r)
    void sort(Rank lo, Rank hi) { mergeSort(lo, hi); }; //对[lo, hi)排序
    void sort() { sort(0, _size); } //整体排序

    int deduplicate();//删除重复的项(无序)   O(n^2)

    int uniquify();//删除重复的项(有序)   O(n)


    void swap(T &a, T &b) {
        T t;
        t = a;
        a = b;
        b = t;
    }


};

template<typename T>
void Vector<T>::copyFrom(T *const A, Rank lo, Rank hi) {
    if ((hi - lo) > DEFAULT_CAPACITY)
        _capacity = 2 * (hi - lo);
    _elem = new T[_capacity];
    _size = 0;
    while (lo < hi)
        _elem[_size++] = A[lo++];
}

template<typename T>
//O（n）大量分配内存的开销，不应该频繁使用mnn
void Vector<T>::expand() {
    if (_size < _capacity)
        return;
    else {
        T *oldElem = _elem;
        _capacity *= 2;
        _elem = new T[_capacity];//_capacity*=2;
        for (int i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
            release(oldElem[i]);
        }
        if (oldElem)
            delete[]oldElem;
    }
}

template<typename T>
//O（n）大量分配内存的开销，不应该频繁使用
void Vector<T>::shrink() {
    if (_capacity <= DEFAULT_CAPACITY || _size >= _capacity / 2)
        return;
    else {
        T *oldElem = _elem;
        _capacity /= 2;
        _elem = new T[_capacity];
        for (int i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
            release(oldElem[i]);
        }
        delete[]oldElem;;
    }
}

template<typename T>
T &Vector<T>::operator[](Rank r) { return _elem[r]; }

template<typename T>
Rank Vector<T>::insert(T const &e) {
    expand();
    _elem[_size] = e;
    _size++;
    return _size - 1;
}

template<typename T>
//平均时间内复杂度：O(n)
Rank Vector<T>::insert(Rank r, const T &e) {
    expand();//如果有必要，则会扩容
    for (int i = _size; i > r; --i) {//自后向前移动
        _elem[i] = _elem[i - 1];
    }
    _elem[r] = e;
    _size++;
    return r;
}

template<typename T>
//平均时间内复杂度：O(n)
int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo == hi) return 0;
    while (hi < _size) _elem[lo++] = _elem[hi++];//[hi,size)顺次前移
    _size = lo;
    shrink();
    return hi - lo;
}

template<typename T>
T Vector<T>::remove(Rank r) {
    T e = _elem[r];
    remove(r, r + 1);
    return e;
}

template<typename T>
Rank Vector<T>::find(const T &e, Rank lo, Rank hi) const {
    while ((lo < hi--) && (e != _elem[hi]));
    return hi;
}


template<typename T>
Rank Vector<T>::deduplicate() {
    int oldSize = _size;
    Rank i = 1;
    while (i < _size)
        if (find(_elem[i], 0, i) < 0)
            i++;//无雷同项，继续看后继一项
        else
            remove(i);//否则删除雷同项，前移，size--
    return oldSize - _size;
}

template<typename T>
Rank Vector<T>::uniquify() {
    Rank i = 0, j = 0;
    while (++j < _size)
        //跳过雷同项，发现不同元素时，直接前移秩紧邻与前者的右侧
        if (_elem[i] != _elem[j])
            _elem[++i] = _elem[j];
    _size = ++i;
    shrink();
    return j - i;//返回被删除元素的个数
}


template<typename T>
void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    T *A = _elem + lo;
    int lb = mi - lo;
    T *B = new T[lb];
    for (Rank i = 0; i < lb; B[i] = A[i++]);
    int lc = hi - mi;
    T *C = _elem + mi;
    /* for (int i = 0,j= 0,k = 0 ;(j<lb)||(k<lc) ; ) {
         if( (j<lb) && ((lc <= k) || (B[j]< C[k])) ) A[i++] =B[j++];
         if( (k<lc) && ((lb <= j) || (C[k] <= B[j])) ) A[i++]= C[k++];
     }*/
    for (int i = 0, j = 0, k = 0; (j < lb);) {
        if ((lc <= k) || (B[j] < C[k])) A[i++] = B[j++];
        if ((k < lc) && (C[k] <= B[j])) A[i++] = C[k++];
    }
    delete[] B;
}


// 二分查找算法（版本C）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template<typename T>

Rank Vector<T>::binSearch(T *S, T const &e, Rank lo, Rank hi) {
    while (lo < hi) { //每步迭代仅需做一次比较判断，有两个分支
        Rank mi = (lo + hi) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移）
        (e < S[mi]) ? hi = mi : lo = mi + 1; //经比较后确定深入[lo, mi)或(mi, hi)
    } //成功查找不能提前终止
    return lo - 1; //循环结束时，lo为大于e的元素的最小秩，故lo - 1即不大于e的元素的最大秩
}


#undef DEFAULT_CAPACITY
#endif //CS_DSA_FALL_2021_VECTOR_H
