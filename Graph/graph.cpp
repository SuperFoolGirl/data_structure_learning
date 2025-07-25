#include <algorithm>
#include <cstdio>
#include <queue>
#include <stack>
#include <vector>

#define MAX_SIZE 100
#define MAX 0x7fffffff

typedef char VertexType;    // 顶点类型
typedef int EdgeType;       // 边类型

// 边结点1
struct Edge {
    int start;          // 起点
    int end;            // 终点
    EdgeType weight;    // 权重
};

struct Graph {
    std::vector<VertexType> vertex;            // 顶点数组
    std::vector<Edge> edge;                    // 边数组
    std::vector<std::vector<EdgeType>> arc;    // 邻接矩阵
    int vertex_num;                            // 顶点数
    int edge_num;                              // 边数
};

std::vector<bool> visited(MAX_SIZE);           // 访问标记数组

// 边结点2（用于邻接表）
struct EdgeNode {
    int edge_vex;    // 一条邻接表中顶点的序号
    int weight;      // 权重
    EdgeNode *next;
};

// 顶点结点，用于邻接表表头
struct VertexNode {
    int in;             // 入度
    VertexType data;    // 顶点数据
    EdgeNode *head;     // 指向边结点的指针
};

// 邻接表结构体
struct AdjGraph {
    VertexNode adj_list[MAX_SIZE];    // 顶点数组，即二维链表
    int vertex_num;                   // 顶点数
    int edge_num;                     // 边数
};

// 重置访问标记数组
void resetVisited() {
    for (int i = 0; i < visited.size(); ++i) {
        visited[i] = false;    // 重置访问标记数组
    }
}

void createAdjGraph(Graph *g, AdjGraph *adj_g) {
    adj_g->vertex_num = g->vertex_num;
    adj_g->edge_num = g->edge_num;

    for (int i = 0; i < adj_g->vertex_num; ++i) {
        adj_g->adj_list[i].in = 0;                 // 初始化入度为0
        adj_g->adj_list[i].data = g->vertex[i];    // 设置顶点数据，每个顶点都要做链表表头
        adj_g->adj_list[i].head = nullptr;         // 初始化边结点指针为空
    }

    // 遍历边数组，将边添加到邻接表中
    for (const auto &e : g->edge) {
        // 创建新的边结点
        EdgeNode *new_edge = new EdgeNode;
        new_edge->edge_vex = e.end;     // 设置边结点的顶点序号
        new_edge->weight = e.weight;    // 设置边结点的权重
        // 实质是头插法
        new_edge->next = adj_g->adj_list[e.start].head;    // 将新边结点插入到表头。这里命名head有些许歧义
        adj_g->adj_list[e.start].head = new_edge;          // 更新表头指针
        adj_g->adj_list[e.end].in++;                       // 更新终点的入度
    }
}

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
            printf("Edge: %c - %c, Weight: %d\n", g->vertex[g->edge[i].start], g->vertex[g->edge[i].end], g->edge[i].weight);
        }
    }
}

// 最短路径 dijkstra算法
// Dijkstra算法：从起点开始，逐步扩展到所有顶点，找到最短路径
// 注意：Dijkstra算法只能用于非负权图，且不能处理负权边
// 优先队列
void dijkstra(Graph *g) {
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;    // 最小堆
    std::vector<int> dist(g->vertex_num, MAX);                                                        // 初始化距离数组，初始值为最大值
    std::vector<int> parent(g->vertex_num, -1);                                                       // 记录前驱节点，用于最后输出路径
    int start = 0;                                                                                    // 起点索引，这里假设从顶点A开始
    dist[start] = 0;                                                                                  // 起点到自己的距离为0
    pq.push({0, start});                                                                              // 将起点加入优先队列，
    // 优先队列中的元素是一个pair，第一个元素是距离，第二个元素是顶点索引
    // curr代表当前处理的顶点，next是下一步待扩展的顶点
    while (!pq.empty()) {
        int curr = pq.top().second;    // 获取当前距离最小的顶点
        pq.pop();

        if (visited[curr]) {
            continue;    // 如果已经访问过，跳过
        }

        // 标记为已访问。把它放在这，而不是下面的for循环里
        // 处理每个出队的顶点时，统一处理其访问属性，而不是尝试拓展时就处理
        // 这种微妙的思想值得体会一下
        visited[curr] = true;

        // 遍历所有邻接顶点
        for (int next = 0; next < g->vertex_num; ++next) {
            if (g->arc[curr][next] != 0 && g->arc[curr][next] != MAX) {    // 如果有边且不是最大值
                int new_dist = dist[curr] + g->arc[curr][next];            // 计算新距离
                if (new_dist < dist[next]) {                               // 如果新距离小于原距离
                    dist[next] = new_dist;                                 // 更新距离
                    parent[next] = curr;                                   // 更新前驱节点
                    pq.push({new_dist, next});                             // 将新距离和顶点加入优先队列
                }
            }
        }
    }

    // 输出最短路径
    for (int i = 1; i < g->vertex_num; ++i) {
        if (dist[i] == MAX) {
            printf("Vertex %c is unreachable from %c\n", g->vertex[i], g->vertex[start]);
        } else {
            printf("Shortest path from %c to %c is %d\n", g->vertex[start], g->vertex[i], dist[i]);
            // 输出路径
            printf("Path: ");
            int j = i;
            while (j != -1) {
                printf("%c ", g->vertex[j]);
                j = parent[j];    // 回溯前驱节点，想象一个串联的结构
            }
            printf("\n");
        }
    }
}

