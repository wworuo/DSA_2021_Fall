//
// Created by Runze Wang on 2021/11/17.
//

#ifndef CS_DSA_FALL_2021_GRAPHLIST_H
#define CS_DSA_FALL_2021_GRAPHLIST_H

#include "Graph.h"

#include "List.h"

#include "Vector.h"
#include <limits.h>


template<typename Tv>
struct Vertex {
    Tv data;
    int inDegree, outDegree;
    VStatus status;
    int dTime, fTime;
    int parent;
    int priority;

    Vertex(Tv const &d = (Tv) 0) :
            data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
            dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {}

    bool operator==(const Vertex<Tv> &v) {
        return this->data == v.data;
    }

    bool operator!=(const Vertex<Tv> &v) {
        return this->data != v.data;
    }
};

template<typename Te>
struct Edge {
    Te data;
    int weight;
    int tailVex;
    int headVex;
    EType type;

    Edge(Te const &d = (Te) 0, int w = 0, int t = -1, int h = -1) : data(d), weight(w), tailVex(t), headVex(h),
                                                                    type(UNDETERMINED) {}
};

typedef int Te;
typedef char Tv;

class GraphList : public Graph<char, int> {
private:
    ListNode<Edge<Te> *> *hotNode;
    Edge<Te> *hotEdge;
    Vector<Vertex<Tv>> V;
    Vector<List<Edge<Te> *>> E;

    virtual void reset() {
        for (int i = 0; i < n; i++) { //所有顶点的; ++i) {
            status(i) = UNDISCOVERED;
            dTime(i) = fTime(i) = -1; //状态，时间标签COVERED;
            parent(i) = -1;
            priority(i) = INT_MAX; //（在遍历树中的）父节点，优先级数i) = -1;
            hotNode = E[i].first();
            for (int j = 0; j < E[i].size(); j++) {//所有边的
                hotEdge = hotNode->data;
                hotEdge->type = UNDETERMINED;//类型
                hotNode = hotNode->succ;
            }
        }
        hotNode = nullptr;
        hotEdge = nullptr;
    }

public:
    GraphList() { n = e = 0, hotNode = nullptr, hotEdge = nullptr; } //构造
    ~GraphList() { //析构
        Edge<Te> *pe;
        for (int j = 0; j < n; j++) { //所有动态创建的
            while (0 < E[j].size()) {
                pe = E[j].first()->data;
                delete pe;
                E[j].remove(E[j].first());
            }
        }
    }

    // 顶点的基本操作：查询第i个顶点（0 <= i < n）
    virtual Tv &vertex(int i) { return V[i].data; }

    int Vsize() const { return V.size(); }

    virtual ListNode<Edge<Te> *> *HotNode() { return hotNode; }

    virtual Edge<Te> *HotEdge() { return hotEdge; }

    virtual int inDegree(int i) { return V[i].inDegree; } //入度
    virtual int outDegree(int i) { return V[i].outDegree; } //出度
    virtual int firstNbr(int i) {
        hotNode = E[i].first();
        return hotNode->data->headVex;
    }

