// 只有边结构，没有结点
// 用于处理kruskal最小生成树算法

#include <algorithm>
#include <iostream>
#include <vector>


using namespace std;

class Graph {
private:
    struct Edge {
        int from;
        int to;
        int weight;

        Edge(int f, int t, int w)
            : from(f)
            , to(t)
            , weight(w) {}

        bool operator<(const Edge& other) const {
            return weight < other.weight;
        }
    };

    int node_num;
    int edge_num;
    vector<Edge> edges;
    vector<int> parent;

public:
    Graph(int n, int m)
        : node_num(n)
        , edge_num(m) {
        edges.reserve(edge_num);
        parent.reserve(node_num + 1);
    }

    void addEdge(int from, int to, int weight) {
        edges.push_back({from, to, weight});
    }

    int kruskal() {
        int tot_weight = 0;
        int used_edges = 0;

        sort(edges.begin(), edges.end());

        for (int i = 1; i <= node_num; ++i) {
            parent[i] = i;
        }

        for (int i = 0; i < edge_num; ++i) {
            int root_from = find(edges[i].from);
            int root_to = find(edges[i].to);

            if (root_from != root_to) {
                parent[root_from] = root_to;
                tot_weight += edges[i].weight;
                used_edges++;
            }
        }

        if (used_edges < node_num - 1) {
            return -1;    // 无法构成最小生成树
        }
        return tot_weight;
    }

private:
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
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

    int res = graph.kruskal();
    if (res == -1) {
        cout << "无法构成最小生成树" << endl;
    } else {
        cout << res << endl;
    }

    return 0;
}
