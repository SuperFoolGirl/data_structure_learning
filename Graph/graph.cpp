#include <algorithm>
#include <cstdio>
#include <queue>
#include <vector>

#define MAX_SIZE 100
#define MAX 0x7fffffff

typedef char VertexType;    // 顶点类型
typedef int EdgeType;       // 边类型

struct Edge {
    int start;              // 起点
    int end;                // 终点
    EdgeType weight;        // 权重
};

struct Graph {
    std::vector<VertexType> vertex;            // 顶点数组
    std::vector<Edge> edge;                    // 边数组
    std::vector<std::vector<EdgeType>> arc;    // 邻接矩阵
    int vertex_num;                            // 顶点数
    int edge_num;                              // 边数
};

std::vector<bool> visited(MAX_SIZE);           // 访问标记数组

// 该函数用于创建一个特定的图，这里没有给出通用的创建方法。本例中的图也是特定的，仅供参考
// 本图为带权无向图
void createGraph(Graph *g) {
    resetVisited();    // 重置访问标记数组

    g->vertex_num = 9;
    g->edge_num = 15;
    g->vertex.resize(g->vertex_num);                                       // 初始化顶点数组
    g->arc.resize(g->vertex_num, std::vector<EdgeType>(g->vertex_num));    // 初始化邻接矩阵

    // 初始化顶点
    g->vertex[0] = 'A';
    g->vertex[1] = 'B';
    g->vertex[2] = 'C';
    g->vertex[3] = 'D';
    g->vertex[4] = 'E';
    g->vertex[5] = 'F';
    g->vertex[6] = 'G';
    g->vertex[7] = 'H';
    g->vertex[8] = 'I';

    // 初始化邻接矩阵
    for (int i = 0; i < g->vertex_num; i++) {
        for (int j = 0; j < g->vertex_num; j++) {
            if (i == j) {
                g->arc[i][j] = 0;      // 对角线为0，表示没有自环
            } else {
                g->arc[i][j] = MAX;    // 初始化为最大值，表示没有边
            }
        }
    }

    // 添加边。临界矩阵的对称边，稍后统一实现
    // A-B,A-F
    g->arc[0][1] = 10;    // A-B
    g->arc[0][5] = 11;    // A-F
    // B-C,B-G,B-I
    g->arc[1][2] = 18;    // B-C
    g->arc[1][6] = 16;    // B-G
    g->arc[1][8] = 12;    // B-I
    // C-D,C-I
    g->arc[2][3] = 22;    // C-D
    g->arc[2][8] = 8;     // C-I
    // D-E,D-G,D-H,D-I
    g->arc[3][4] = 20;    // D-E
    g->arc[3][6] = 24;    // D-G
    g->arc[3][7] = 16;    // D-H
    g->arc[3][8] = 21;    // D-I
    // E-F,E-H
    g->arc[4][5] = 26;    // E-F
    g->arc[4][7] = 7;     // E-H
    // F-G
    g->arc[5][6] = 17;    // F-G
    // G-H
    g->arc[6][7] = 19;    // G-H

    // 初始化edge数组
    for (int i = 0; i < g->vertex_num; ++i) {
        for (int j = i + 1; j < g->vertex_num; ++j) {
            if (g->arc[i][j] != MAX && g->arc[i][j] != 0) {    // 如果有边
                Edge e;
                e.start = i;
                e.end = j;
                e.weight = g->arc[i][j];
                g->edge.push_back(e);    // 添加边到边数组
            }
        }
    }
    // 给edge数组排序，按权值从小到大，后面kruskal算法需要
    std::sort(g->edge.begin(), g->edge.end(), [](const Edge &a, const Edge &b) {
        return a.weight < b.weight;    // 按权值排序，lambda表达式创建匿名函数
    });

    // 处理对称边
    for (int i = 0; i < g->vertex_num; i++) {
        for (int j = i + 1; j < g->vertex_num; j++) {
            g->arc[j][i] = g->arc[i][j];    // 对称边
        }
    }
}

int find(int *parent, int index) {
    if (parent[index] != index) {
        parent[index] = find(parent, parent[index]);    // 路径压缩
    }
    return parent[index];                               // 返回根节点
}