    virtual int nextNbr(int i, int j) {//对于大量数据连读读写很有效，存在递归或者数据较少时，效果略差

        if (hotNode == nullptr || hotNode->data == nullptr) {//hotNode为其他特殊情况下，hot被置空
            // hotNode->data为空一般是在trail处的值
            if (hotNode == E[i].trail())//该式成立，说明E[i]这个list已经访问完毕，再访问next（i,j）也不应该有值
                return -1;
            for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                if (j == hotNode->data->headVex) {
                    hotNode = hotNode->succ;
                    if (hotNode != E[i].trail())
                        return hotNode->data->headVex;
                    else {
                        return -1;
                    }
                }
            }
        }
        if (hotNode == E[i].last()) {
            hotNode = hotNode->succ;
            return -1;
        }
        if (hotNode->data->tailVex == i && hotNode->data->headVex == j) {
            hotNode = hotNode->succ;
            return hotNode->data->headVex;
        }

    }

    virtual VStatus &status(int i) { return V[i].status; } //状态
    virtual int &dTime(int i) { return V[i].dTime; } //时间标签dTime
    virtual int &fTime(int i) { return V[i].fTime; } //时间标签fTime
    virtual int &parent(int i) { return V[i].parent; } //在遍历树中的父亲
    virtual int &priority(int i) { return V[i].priority; } //在遍历树中的优先级数

    // 顶点的动态操作
    virtual int insert(Tv const &vertex) { //插入顶点，返回编号
        List<Edge<Te> *> Ledge;
        E.insert(Ledge);
        n++; //各顶点预留一条潜在的关联边
        Vertex<Tv> v(vertex);
        V.insert(v);
        return V.size(); //顶点向量增加一个顶点
    }

    virtual Tv remove(int i) { //删除第i个顶点及其关联边（0 <= i < n）
        ListNode<Edge<Te> *> *pNode;
        while (E[i].size() > 0) {
            pNode = E[i].first();
            hotEdge = pNode->data;
            int j = hotEdge->headVex;
            V[j].inDegree--;
            e--;
            delete hotEdge;
        }
        E.remove(i);
        n--; //删除第i行
        Tv vBak = vertex(i);
        V.remove(i); //删除顶点i
        for (int j = 0; j < n; j++) {//所有入边
            for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                hotEdge = pNode->data;
                if (i == hotEdge->headVex)
                    break;
            }
            if (E[j].trail() != pNode) {
                E[j].remove(pNode);
                delete hotEdge;
                V[j].outDegree--;
                e--;
            }//逐条搜索删除
        }
        hotNode = nullptr;
        hotEdge = nullptr;
        return vBak; //返回被删除顶点的信息
    }

    // 边的确认操作
    virtual bool exists(int i, int j) {//边(i, j)是否存在,保留hotNode结点,如果存在，指向当前结点，如果不存在，指向下一个节点
        if ((0 <= i) && (i < n) & (0 <= j) && (j < n)) {
            for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                hotEdge = hotNode->data;
                if (j == hotEdge->headVex)
                    return true;
            }
        }
        return false;
    }

    // 边的基本操作：查询顶点i与j之间的联边（0 <= i, j < n且exists(i,j)）或者来自于nextNbr保证其边的存在性
    int Esize() const { return e; }

    virtual EType &type(int i, int j) {//边(i, j)的类型,注意判断边是否存在,否则可能会出错
        if (hotNode != E[i].trail()) {
            if (hotNode->data->tailVex == i && hotNode->data->headVex == j)
                return hotNode->data->type;
            else {
                for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                    hotEdge = hotNode->data;
                    if (j == hotEdge->headVex) {
                        return hotEdge->type;
                    }
                }
            }
        }
    }

    virtual Te &edge(int i, int j) {//边(i, j)的数据,注意判断边是否存在,否则可能会出错
        if (hotNode != E[i].trail()) {
            if (hotNode->data->tailVex == i && hotNode->data->headVex == j)
                return hotNode->data->data;
            else {
                for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                    hotEdge = hotNode->data;
                    if (j == hotEdge->headVex) {
                        return hotEdge->data;
                    }
                }
            }
        }
    }

    virtual int &weight(int i, int j) {//边的权重，注意判断边是否存在,否则可能会出错
        if (hotNode != E[i].trail()) {
            if (hotNode->data->tailVex == i && hotNode->data->headVex == j)
                return hotNode->data->weight;
            else {
                for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                    hotEdge = hotNode->data;
                    if (j == hotEdge->headVex) {
                        return hotEdge->weight;
                    }
                }
            }
        }
    } //边(i, j)的权重,注意判断边是否存在

// 边的动态操作
    virtual void insert(Te const &edge, int w, int i, int j) { //插入权重为w的边e = (i, j)
        if (exists(i, j)) return; //确保该边尚不存在
        Edge<Te> *newEdge = new Edge<Te>(edge, w, i, j);
        for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
            hotEdge = hotNode->data;
            if (hotEdge->headVex < j) {
                break;
            }
        }
        E[i].insertBefore(newEdge, hotNode);
        e++;
        V[i].outDegree++;
        V[j].inDegree++; //更新边计数与关联顶点的度数
    }

    virtual Te remove(int i, int j) { //删除顶点i和j之间的联边,使用前要一定确保（exists(i, j)）
        if (hotNode != E[i].trail()) {
            if (hotNode->data->tailVex == i && hotNode->data->headVex == j)
                hotEdge = hotNode->data;
            else {
                for (hotNode = E[i].first(); hotNode != E[i].trail(); hotNode = hotNode->succ) {
                    hotEdge = hotNode->data;
                    if (j == hotEdge->headVex)
                        break;
                }
            }
        }
        Te eBak = hotEdge->data;//备份后删除边记录
        delete hotEdge;
        e--;
        V[i].outDegree--;
        V[j].inDegree--; //更新边计数与关联顶点的度数
        return eBak; //返回被删除边的信息
    }
};

#undef Tv
#undef Te
#endif //CS_DSA_FALL_2021_GRAPHLIST_H
