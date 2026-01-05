#pragma once
#include <unordered_map>
#include <vector>
#include <queue>

/*
 * UserGraph:
 * - Node: user id
 * - Edge: friendship / common interest
 * - BFS for friend recommendation
 */

class UserGraph {
private:
    std::unordered_map<int, std::vector<int>> adj;

public:
    void add_friendship(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // BFS friend recommendation
    std::vector<int> recommend_friends(int user, int limit = 5) const {
        std::unordered_map<int, bool> visited;
        std::queue<int> q;
        std::vector<int> result;

        visited[user] = true;
        q.push(user);

        while (!q.empty() && (int)result.size() < limit) {
            int u = q.front(); q.pop();
            auto it = adj.find(u);
            if (it == adj.end()) continue;

            for (int v : it->second) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                    result.push_back(v);
                    if ((int)result.size() >= limit) break;
                }
            }
        }
        return result;
    }
};