// 类似前序遍历，用dfs实现
// 图是连通的，而且不需要找路径，所以不需要重置标记，即dfs后没有必要重置visited数组
void dfs(Graph *g, int i) {
    visited[i] = true;
    printf("%c\n", g->vertex[i]);    // 访问顶点
    // 描述一下访问过程：
    // 访问顺序为：i -> j
    // 如果arc[i][j] == C且下一个点j未被访问，表示有边连接i和j，且下一个点j可作为新的起点继续访问
    // 所以递归的dfs中，传入j作为新的起点
    for (int j = 0; j < g->vertex_num; j++) {
        if (g->arc[i][j] != 0 && g->arc[i][j] != MAX && visited[j] == false) {    // 如果有边且未访问
            dfs(g, j);                                                            // 递归访问
        }
    }
}

// 层序遍历
void bfs(Graph *g) {
    int curr = 0;
    visited[curr] = true;               // 标记起点已访问
    printf("%c\n", g->vertex[curr]);    // 访问起点

    std::queue<int> q;
    q.push(curr);

    while (!q.empty()) {
        curr = q.front();
        q.pop();
        for (int j = 0; j < g->vertex_num; ++j) {
            if (g->arc[curr][j] != 0 && g->arc[curr][j] != MAX && visited[j] == false) {    // 如果有边且未访问
                visited[j] = true;                                                          // 标记已访问
                printf("%c\n", g->vertex[j]);                                               // 访问顶点
                q.push(j);                                                                  // 入队
            }
        }
    }
}

// 最小生成树(无向有权图)
// Prim算法：任取一个结点开始扩展，直到所有结点都被访问
// Kruskal算法：先将所有边按权值从小到大排序，然后依次取出边，判断是否形成环，直到所有结点都被访问
// 由于图是连通的，所以可以从任意一个顶点开始
// 提前说一句，代码实现与离散数学的肉眼做法不太一样
void prim(Graph *g) {
    int min_edge[MAX_SIZE];     // 记录当前最小边的权值
    int vex_index[MAX_SIZE];    // 值表示起点，下标表示终点。二者映射代表边的关系

    // 先从顶点A开始，完成准备工作
    for (int i = 0; i < g->vertex_num; ++i) {
        min_edge[i] = g->arc[0][i];
        vex_index[i] = 0;
    }

    // 主循环
    for (int i = 1; i < g->vertex_num; ++i) {
        int min = MAX;
        int new_node = 0;
        // 找到当前最小边
        for (int j = 0; j < g->vertex_num; ++j) {
            if (min_edge[j] != 0 && min_edge[j] < min) {
                min = min_edge[j];
                new_node = j;    // 记录当前最小边的终点
            }
        }
        // 访问当前最小边的终点
        printf("Edge: %c - %c, Weight: %d\n", g->vertex[vex_index[new_node]], g->vertex[new_node], min);
        min_edge[new_node] = 0;    // 标记已访问，避免生成树成环

        // 找新连接结点k的待选路径
        for (int j = 0; j < g->vertex_num; ++j) {
            // 与新结点可连接的边进行比较，如果小于原min_edge[j]，则更新
            // !=0判断的目的是避免成环，小于判断是为了满足“最小”生成树的要求
            // 这里逻辑用语言可能讲不清楚，建议回到原视频
            if (g->arc[new_node][j] != 0 && g->arc[new_node][j] < min_edge[j]) {
                min_edge[j] = g->arc[new_node][j];
                vex_index[j] = new_node;    // 更新边的起点
            }
        }
    }
}

// 边少时，即对于稀疏图，Kruskal算法更高效
void kruskal(Graph *g) {
    // 需要用到已排序的edge数组和并查集
    // 回忆下并查集在图中的作用：判断两个顶点是否在同一连通分量中
    int parent[g->vertex_num];    // 并查集的父节点数组
    for (int i = 0; i < g->vertex_num; ++i) {
        parent[i] = i;            // 初始化并查集，每个顶点的父节点是它自己
    }

    for (int i = 0; i < g->edge_num; ++i) {
        int n = find(parent, g->edge[i].start);    // 查找起点的根节点
        int m = find(parent, g->edge[i].end);      // 查找终点的根节点

        // 如果起点和终点不在同一个集合中，说明不会形成环
        if (n != m) {
            parent[n] = m;    // 合并两个集合，将终点的根节点指向起点的根节点
            printf("Edge: %c - %c, Weight: %d\n", g->vertex[g->edge[i].start], g->vertex[g->edge[i].end],
                   g->edge[i].weight);
        }
    }
}

// 最短路径dijkstra算法
void dijkstra(Graph *g) {

}

// 重置访问标记数组
void resetVisited() {
    for (int i = 0; i < visited.size(); ++i) {
        visited[i] = false;    // 重置访问标记数组
    }
}

int main() {
    Graph g;
    createGraph(&g);
    kruskal(&g);    // 使用Kruskal算法生成最小生成树
    return 0;
}
