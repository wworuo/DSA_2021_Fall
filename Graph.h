//
// Created by Runze Wang on 2021/11/17.
//

#ifndef CS_DSA_FALL_2021_GRAPH_H
#define CS_DSA_FALL_2021_GRAPH_H

#include <iostream>
#include "Stack&Queue.h"

typedef enum {
    UNDISCOVERED, DISCOVERED, VISITED
} VStatus;
typedef enum {
    UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD
} EType; //边在遍历树中所属的类型

template<typename Tv, typename Te>
class Graph {
    void BFS(int, int &); //（连通域）广度优先搜索算法
    void DFS(int, int &); //（连通域）深度优先搜索算法
    template<typename VST>
    void TravelBFS(int,VST visit);
    template<typename PU>
    void PFS(int, PU); //（连通域）优先级搜索框架
protected:
    virtual void reset() = 0;

public:
    struct DijkPU { //针对Dijkstra算法的顶点优先级更新器
        virtual void operator()(Graph<Tv, Te> *g, int uk, int v);
    };

    struct PrimPU { //针对Prim算法的顶点优先级更新器
        virtual void operator()(Graph<Tv, Te> *g, int uk, int v);
    };
    struct PrintTree{
        virtual void operator()(Graph<Tv,Te>*g,int parent,int v );
    };
    // 顶点
    int n; //顶点总数
    virtual int insert(Tv const &) = 0; //插入顶点，返回编号
    virtual Tv remove(int) = 0; //删除顶点及其关联边，返回该顶点信息
    virtual Tv &vertex(int) = 0; //顶点v的数据（该顶点的确存在）
    virtual int inDegree(int) = 0; //顶点v的入度（该顶点的确存在）
    virtual int outDegree(int) = 0; //顶点v的出度（该顶点的确存在）
    virtual int firstNbr(int) = 0; //顶点v的首个邻接顶点
    virtual int nextNbr(int, int) = 0; //顶点v的（相对于顶点j的）下一邻接顶点
    virtual VStatus &status(int) = 0; //顶点v的状态
    virtual int &dTime(int) = 0; //顶点v的时间标签dTime
    virtual int &fTime(int) = 0; //顶点v的时间标签fTime
    virtual int &parent(int) = 0; //顶点v在遍历树中的父亲
    virtual int &priority(int) = 0; //顶点v在遍历树中的优先级数
// 边：这里约定，无向边均统一转化为方向互逆的一对有向边，从而将无向图视作有向图的特例
    int e; //边总数
    virtual bool exists(int, int) = 0; //边(v, u)是否存在
    virtual void insert(Te const &, int, int, int) = 0; //在顶点v和u之间插入权重为w的边e
    virtual Te remove(int, int) = 0; //删除顶点v和u之间的边e，返回该边信息
    virtual EType &type(int, int) = 0; //边(v, u)的类型
    virtual Te &edge(int, int) = 0; //边(v, u)的数据（该边的确存在）
    virtual int &weight(int, int) = 0; //边(v, u)的权重

    // 算法
    void bfs(int); //广度优先搜索算法
    void dfs(int); //深度优先搜索算法
    template<typename VST>
    void travelBFS(int,VST );
    void prim(int); //最小支撑树Prim算法
    void dijkstra(int); //最短路径Dijkstra算法
    template<typename PU>
    void pfs(int, PU); //优先级搜索框架
};

template<typename Tv, typename Te>
void Graph<Tv, Te>::bfs(int s) {
    reset();
    int clock = 0;
    int v = s;
    do {
        if (UNDISCOVERED == status(v))
            BFS(v, clock);
        v++;
        v = v % n;
    } while (s != v);
}

template<typename Tv, typename Te>
void Graph<Tv, Te>::BFS(int v, int &clock) {
    Queue<int> Q;
    status(v) = DISCOVERED;
    Q.push_back(v);
    while (!Q.empty()) {
        int w = Q.front();
        Q.pop_front();
        dTime(w) = ++clock;
        for (int u = firstNbr(w); -1 < u; u = nextNbr(w, u)) {
            switch (status(u)) {
                case UNDISCOVERED:
                    status(u) = DISCOVERED;
                    Q.push_back(u);
                    type(w, u) = TREE;
                    parent(u) = w;
                    break;
                case DISCOVERED:
                    type(w, u) = CROSS;
                    break;
                default:
                    type(w, u) = BACKWARD;
                    break;
            }
        }
        status(w) = VISITED;
        std::cout << vertex(w);
        fTime(w) = clock;
    }
}
template<typename Tv, typename Te>
template<typename VST>
void Graph<Tv, Te>::travelBFS(int s, VST visit) {
    for (int i = 0; i < n; i++){
        status(i) = UNDISCOVERED;
     //状态，时间标签COVERED;
    }
    int v = s;
    do {
        if (UNDISCOVERED == status(v))
            TravelBFS(v, visit);
        v++;
        v = v % n;
    } while (s != v);
}
//TODO
template<typename Tv, typename Te>
template<typename VST>
void Graph<Tv, Te>::TravelBFS(int v, VST visit) {
    Queue<int> Q;
    status(v) = DISCOVERED;
    Q.push_back(v);
    while (!Q.empty()) {
        int w = Q.front();
        Q.pop_front();
        for (int u = firstNbr(w); -1 < u; u = nextNbr(w, u)) {
            switch (status(u)) {
                case UNDISCOVERED:
                    status(u) = DISCOVERED;
                    Q.push_back(u);
                    visit(this,w,u);
                    break;
                default:
                    visit(this,w,u);
                    break;
            }
        }
        status(w) = VISITED;
    }
}


