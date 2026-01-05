#pragma once
#include <vector>
#include <list>
#include <utility>
#include <functional>
#include <stdexcept>

template<typename K, typename V>
class HashTable {
private:
    std::vector<std::list<std::pair<K, V>>> buckets;
    size_t n;

    size_t index_of(const K& k) const {
        return std::hash<K>{}(k) % buckets.size();
    }

public:
    explicit HashTable(size_t m = 101) : buckets(m), n(0) {}

    void put(const K& k, const V& v) {
        auto& bucket = buckets[index_of(k)];
        for (auto& kv : bucket) {
            if (kv.first == k) {
                kv.second = v;
                return;
            }
        }
        bucket.push_back(std::make_pair(k, v));
        n++;
    }

    bool contains(const K& k) const {
        const auto& bucket = buckets[index_of(k)];
        for (const auto& kv : bucket) {
            if (kv.first == k) return true;
        }
        return false;
    }

    V& get(const K& k) {
        auto& bucket = buckets[index_of(k)];
        for (auto& kv : bucket) {
            if (kv.first == k) return kv.second;
        }
        throw std::out_of_range("Key not found");
    }

    const V& get(const K& k) const {
        const auto& bucket = buckets[index_of(k)];
        for (const auto& kv : bucket) {
            if (kv.first == k) return kv.second;
        }
        throw std::out_of_range("Key not found");
    }

    size_t size() const { return n; }
};
