// 结构组织：邻接表下包含顶点数组，每个顶点数组指向一个链表，储存该顶点的所有邻接点
// 这个形式与h, e, ne数组是一致的，只是那边用静态数组+idx来模拟而已，原理都是相同的
// idx起到了区分不同边的作用，同时作为e数组的索引对“交错”也有贡献

// 更具体地说，Edge其实是边结点，它是链表中的一个结点元素。但命名为EdgeNode过于冗长
// Node拉出来一条链表，相当于哨兵结点，它保存着链表的头指针
// 同一条链表上的边都是Node的出边，没有先后顺序之分
// 参考邻接表的示意图，每条单链表之间的元素是不会连接的，即next指针不会跨越链表。两条链表之间绝对没有指针连接。
// 但是毫无疑问，图中边的两端是两个顶点，一定是存在“交错”的，而上述结构没有体现这种交错
// 例如在dfs中，需要沿着图的方向走，不是沿着链表走
// for (Edge* curr = nodes[from].head; curr; curr = curr->next) {
//     int to_node = curr->to;
//     if (!visited[to_node]) {
//         dfs(to_node, visited);  // 递归后，from改变，进入新的链表
//     }
// }
// 可见，即使结构上没有体现交错，但逻辑上是存在交错的，从而可以满足有向图的遍历需求
// “交错”体现在Edge::to，指向的是另一个Node；而Edge::next是服从当前链表的
// 对于静态数组模拟来说，e[边]=点，e数组相当于to来体现交错，ne[边]=边，ne数组相当于next来体现链表关系

// 基于上面的讨论，将图转化为链表来考虑，是很容易想到，是否要用双向链表来表示无向图？
// 实践上，最常用的还是两次加边法，因此不必考虑双向链表

// 还有一点，经常会产生混淆：
// 上述提到的独立链表，我们在遍历时经常会用到，从链表头遍历到尾
// 但上述模式是基于邻接表，千万别忘了在图中是怎样的：
// 在图中，遍历每条独立的邻接表，实际上是在遍历一个顶点的所有出边！
// 千万不要混淆为类似链表的遍历方式！
// 自己要积极去想象遍历图的过程，久而久之就会形成正确的认知

// 边的结构体有两种：
// 1. 用于邻接表
// 2. 简单三元组，用于kruskal算法

// 邻接表使用于稀疏图
// 处理以下问题：
// 1. 拓扑排序
// 2. 关键路径
// 3. dijkstra算法
// 4. dfs和bfs遍历
// 5. 求连通分量（无向图）
// 6. prim最小生成树（无向图）

#include <algorithm>
#include <iostream>
#include <queue>

using namespace std;

class Graph {
private:
    // 边结点结构体，相当于结合了e,ne,w
    struct Edge {
        int to;
        int weight;
        Edge* next;

        Edge(int t, int w)
            : to(t)
            , weight(w)
            , next(nullptr) {}
    };

    // 结点结构体，结合了h,d
    struct Node {
        int in;
        Edge* head;

        Node()
            : in(0)
            , head(nullptr) {}
    };

    vector<Node> nodes;    // 节点数组，拉出来“二维链表”，也就是邻接表主体
    int node_num;
    int edge_num;

public:
    static const int MAX = 0x3f3f3f3f;

public:
    Graph(int n, int e)
        : node_num(n)
        , edge_num(e) {
        // 初始化节点，从1开始，0用不到
        for (int i = 0; i <= n; ++i) {
            nodes.push_back(Node());
        }
    }

    ~Graph() {
        // 释放每一条链表
        for (auto& node : nodes) {
            Edge* e = node.head;
            while (e) {
                Edge* to_del = e;
                e = e->next;
                delete to_del;
            }
        }
    }

    // 头插法，插入一条边，同时更新入度
    void addEdge(int from, int to, int weight = 1) {
        Edge* new_edge = new Edge(to, weight);
        new_edge->next = nodes[from].head;
        nodes[from].head = new_edge;
        nodes[to].in++;
    }

    // 注意，如果为无向图，需要删除两次边
    void deleteEdge(int from, int to) {
        for (Edge *curr = nodes[from].head, *pre = nullptr; curr;) {
            if (curr->to == to) {
                // 找到目标边，进行删除
                if (pre == nullptr) {
                    // 删除的是链表头
                    nodes[from].head = curr->next;
                } else {
                    pre->next = curr->next;
                }
                delete curr;
                nodes[to].in--;
                break;
            } else {
                pre = curr;
                curr = curr->next;
            }
        }
    }