// 最短路径 Floyd算法
// Floyd算法：计算所有顶点对之间的最短路径，适用于有向图和负权边，但不能处理负权环
// 相比于Dijkstra算法，Floyd算法更通用，打破了起点固定和负边权的限制，但时间复杂度较高
// 简单讲一下算法：
// 1. 准备dist数组，dist[i][j]表示从顶点i到顶点j的最短路径。初始化为临界矩阵即可
// 2. 准备path数组，path[i][j]表示从顶点i到顶点j这条路径上，j的前驱节点。
// 3. 对于图中的每个顶点，依次以它为中间点，更新dist数组和path数组
// 4. 以顶点k为中间点，k行k列以及主对角线不需要动，其他行列更新为以k为中间点的最短路径
// 5. 最终dist数组中存储了所有顶点对之间的最短路径，path数组中存储了前驱节点信息。根据path一步步回溯即可得到路径
void floyd(Graph *g) {
    int dist[MAX_SIZE][MAX_SIZE];    // 最短路径矩阵
    int path[MAX_SIZE][MAX_SIZE];    // 前驱节点矩阵

    // 初始化dist和path矩阵
    for (int i = 0; i < g->vertex_num; ++i) {
        for (int j = 0; j < g->vertex_num; ++j) {
            dist[i][j] = g->arc[i][j];                         // 初始化最短路径矩阵为临界矩阵
            if (g->arc[i][j] != MAX && g->arc[i][j] != 0) {    // 0也是非法边权，即自反边
                // 如果有边，前驱节点为起点。注意，根据邻接矩阵得来的dist数组，i到j的路径是直接的，i就是前驱节点
                path[i][j] = i;
            } else {
                path[i][j] = -1;    // 如果没有边，前驱节点为-1，表示不可达
            }
        }
    }

    // Floyd算法核心逻辑
    // 类比一下动态规划，dist[i][j]表示从i到j的最短路径，中间经历的点是不考虑的，或者说抽象处理的
    for (int mid = 0; mid < g->vertex_num; ++mid) {              // 中间点
        for (int i = 0; i < g->vertex_num; ++i) {                // 起点
            for (int j = 0; j < g->vertex_num; ++j) {            // 终点
                if (dist[i][mid] != MAX && dist[mid][j] != MAX && dist[i][mid] + dist[mid][j] < dist[i][j]) {
                    dist[i][j] = dist[i][mid] + dist[mid][j];    // 更新最短路径
                    path[i][j] = path[mid][j];                   // 更新前驱节点
                }
            }
        }
    }

    // 输出最短路径和前驱节点信息
    for (int i = 0; i < g->vertex_num; ++i) {
        for (int j = 0; j < g->vertex_num; ++j) {
            if (dist[i][j] == MAX) {
                printf("Vertex %c to %c is unreachable\n", g->vertex[i], g->vertex[j]);
            } else {
                printf("Shortest path from %c to %c is %d\n", g->vertex[i], g->vertex[j], dist[i][j]);
                // 输出路径
                printf("Path: ");
                int k = j;
                while (k != -1) {
                    printf("%c ", g->vertex[k]);
                    k = path[i][k];    // 回溯前驱节点
                }
                printf("\n");
            }
        }
    }
}

