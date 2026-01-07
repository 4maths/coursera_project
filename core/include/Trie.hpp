    #pragma once
    #include <string>
    #include <vector>
    #include <array>

    class Trie {
    private:
        struct Node {
            std::array<int, 128> next;
            bool is_end;
            Node() : is_end(false) { next.fill(-1); }
        };

        std::vector<Node> nodes;

        void dfs_collect(int u, std::string& cur,
                        std::vector<std::string>& out, int limit) const
        {
            if ((int)out.size() >= limit) return;

            if (nodes[u].is_end) {
                out.push_back(cur);
                if ((int)out.size() >= limit) return;
            }

            for (int c = 0; c < 128; ++c) {
                int v = nodes[u].next[c];
                if (v != -1) {
                    cur.push_back((char)c);
                    dfs_collect(v, cur, out, limit);
                    cur.pop_back();
                }
            }
        }

    public:
        Trie() { nodes.emplace_back(); } // root

        void insert(const std::string& s) {
            int u = 0;
            for (unsigned char ch : s) {
                if (nodes[u].next[ch] == -1) {
                    nodes[u].next[ch] = (int)nodes.size();
                    nodes.emplace_back();
                }
                u = nodes[u].next[ch];
            }
            nodes[u].is_end = true;
        }

        std::vector<std::string> autocomplete(const std::string& prefix, int limit = 8) const {
            int u = 0;
            for (unsigned char ch : prefix) {
                if (nodes[u].next[ch] == -1) return {};
                u = nodes[u].next[ch];
            }
            std::vector<std::string> out;
            std::string cur = prefix;
            dfs_collect(u, cur, out, limit);
            return out;
        }
    };
