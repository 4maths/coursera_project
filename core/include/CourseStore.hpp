#pragma once
#include <string>
#include <list>
#include "models.hpp"
#include "DoublyLinkedList.hpp"
#include "HashTable.hpp"
#include "Trie.hpp"

/*
 * CourseStore:
 * - Stores Course objects (owned by CourseStore)
 * - Provides:
 *   + Doubly linked list traversal (catalog order)
 *   + Hash lookups: by id, by title
 *   + Trie autocomplete on titles
 */

class CourseStore {
private:
    std::list<Course> owned;                 // owns Course memory
    DoublyLinkedList<Course*> catalog;         // order list
    HashTable<int, Course*> byId;
    HashTable<std::string, Course*> byTitle;
    Trie titleTrie;

public:
    CourseStore() : byId(101), byTitle(101) {}

    Course* add_course(const Course& c) {
        owned.push_back(c);
        Course* ptr = &owned.back();

        catalog.push_back(ptr);
        byId.put(ptr->id, ptr);
        byTitle.put(ptr->title, ptr);
        titleTrie.insert(ptr->title);

        return ptr;
    }

    Course* get_by_id(int id) {
        return byId.get(id);
    }

    Course* get_by_title(const std::string& title) {
        return byTitle.get(title);
    }

    std::vector<std::string> autocomplete_title(const std::string& prefix, int limit = 8) const {
        return titleTrie.autocomplete(prefix, limit);
    }

    // Catalog navigation (DLL cursor)
    Course* first() {
        catalog.reset_to_head();
        return catalog.current();
    }

    bool can_next() const { return catalog.can_next(); }
    bool can_prev() const { return catalog.can_prev(); }

    Course* next() { return catalog.next(); }
    Course* prev() { return catalog.prev(); }
};