// 拓扑排序
// 用于有向无环图，基于AOV网——用顶点表示活动，用弧表示活动之间的优先级，这样的有向图为顶点表示活动的网
// 每次选择入度为0的顶点，将其加入结果序列，并删除该顶点及其出边，直到所有顶点都被处理
// 如果遇到环，则无法进行拓扑排序
// 代码中，需要把邻接矩阵转为邻接表
void topologicalSort(AdjGraph *g) {
    std::stack<int> s;                   // 用栈来存储入度为0的顶点
    for (int i = 0; i < g->vertex_num; ++i) {
        if (g->adj_list[i].in == 0) {    // 如果入度为0，入栈
            s.push(i);
        }
    }

    while (!s.empty()) {
        int curr = s.top();
        s.pop();
        printf("%c ", g->adj_list[curr].data);    // 访问顶点
        EdgeNode *e = g->adj_list[curr].head;     // 获取当前顶点的边结点

        // 遍历当前顶点的所有出边，即遍历一条邻接表
        // 所有出边的入度都减1，且入度为0的顶点入栈
        while (e != nullptr) {
            int next = e->edge_vex;             // 获取下一个顶点，即当前边的终点
            g->adj_list[next].in--;             // 入度减1
            if (g->adj_list[next].in == 0) {    // 如果入度为0，入栈
                s.push(next);
            }
            e = e->next;                        // 移动到下一个边结点
        }
    }
}

// 关键路径
// 从前往后推：事件最早发生时间，岔路取最大值
// 从后往前推：事件最晚发生时间，岔路取最小值
// 关键路径求法1：取事件最早发生时间和最晚发生时间相等的结点，连接成路径
// 这个求法有两种实现：动态规划或者栈
// 关键路径求法2：从起点到终点的最长路径，表示完成整个工程所需的最短时间
// 从上述两个方法可以看出，关键路径不一定会遍历所有顶点

// 关键路径上的活动称为关键活动，关键活动的延误会导致整个工程的延误
// 在一个表示工程的带权有向图中，用顶点表示事件，用有向边表示活动，用边上的权值表示活动持续事件，称为AOE网（带权值的图也被称为网）
// 拓扑排序仅能体现做事的优先级，但不能体现做事的时间，关键路径则可以
void criticalPath(AdjGraph *g) {
    // 过程中用的拓扑排序
    std::stack<int> s1;
    std::stack<int> s2;
    int earliest[MAX_SIZE];              // 最早发生时间
    int latest[MAX_SIZE];                // 最晚发生时间

    for (int i = 0; i < g->vertex_num; ++i) {
        if (g->adj_list[i].in == 0) {    // 入度为0的顶点入栈
            s1.push(i);
        }
    }
    for (int i = 0; i < g->vertex_num; ++i) {
        earliest[i] = 0;    // 初始化最早发生时间为0
        latest[i] = MAX;    // 初始化最晚发生时间为最大值
    }

    while (!s1.empty()) {
        int curr = s1.top();
        s1.pop();
        printf("%c\n", g->adj_list[curr].data);                                       // 拓扑排序输出
        s2.push(curr);                                                                // 将当前顶点入栈，后续用于逆序处理

        EdgeNode *e = g->adj_list[curr].head;                                         // 获取当前顶点的边结点
        while (e != nullptr) {
            int next = e->edge_vex;                                                   // 获取下一个顶点
            earliest[next] = std::max(earliest[next], earliest[curr] + e->weight);    // 更新最早发生时间
            g->adj_list[next].in--;                                                   // 入度减1
            if (g->adj_list[next].in == 0) {                                          // 如果入度为0，入栈
                s1.push(next);
            }
            e = e->next;                                                              // 移动到下一个边结点
        }

        // 输出最早发生时间
        for (int i = 0; i < g->vertex_num; ++i) {
            printf("Earliest time for %c: %d\n", g->adj_list[i].data, earliest[i]);
        }

        // 初始化最晚发生时间
        for (int i = 0; i < g->vertex_num; ++i) {
            latest[i] = earliest[g->vertex_num - 1];    // 最后一个顶点的最晚发生时间为最早发生时间
        }

        while (!s2.empty()) {
            int curr = s2.top();
            s2.pop();
            EdgeNode *e = g->adj_list[curr].head;                                   // 获取当前顶点的边结点
            while (e != nullptr) {
                int next = e->edge_vex;                                             // 获取下一个顶点
                latest[curr] = std::min(latest[curr], latest[next] - e->weight);    // 更新最晚发生时间
                e = e->next;                                                        // 移动到下一个边结点
            }
        }

        // 输出最晚发生时间
        for (int i = 0; i < g->vertex_num; ++i) {
            printf("Latest time for %c: %d\n", g->adj_list[i].data, latest[i]);
        }

        // 输出关键路径
        printf("Critical Path: ");
        for (int i = 0; i < g->vertex_num; ++i) {
            if (earliest[i] == latest[i]) {    // 如果最早发生时间和最晚发生时间相等，说明是关键活动
                printf("%c ", g->adj_list[i].data);
            }
        }
    }
}

int main() {
    Graph g;
    createGraph(&g);
    AdjGraph adj_g;
    createAdjGraph(&g, &adj_g);    // 创建邻接表
    criticalPath(&adj_g);
    return 0;
}