    bool topoSort(vector<int>& topo_order) {
        queue<int> q;
        topo_order.clear();

        // 拓扑排序会破坏入度信息，因此需要对备份进行操作
        vector<int> curr_in_degree(node_num + 1);
        for (int i = 1; i <= node_num; ++i) {
            curr_in_degree[i] = nodes[i].in;
        }

        // 入度为0的节点入队
        for (int i = 1; i <= node_num; ++i) {
            if (curr_in_degree[i] == 0) {
                q.push(i);
            }
        }

        while (!q.empty()) {
            int t = q.front();
            q.pop();
            
            topo_order.push_back(t);

            for (Edge* e = nodes[t].head; e; e = e->next) {
                int to = e->to;
                curr_in_degree[to]--;
                if (curr_in_degree[to] == 0) {
                    q.push(to);
                }
            }
        }

        return topo_order.size() == node_num;
    }

    int dijkstra(int start_node = 1, int end_node = -1) {
        if (start_node < 1 || start_node > node_num || (end_node != -1 && (end_node < 1 || end_node > node_num))) {
            return -2;    // 输入节点不合法
        }
        if (end_node == -1) {
            end_node = node_num;
        }

        vector<int> dist(node_num + 1, MAX);
        dist[start_node] = 0;
        vector<bool> vis(node_num + 1, false);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> heap;
        heap.push({0, start_node});

        while (!heap.empty()) {
            auto t = heap.top();
            heap.pop();

            int curr_dist = t.first;
            int curr_node = t.second;

            // 剪枝
            if (curr_node == end_node) {
                return curr_dist;
            }

            if (vis[curr_node]) {
                continue;
            }
            vis[curr_node] = true;

            for (Edge* e = nodes[curr_node].head; e; e = e->next) {
                int to = e->to;
                int weight = e->weight;

                if (dist[to] > curr_dist + weight) {
                    dist[to] = curr_dist + weight;
                    heap.push({dist[to], to});
                }
            }
        }

        return dist[end_node] == MAX ? -1 : dist[end_node];
    }