template<typename Tv, typename Te>
void Graph<Tv, Te>::dfs(int s) {
    reset();
    int clock = 0;
    int v = s;
    do {
        if (UNDISCOVERED == status(v))
            DFS(v, clock);
    } while (s != (v = (++v % n)));
}
//深度优先搜索DFS算法（单个连通域）
/*void Graph<Tv, Te>::DFS(int v, int &clock) { //assert: 0 <= v < n
    dTime(v) = ++clock;
    status(v) = DISCOVERED; //发现当前顶点v
    std::cout<<vertex(v);
    int u;
    for ( u = firstNbr(v); -1 < u; u = nextNbr(v, u)) //枚举v的所有邻居u
        switch (status(u)) { //并视其状态分别处理
            case UNDISCOVERED: //u尚未发现，意味着支撑树可在此拓展
                type(v, u) = TREE;parent(u) = v;
                DFS(u, clock);
                break;
            case DISCOVERED: //u已被发现但尚未访问完毕，应属被后代指向的祖先
                type(v, u) = BACKWARD;
                break;
                //有向图才可能有 forward 和 cross 边
            default: //u已访问完毕（VISITED，有向图），则视承袭关系分为前向边或跨边
                type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS;
                break;
        }
    status(v) = VISITED;
    fTime(v) = ++clock; //至此，当前顶点v方告访问完毕
}
*/
template<typename Tv, typename Te>
void Graph<Tv, Te>::DFS(int v, int &clock) {
    Stack<int> S;
    int w = -1, u;
    dTime(v) = ++clock;
    status(v) = DISCOVERED;
    std::cout << vertex(v);
    S.push(v);
    while (!S.empty()) {
        if (w == -1) {
            u = firstNbr(S.top());
        } else {
            u = nextNbr(S.top(), w);
        }
        if (u != -1) {
            switch (status(u)) {
                case UNDISCOVERED://u尚未发现，意味着支撑树可在此拓展
                    type(S.top(), u) = TREE;
                    parent(u) = S.top();
                    S.push(u);
                    dTime(u) = ++clock;
                    status(u) = DISCOVERED;
                    std::cout << vertex(u);
                    w = -1;
                    break;
                case DISCOVERED: //u已被发现但尚未访问完毕，应属被后代指向的祖先
                    type(S.top(), u) = BACKWARD;
                    w = u;
                    break;
                    //有向图才可能有 forward 和 cross 边
                default: //u已访问完毕（VISITED，有向图），则视承袭关系分为前向边或跨边
                    type(S.top(), u) = (dTime(S.top()) < dTime(u)) ? FORWARD : CROSS;
                    w = u;
                    break;
            }
        } else {
            w = S.top();
            status(w) = VISITED;
            fTime(w) = ++clock;
            S.pop();
        }
    }
}

template<typename Tv, typename Te>
template<typename PU>
//优先级搜索（全图）
void Graph<Tv, Te>::pfs(int s, PU prioUpdater) { //assert: 0 <= s < n
    reset();
    int v = s; //初始化
    do //逐一检查所有顶点
        if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
            PFS(v, prioUpdater); //即从该顶点出发启动一次PFS
    while (s != (v = (++v % n))); //按序号检查，故不漏不重
}

template<typename Tv, typename Te>
template<typename PU>
void Graph<Tv, Te>::PFS(int s, PU prioUpdater) {//通过定义具体的优先级更新策略prioUpdater，即可实现不同的算法功能
    priority(s) = 0;
    status(s) = VISITED;
    parent(s) = -1;
    while (1) {
        for (int w = firstNbr(s); -1 < w; w = nextNbr(s, w)) {
            prioUpdater(this, s, w);
        }
        for (int shortest = INT_MAX, w = 0; w < n; w++) {//选出下一个最短边
            if (UNDISCOVERED == status(w))
                if (shortest > priority(w)) {
                    shortest = priority(w);
                    s = w;
                }
        }
        if (VISITED == status(s)) break;
        status(s) = VISITED;
        if (exists(parent(s),s))
        type(parent(s), s) = TREE;
    }
}

template<typename Tv, typename Te>
//最短路径
void Graph<Tv, Te>::DijkPU::operator()(Graph<Tv, Te> *g, int uk, int v) {
    if (UNDISCOVERED == g->status(v))
        if (g->priority(v) > g->priority(uk) + g->weight(uk, v)) {
            g->priority(v) = g->priority(uk) + g->weight(uk, v);
            g->parent(v) = uk;
        }
}

template<typename Tv, typename Te>
//最小生成树
void Graph<Tv, Te>::PrimPU::operator()(Graph<Tv, Te> *g, int uk, int v) {
    if (UNDISCOVERED == g->status(v))
        if (g->priority(v) > g->weight(uk, v)) {
            g->priority(v) = g->weight(uk, v);
            g->parent(v) = uk;
        }
}
template<typename Tv,typename Te>
void Graph<Tv, Te>::PrintTree::operator()(Graph<Tv, Te> *g, int parent, int child) {
    if(g->type(parent,child)==TREE)
        std::cout<<g->vertex(parent)<<g->vertex(child)<<' ';
}

#endif //CS_DSA_FALL_2021_GRAPH_H
