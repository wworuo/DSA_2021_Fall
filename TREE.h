//
// Created by Runze Wang on 2021/10/18.
//

#ifndef CS_DSA_FALL_2021_TREE_H
#define CS_DSA_FALL_2021_TREE_H

#include <utility>
#include "Stack&Queue.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>

#define IsRoot(x) ( ! ( (x).parent ) )
#define IsLChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->lChild ) )
#define IsRChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->rChild) )
#define IsLeaf(x) ( ! HasChild(x) )
#define HasLChild(x) ( (x).lChild )
#define HasRChild(x) ( (x).rChild )
#define HasChild(x) ( HasLChild(x) || HasRChild(x) ) //至少拥有一个孩子
#define HasSibling(x) (sibling(x))
#define sibling(p) ( IsLChild( (p) ) ? (p).parent->rChild : (p).parent->lChild ) /*兄弟*/
#define uncle(x) ( sibling( (x)->parent ) ) /*叔叔*/
#define FromParentTo(x) ( IsRoot(x) ? _root : ( IsLChild(x) ? (x).parent->lChild : (x).parent->rChild ) ) //来自父亲的引用
#define stature(p) ((p) ? (p)->height : -1) //当前 p 的高度
#define N 50


inline int max(int a, int b) {
    return a > b ? a : b;
}

//处理字符串的函数
bool manageString(const char *filename, std::string arrayS[]);

typedef enum {
    LINK, THREAD
} TAG; //节点状态



struct
Node {
    std::string data;
    Node *parent;
    Node *lChild;
    Node *rChild;
    int height;
    TAG LTag, RTag;

    Node() : data(nullptr), parent(nullptr), lChild(nullptr), rChild(nullptr), height(0), LTag(LINK), RTag(LINK) {}

    Node(const std::string &e, Node *p = nullptr, Node *lc = nullptr, Node *rc = nullptr, int h = 0, TAG t = LINK) : \
     data(e), parent(p), lChild(lc), rChild(lc), height(h), LTag(LINK), RTag(LINK) {}

    int size(); //统计当前节点后代总数，亦即以其为根的子树的规模
    Node *insertAsLc(const std::string &e);//作为当前节点的左孩子插入新节点
    Node *insertAsRc(const std::string &e);//作为当前节点的右孩子插入新节点
    Node *succ();//取当前节点的直接后续遍历后继
    template<typename VST>
    friend void travelNodeLevel(Node *, VST &);//子树层次遍历
    template<typename VST>
    friend void travelNodePre(Node *, VST &); //子树先序遍历
    template<typename VST>
    friend void travelNodeIn(Node *, VST &); //子树中序遍历
    template<typename VST>
    friend void travelNodePost(Node *, VST &); //子树后序遍历

};


template<typename VST>
void travelNodePre(Node *x, VST &visit) {
    if (!x) return;
    visit(x->data);
    travelNodePre(x->lChild, visit);
    travelNodePre(x->rChild, visit);
}

template<typename VST>
void travelNodeIn(Node *x, VST &visit) {
    if (!x) return;
    travelNodeIn(x->lChild, visit);
    visit(x->data);
    travelNodeIn(x->rChild, visit);
}

template<typename VST>
void travelNodePost(Node *x, VST &visit) {
    if (!x) return;
    travelNodePost(x->lChild, visit);
    travelNodePost(x->rChild, visit);
    visit(x->data);
}

template<typename VST>
void travelNodeLevel(Node *x, VST &visit) {
    if (!x) return;
    Queue<Node *> q;
    q.push_back(x);
    while (!q.empty()) {
        Node *y = q.front();
        q.pop_front();
        visit(y->data);
        if (y->lChild) q.push_back(y->lChild);
        if (y->rChild) q.push_back(y->rChild);
    }
}

class Tree {
protected:
    Node *_root;
    Node *_hot;
    int _size;

    virtual int updateHeight(Node *x);//更新节点x的高度
    virtual void updateHeightAbov(Node *x);//更新x及祖先的高度
    virtual void createAsPre(Node *&x, std::string *&s);

    virtual void createAsPre(Node *&x, Node *&node);

public:
    Tree() : _root(nullptr), _hot(nullptr), _size(0) {}

    virtual ~Tree() {
        if (0 < _size) remove(_root);
    }

    //创建接口
    virtual bool createTree(std::string *s);

    //释放接口
    virtual int remove(Node *x);

    virtual int release(Node *x);

    //可读接口
    int size() const { return _size; }

    bool empty() const { return !_root; }

    Node *root() const { return _root; }


    //插入接口
    Node *insertAsRC(Node *x, const std::string &e);//插入右结点，一个元素
    Node *insertAsLC(const std::string &e, Node *x);//插入右结点，一个元素
    Node *insertAsRoot(const std::string &e);//将e当作根节点插入空的二叉树

    //遍历接口
    template<typename VST>
    //操作器
    void travelLevel(VST &visit) { if (_root) travelNodeLevel(_root, visit); } //层次遍历

    template<typename VST>
    void travelPre(Node *x, VST &visit) { if (x) travelNodePre(x, visit); } //先序遍历
    template<typename VST>
    void travelPre(VST &visit) { if (_root) travelPre(_root, visit); }

    template<typename VST>
    void travelIn(Node *x, VST &visit) { if (x) travelNodeIn(x, visit); }//中序遍历
    template<typename VST>
    void travelIn(VST &visit) { if (_root) travelIn(_root, visit); }

    template<typename VST>
    void travelPost(Node *x, VST &visit) { if (x) travelNodePost(x, visit); }//后序遍历
    template<typename VST>
    void travelPost(VST &visit) { if (_root) travelPost(_root, visit); }

    //运算符
    Tree &operator=(const Tree &tree);

};

class ThreadBT : public Tree {
protected:
    void createAsPre(Node *&x, Node *&node) override;

    virtual void PostThreading(Node *&x);

public:
    ThreadBT() : Tree() {}

    virtual ~ThreadBT();

    virtual bool createTree(const Tree &tree);

    template<typename VST>
    void PostOrderVisit(VST &visit);
};

template<typename VST>
void ThreadBT::PostOrderVisit(VST &visit) {
    if (_root) {
        Node *curNode = _root;
        _hot = nullptr;
        while (curNode != nullptr) {
            while (curNode->lChild != _hot && curNode->LTag == LINK)
                curNode = curNode->lChild;

            while (curNode && curNode->RTag == THREAD) {
                visit(curNode->data);
                _hot = curNode;
                curNode = curNode->rChild;
            }

            if (curNode == _root) {//到达了根结点
                visit(_root->data);
                return;
            }
            //从右子树往上爬
            while (curNode && curNode->rChild == _hot) {
                visit(curNode->data);
                _hot = curNode;
                curNode = curNode->parent;
            }

            if (curNode && curNode->RTag == LINK)
                curNode = curNode->rChild;
        }
        _hot = nullptr;
    }
}

#endif //CS_DSA_FALL_2021_TREE_H
