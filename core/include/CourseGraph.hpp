#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <stdexcept>

/*
 * CourseGraph:
 * - Undirected edges: related courses (same domain)
 * - Directed edges: prerequisite -> course (learning path)
 */

class CourseGraph {
private:
    // adjacency list for related courses (undirected)
    std::unordered_map<int, std::vector<int>> related;

    // adjacency list for learning path (directed DAG)
    std::unordered_map<int, std::vector<int>> prereq_to;

public:
    // Add relation: two courses are related
    void add_related(int c1, int c2) {
        related[c1].push_back(c2);
        related[c2].push_back(c1);
    }

    // Add prerequisite: pre -> course
    void add_prerequisite(int pre, int course) {
        prereq_to[pre].push_back(course);
    }

    // ===== Recommendation (BFS) =====
    std::vector<int> recommend_related(int start, int limit = 5) const {
        std::vector<int> result;
        std::unordered_map<int, bool> visited;
        std::queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty() && (int)result.size() < limit) {
            int u = q.front(); q.pop();
            auto it = related.find(u);
            if (it == related.end()) continue;

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

    // ===== Learning Path (Topological Sort) =====
    std::vector<int> learning_path(const std::vector<int>& all_courses) const {
    std::unordered_map<int, int> indeg;

    // 1. init indeg = 0 cho TẤT CẢ course
    for (int c : all_courses)
        indeg[c] = 0;

    // 2. tính indeg từ prerequisite graph
    for (const auto& p : prereq_to) {
        for (int v : p.second)
            indeg[v]++;
    }

    // 3. topo sort
    std::queue<int> q;
    for (const auto& kv : indeg)
        if (kv.second == 0)
            q.push(kv.first);

    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);

        auto it = prereq_to.find(u);
        if (it == prereq_to.end()) continue;

        for (int v : it->second) {
            if (--indeg[v] == 0)
                q.push(v);
        }
    }

    return order;
}
};