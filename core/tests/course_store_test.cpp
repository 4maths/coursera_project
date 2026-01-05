#include <iostream>
#include <vector>
#include "CourseStore.hpp"

static void print_auto(const std::vector<std::string>& v) {
    if (v.empty()) { std::cout << "(no result)\n"; return; }
    for (const auto& s : v) std::cout << "  " << s << "\n";
}

int main() {
    CourseStore store;

    // Multi-domain courses
    store.add_course({1, "Calculus I", Domain::Mathematics, 4.6f, {}});
    store.add_course({2, "Calculus II", Domain::Mathematics, 4.5f, {1}});
    store.add_course({3, "Classical Mechanics", Domain::Physics, 4.4f, {}});
    store.add_course({4, "General Chemistry", Domain::Chemistry, 4.2f, {}});
    store.add_course({5, "English Grammar Basics", Domain::English, 4.7f, {}});
    store.add_course({6, "Vietnamese Literature", Domain::Literature, 4.1f, {}});
    store.add_course({7, "Data Structures and Algorithms", Domain::DataStructures, 4.9f, {}});
    store.add_course({8, "Cloud Computing Fundamentals", Domain::CloudDevOps, 4.8f, {}});

    // Hash lookup
    Course* c = store.get_by_id(7);
    std::cout << "Lookup by id=7: " << c->title << " (rating=" << c->rating << ")\n";

    Course* c2 = store.get_by_title("Calculus I");
    std::cout << "Lookup by title: " << c2->title << " (id=" << c2->id << ")\n";

    // Trie autocomplete
    std::cout << "\nAutocomplete \"Cal\":\n";
    print_auto(store.autocomplete_title("Cal"));

    std::cout << "\nAutocomplete \"Cl\":\n";
    print_auto(store.autocomplete_title("Cl"));

    std::cout << "\nAutocomplete \"Da\":\n";
    print_auto(store.autocomplete_title("Da"));

    // DLL catalog navigation
    std::cout << "\nCatalog forward (DLL next):\n";
    Course* cur = store.first();
    std::cout << "  " << cur->title << "\n";
    while (store.can_next()) {
        cur = store.next();
        std::cout << "  " << cur->title << "\n";
    }

    std::cout << "\nCatalog backward (DLL prev):\n";
    while (store.can_prev()) {
        cur = store.prev();
        std::cout << "  " << cur->title << "\n";
    }

    return 0;
}
