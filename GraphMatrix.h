//
// Created by Runze Wang on 2021/11/24.
//

#ifndef CS_DSA_FALL_2021_GRAPHMATRIX_H
#define CS_DSA_FALL_2021_GRAPHMATRIX_H

#include "Graph.h"
#include "Vector.h"



template<typename Tv>
struct Vertex { //顶点对象（为简化起见，并未严格封装）
    Tv data;//数据
    int inDegree, outDegree;//出入度数
    VStatus status; //状态
    int dTime, fTime; //时间标签
    int parent;//在遍历树中的父节点
    int priority; //优先级数
    Vertex(Tv const &d = (Tv) 0) : //构造新顶点
            data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
            dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //暂不考虑权重溢出
    bool operator==(const Vertex<Tv> &v) {
        return this->data == v.data;
    }

    bool operator!=(const Vertex<Tv> &v) {
        return this->data != v.data;
    }
};

template<typename Te>
struct Edge { //边对象（为简化起见，并未严格封装）
    Te data;
    int weight;
    EType type; //数据、权重、类型
    Edge(Te const &d, int w) : data(d), weight(w), type(UNDETERMINED) {} //构造
};

typedef int Te;
typedef char Tv;
class GraphMatrix : public Graph<Tv, Te> { //基于向量，以邻接矩阵形式实现的图
private:
    Vector<Vertex<Tv>> V; //顶点集（向量）
    Vector<Vector<Edge<Te> *>> E; //边集（邻接矩阵）
    void reset() override {
        for (int i = 0; i < n; i++) { //所有顶点的; ++i) {
            status(i) = UNDISCOVERED;
            dTime(i) = fTime(i) = -1; //状态，时间标签COVERED;
            parent(i) = -1;
            priority(i) = INT_MAX; //（在遍历树中的）父节点，优先级数i) = -1;
            for (int j = 0; j < n; j++) //所有边的
                if (exists(i, j)) type(i, j) = UNDETERMINED; //类型
        }
    }

public:
    GraphMatrix() { n = e = 0; } //构造
    ~GraphMatrix() { //析构
        for (int j = 0; j < n; j++) //所有动态创建的
            for (int k = 0; k < n; k++) //边记录
                delete E[j][k]; //逐条清除
    }

    // 顶点的基本操作：查询第i个顶点（0 <= i < n）
    Tv &vertex(int i) override { return V[i].data; } //数据
    int Vsize() const { return V.size(); }
    virtual int inDegree(int i) { return V[i].inDegree; } //入度
    virtual int outDegree(int i) { return V[i].outDegree; } //出度
    virtual int firstNbr(int i) { return nextNbr(i, n); } //首个邻接顶点
    virtual int nextNbr(int i, int j) //相对于顶点j的下一邻接顶点（改用邻接表可提高效率）
    {
        while ((-1 < j) && (!exists(i, --j)));//j>0且i与--j之间不存在边
        return j;
    } //逆向线性试探
    virtual VStatus &status(int i) { return V[i].status; } //状态
    virtual int &dTime(int i) { return V[i].dTime; } //时间标签dTime
    virtual int &fTime(int i) { return V[i].fTime; } //时间标签fTime
    virtual int &parent(int i) { return V[i].parent; } //在遍历树中的父亲
    virtual int &priority(int i) { return V[i].priority; } //在遍历树中的优先级数
    // 顶点的动态操作
    virtual int insert(Tv const &vertex) { //插入顶点，返回编号
        Vector<Edge<int> *> Vedge;
        E.insert(Vedge); //创建新顶点对应的边向量
        E[n].insert(nullptr);

        for (int j = 0; j < n; j++)
            E[j].insert(nullptr);
        for (int i = 0; i < n; ++i)
            E[n].insert(nullptr);
        n++; //各顶点预留一条潜在的关联边
        Vertex<Tv> v(vertex);
        V.insert(v); //顶点向量增加一个顶点
        return V.size();
    }

    virtual Tv remove(int i) { //删除第i个顶点及其关联边（0 <= i < n）
        for (int j = 0; j < n; j++) //所有出边
            if (exists(i, j)) {
                delete E[i][j];
                V[j].inDegree--;
                e--;
            } //逐条删除
        E.remove(i);
        n--; //删除第i行
        Tv vBak = vertex(i);
        V.remove(i); //删除顶点i
        for (int j = 0; j < n; j++) //所有入边
            if (exists(i,j) ) {
                Edge<Te>* x=E[j][i];
                E[j].remove(i);
                delete x;
                V[j].outDegree--;
                e--;
            } //逐条删除
        return vBak; //返回被删除顶点的信息
    }

    // 边的确认操作
    virtual bool exists(int i, int j) //边(i, j)是否存在
    { return (0 <= i) && (i < n) && (0 <= j) && (j < n) && E[i][j] != NULL; }

    // 边的基本操作：查询顶点i与j之间的联边（0 <= i, j < n且exists(i, j)）
    int Esize() const { return E.size(); }
    virtual EType &type(int i, int j) { return E[i][j]->type; } //边(i, j)的类型
    virtual Te &edge(int i, int j) { return E[i][j]->data; } //边(i, j)的数据
    virtual int &weight(int i, int j) { return E[i][j]->weight; } //边(i, j)的权重
    // 边的动态操作
    virtual void insert(Te const &edge, int w, int i, int j) { //插入权重为w的边e = (i, j)
        if (exists(i, j)) return; //确保该边尚不存在
        E[i][j] = new Edge<Te>(edge, w); //创建新边
        e++;
        V[i].outDegree++;
        V[j].inDegree++; //更新边计数与关联顶点的度数
    }

    virtual Te remove(int i, int j) { //删除顶点i和j之间的联边（exists(i, j)）
        Te eBak = edge(i, j);
        delete E[i][j];
        E[i][j] = NULL; //备份后删除边记录
        e--;
        V[i].outDegree--;
        V[j].inDegree--; //更新边计数与关联顶点的度数
        return eBak; //返回被删除边的信息
    }
};

#endif //CS_DSA_FALL_2021_GRAPHMATRIX_H
