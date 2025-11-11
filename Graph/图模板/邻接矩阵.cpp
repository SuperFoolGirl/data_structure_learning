// 邻接矩阵，处理稠密图
// 1. prim最小生成树（无向图）
// 2. Floyd最短路径
// 3. dfs和bfs
// 4. 拓扑排序
// 5. 关键路径
// 6. dijkstra

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

class Graph {
private:
    int node_num;
    int edge_num;
    vector<vector<int>> matrix;
    vector<int> in_degree;

public:
    static const int MAX = 0x3f3f3f3f;

public:
    Graph(int n, int e)
        : node_num(n)
        , edge_num(e) {
        matrix.resize(n + 1, vector<int>(n + 1, MAX));
        in_degree.resize(n + 1, 0);

        // 自环边权为0
        for (int i = 1; i <= n; ++i) {
            matrix[i][i] = 0;
        }
    }

    void addEdge(int from, int to, int weight) {
        // 正确处理重边，存储较小的边权
        if (matrix[from][to] > weight) {
            if (matrix[from][to] == MAX) {
                in_degree[to]++;
            }
            matrix[from][to] = weight;
        }
    }

    void deleteEdge(int from, int to) {
        if (matrix[from][to] != MAX) {
            matrix[from][to] = MAX;
            in_degree[to]--;
        }
    }

    vector<vector<int>> floyd() {
        vector<vector<int>> dist = matrix;    // 初始化距离矩阵

        for (int k = 1; k <= node_num; ++k) {
            for (int i = 1; i <= node_num; ++i) {
                for (int j = 1; j <= node_num; ++j) {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }

        return dist;
    }

    // 稠密图不需要堆优化
    int prim() {
        vector<int> min_weight(node_num + 1, MAX);
        vector<bool> in_mst(node_num + 1, false);
        min_weight[1] = 0;

        int tot_weight = 0;

        // 将所有结点加入mst集合，i只代表循环次数
        for (int i = 1; i <= node_num; ++i) {
            int curr_node = -1;

            // 每轮循环找到当前最小边，只考虑不在mst集合中的点
            for (int j = 1; j <= node_num; ++j) {
                if (!in_mst[j] && (curr_node == -1 || min_weight[j] < min_weight[curr_node])) {
                    curr_node = j;
                }
            }

            if (curr_node == -1) {
                break;    // 剩下的节点均不可达
            }

            if (min_weight[curr_node] == MAX) {
                return -1;    // 图不连通，无法构成生成树
            }

            in_mst[curr_node] = true;
            tot_weight += min_weight[curr_node];

            // 松弛操作：基于此轮集合中加入了curr_node
            for (int j = 1; j <= node_num; ++j) {
                // 更新不在mst集合中的点到集合的最小距离
                // 如果通过curr_node到达j更近，则更新
                if (!in_mst[j]) {
                    min_weight[j] = min(min_weight[j], matrix[curr_node][j]);
                }
            }
        }

        return tot_weight;
    }

    // 使用朴素版dijkstra算法，堆优化会在稠密图中变慢
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

        for (int i = 1; i <= node_num; ++i) {
            int curr_node = -1;
            for (int j = 1; j <= node_num; ++j) {
                if (!vis[j] && (curr_node == -1 || dist[j] < dist[curr_node])) {
                    curr_node = j;
                }
            }

            if (curr_node == -1 || dist[curr_node] == MAX) {
                break;    // 剩余节点不可达
            }

            vis[curr_node] = true;
            if (curr_node == end_node) {
                return dist[end_node];
            }

            for (int j = 1; j <= node_num; ++j) {
                int weight = matrix[curr_node][j];
                if (!vis[j] && weight != MAX) {
                    dist[j] = min(dist[j], dist[curr_node] + weight);
                }
            }
        }

        return dist[end_node] == MAX ? -1 : dist[end_node];
    }

    bool topoSort(vector<int>& topo_order) {
        vector<int> curr_in_degree = in_degree;
        queue<int> q;

        for (int i = 1; i <= node_num; ++i) {
            if (curr_in_degree[i] == 0) {
                q.push(i);
            }
        }

        while (!q.empty()) {
            int t = q.front();
            q.pop();
            topo_order.push_back(t);

            for (int to = 1; to <= node_num; ++to) {
                if (matrix[t][to] != MAX) {
                    curr_in_degree[to]--;
                    if (curr_in_degree[to] == 0) {
                        q.push(to);
                    }
                }
            }
        }

        return topo_order.size() == node_num;
    }

    void bfs(int start = 1) {
        if (start < 1 || start > node_num) {
            return;
        }

        queue<int> q;
        vector<bool> vis(node_num + 1, false);
        q.push(start);
        vis[start] = true;

        while (!q.empty()) {
            int t = q.front();
            q.pop();

            cout << t << " ";

            for (int to = 1; to <= node_num; ++to) {
                if (matrix[t][to] != MAX && !vis[to]) {
                    vis[to] = true;
                    q.push(to);
                }
            }
        }
    }

    void dfs(int node, vector<bool>& vis) {
        vis[node] = true;

        for (int to = 1; to <= node_num; ++to) {
            if (matrix[node][to] != MAX && !vis[to]) {
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

    bool criticalPath() {
        vector<int> earliest(node_num + 1, 0);
        vector<int> latest(node_num + 1, 0x3f3f3f3f);
        vector<int> topo_order;

        if (!topoSort(topo_order)) {
            return false;    // 有环，无法进行关键路径分析
        }

        int start_node = topo_order.front();
        int end_node = topo_order.back();

        // 计算最早发生时间
        for (int node : topo_order) {
            for (int to = 1; to <= node_num; ++to) {
                if (matrix[node][to] != MAX) {
                    int weight = matrix[node][to];
                    earliest[to] = max(earliest[to], earliest[node] + weight);
                }
            }
        }
        // 输出最早发生时间
        for (int i = 1; i <= node_num; ++i) {
            cout << "Earliest time of node " << i << ": " << earliest[i] << endl;
        }
        // 初始化终点的最晚发生时间
        latest[end_node] = earliest[end_node];
        // 计算最晚发生时间（逆拓扑序）
        for (auto it = topo_order.rbegin(); it != topo_order.rend(); ++it) {
            int node = *it;
            for (int to = 1; to <= node_num; ++to) {
                if (matrix[node][to] != MAX) {
                    int weight = matrix[node][to];
                    latest[node] = min(latest[node], latest[to] - weight);
                }
            }
        }
        // 输出最晚发生时间
        for (int i = 1; i <= node_num; ++i) {
            cout << "Latest time of node " << i << ": " << latest[i] << endl;
        }
        // 输出关键活动
        for (int node = 1; node <= node_num; ++node) {
            for (int to = 1; to <= node_num; ++to) {
                if (matrix[node][to] != MAX) {
                    int weight = matrix[node][to];
                    if (earliest[node] == latest[to] - weight) {
                        if (earliest[node] >= earliest[start_node] && latest[to] <= latest[end_node]) {
                            cout << "Critical Activity: " << node << " -> " << to << endl;
                        }
                    }
                }
            }
        }
        return true;
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