    // 层序遍历
    void bfs(int start = 1) {
        if (start < 1 || start > node_num) {
            return;    // 输入节点不合法
        }

        queue<int> q;
        vector<bool> vis(node_num + 1, false);
        q.push(start);
        vis[start] = true;

        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            cout << curr << " ";

            for (Edge* e = nodes[curr].head; e; e = e->next) {
                int to = e->to;

                if (vis[to]) {
                    continue;
                }

                vis[to] = true;
                q.push(to);
            }
        }
        cout << endl;
    }

    void dfs(int node, vector<bool>& vis) {
        vis[node] = true;

        for (Edge* e = nodes[node].head; e; e = e->next) {
            int to = e->to;
            if (!vis[to]) {
                dfs(to, vis);
            }
        }
    }

    int getConnectedComponents() {
        // 连通块问题，即若连通分量，用dfs
        // 注意！本函数只能在无向图中使用，需要保证addEdge时双向加边，否则无效
        // 强连通问题需要用强连通分量算法，这里不涉及
        vector<bool> vis(node_num + 1, false);
        int components = 0;

        for (int i = 1; i <= node_num; ++i) {
            if (!vis[i]) {
                components++;
                dfs(i, vis);
            }
        }
        return components;
    }

    // 前提：有向无环图
    // 完成图中的所有事件，最耗时的路径，即为关键路径
    // 最早开始时间和最晚开始时间，是保证在不影响总工期的前提下，某事件可以开始的最早和最晚时间
    // 因此，关键路径上的活动的最早开始时间和最晚开始时间相等，因为本来就是最耗时的，必须按时完成

    // AOE网，即activity on edge network，活动在边上
    // 我们称顶点为事件，边为活动
    // ve：某事件的最早发生时间
    // vl：某事件的最晚发生时间
    // e：某活动的最早开始时间
    // l：某活动的最晚开始时间
    // 事件本身并不耗时，耗时的是事件之间的活动。即点没有权，有权的是边

    // ========ve计算========
    // 当一个点入度不为0时，说明它有前驱事件；这很容易联想到拓扑排序
    // 只有最耗时的前驱事件完成后，再完成它与当前事件之间的活动，当前活动才能开始
    // 那么这里的活动其实就是边权，注意类比
    // 对于起点来说，ve=0，因为没有前驱事件
    // 对于非起点来说，ve=max(ve(所有前驱事件)+当前活动时间)

    // ========vl计算========
    // 终点的最早开始时间和最晚开始时间必然相等，因为终点必然在关键路径上
    // 因此，vl(终点)=ve(终点)
    // 其他点的vl先初始化为无穷大
    // 只有保证后继事件能够按时完成，当前事件的最晚时间才能往后拖延
    // 因此，vl=min(vl(所有后继事件)-当前活动时间)

    // 所有点的ve和vl都计算完成后，ve和vl相等的活动即为关键活动
    // 连接关键活动的路径即为关键路径
    // 人眼能一眼把图连起来，但计算机却做不到，所以下面需要求e和l

    // ========e和l的计算========
    // 设某事件为u，后继事件为v。u,v之间的活动为w
    // 显然我们要计算e(w)和l(w)
    // 当u可以开始时，w就可以开始了。那么，u越早开始，w就越早开始
    // 因此，e(w)=ve(u)
    // 另一方面，想要w尽可能晚，就要让v尽可能晚开始，同时保证w完成后，不再有多余的事件间隙
    // 也就是w完成后，恰好是v的最晚开始时间
    // 因此，l(w)=vl(v)-w

    // 下面回到关键路径，如何通过边的性质来判断该边是否属于关键路径：
    // 一条边属于关键路径，当且仅当 e(边)=l(边)
    // e(边)=ve(边的起点)，l(边)=vl(边的终点)-边权
    // 联立得，关键路径判断的充要条件为 ve(起点)=vl(终点)-边权
    // 可见，虽然关键路径的判断需要通过活动的最早和最晚开始时间来判断，但可以转化为事件。所以函数中只有两个数组来存储事件，而不是四个数组

    // 求关键路径：
    // 1. 拓扑排序求ve
    // 2. 逆拓扑排序求vl
    // 3. 遍历图里的每一条边，得到边的起点和终点，用 ve(起点)=vl(终点)-边权 来判断该边是否属于关键路径
    bool criticalPath(int start_node = 1, int end_node = -1) {
        if (start_node < 1 || start_node > node_num || (end_node != -1 && (end_node < 1 || end_node > node_num))) {
            if (start_node != end_node) {
                return false;
            }
        }
        if (end_node == -1) {
            end_node = node_num;
        }

        vector<int> earliest(node_num + 1, 0);
        vector<int> latest(node_num + 1, MAX);
        vector<int> topo_order;

        if (!topoSort(topo_order)) {
            return false;    // 有环，无法进行关键路径分析
        }

        // 根据拓扑排序，计算事件最早发生时间ve
        for (int node : topo_order) {
            // 遍历node的所有出边
            for (Edge* e = nodes[node].head; e; e = e->next) {
                int to = e->to;
                int weight = e->weight;
                earliest[to] = max(earliest[to], earliest[node] + weight);
            }
        }

        // 初始化终点的最晚发生时间
        latest[end_node] = earliest[end_node];

        // 计算最晚发生时间（逆拓扑序）
        // rbegin()和rend()返回的是反向迭代器，遍历顺序是从后向前
        for (auto it = topo_order.rbegin(); it != topo_order.rend(); ++it) {
            int node = *it;
            for (Edge* e = nodes[node].head; e; e = e->next) {
                int to = e->to;
                int weight = e->weight;
                latest[node] = min(latest[node], latest[to] - weight);
            }
        }

        // 输出关键活动
        for (int node = 1; node <= node_num; ++node) {
            for (Edge* e = nodes[node].head; e; e = e->next) {
                int to_node = e->to;
                int weight = e->weight;
                if (earliest[node] == latest[to_node] - weight) {
                    if (earliest[node] >= earliest[start_node] && latest[to_node] <= latest[end_node]) {
                        cout << "Critical Activity: " << node << " -> " << to_node << endl;
                    }
                }
            }
        }

        return true;
    }

    // 使用优先队列的prim算法，对于稀疏图来说性能也很高
    // 注意，必须为无向图才可使用
    int prim() {
        // min_weight[i]存储的是当前集合中所有点到外部点i的所有边中，权值最小的那条边的权值。
        vector<int> min_weight(node_num + 1, MAX);
        // 维护最小生成树mst集合
        vector<bool> in_mst(node_num + 1, false);
        // 高效地维护所有外部点到当前集合的最小距离，
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> heap;

        min_weight[1] = 0;
        heap.push({0, 1});

        int tot_weight = 0;
        int node_joined = 0;

        while (!heap.empty()) {
            // 取出当前距离集合最近的点
            auto t = heap.top();
            heap.pop();

            int curr_weight = t.first;
            int curr_node = t.second;

            if (in_mst[curr_node]) {
                continue;
            }

            in_mst[curr_node] = true;
            tot_weight += curr_weight;
            node_joined++;

            // 松弛操作：遍历所有与curr_node相连的边
            for (Edge* e = nodes[curr_node].head; e; e = e->next) {
                int to = e->to;
                int weight = e->weight;

                // e这条边是直接与curr_node相连的，本身权值为weight
                // 如果to尚未在集合中，且通过curr_node到达集合更短，则更新
                // curr_node已经在集合中了，且e又与之相连，那么直接加上weight，就是to到集合的距离
                if (!in_mst[to] && min_weight[to] > weight) {
                    min_weight[to] = weight;
                    heap.push({weight, to});
                }
            }
        }

        if (node_joined < node_num) {
            return -1;    // 图不连通，无法构成生成树
        }
        return tot_weight;
    }
};

int main() {
    int n, m;
    cin >> n >> m;
    Graph graph(n, m);

    for (int i = 0; i < m; ++i) {
        int from, to, weight;
        cin >> from >> to >> weight;
        graph.addEdge(from, to, weight);
    }

    return 0;
}
