#include <iostream>
#include <vector>
#include <string>
#include "models.hpp"
#include "CourseStore.hpp"
#include "CourseGraph.hpp"
#include "UserGraph.hpp"
#include "MessageQueue.hpp"

static void print_menu() {
    std::cout << "\n=== Coursera Lite (DSA Demo) ===\n";
    std::cout << "1. View course catalog\n";
    std::cout << "2. Search course (autocomplete)\n";
    std::cout << "3. Recommend related courses\n";
    std::cout << "4. Learning path\n";
    std::cout << "5. Recommend friends\n";
    std::cout << "6. Chat demo\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose: ";
}

int main() {
    CourseStore store;
    CourseGraph cgraph;
    UserGraph ugraph;
    MessageQueue mq;

    store.add_course({1, "Calculus I", Domain::Mathematics, 4.6f, {}});
    store.add_course({2, "Calculus II", Domain::Mathematics, 4.5f, {1}});
    store.add_course({3, "Linear Algebra", Domain::Mathematics, 4.7f, {}});

    cgraph.add_related(1, 2);
    cgraph.add_related(1, 3);
    cgraph.add_prerequisite(1, 2);

    std::vector<int> all_courses = {1,2,3};

    ugraph.add_friendship(1, 2);
    ugraph.add_friendship(2, 3);

    int choice;
    do {
        std::cout << "\n1. View catalog\n";
        std::cout << "2. Search\n";
        std::cout << "3. Recommend course\n";
        std::cout << "4. Learning path\n";
        std::cout << "5. Chat demo\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose: ";

        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            Course* c = store.first();
            std::cout << c->title << "\n";
            while (store.can_next()) {
                c = store.next();
                std::cout << c->title << "\n";
            }
        }
        else if (choice == 2) {
            std::string p;
            std::cout << "Prefix: ";
            std::getline(std::cin, p);
            for (auto& s : store.autocomplete_title(p))
                std::cout << s << "\n";
        }
        else if (choice == 3) {
            int id;
            std::cout << "Course id: ";
            std::cin >> id;
            for (int x : cgraph.recommend_related(id))
                std::cout << "Course " << x << "\n";
        }
        else if (choice == 4) {
            for (int x : cgraph.learning_path(all_courses))
                std::cout << "Course " << x << "\n";
        }
        else if (choice == 5) {
            mq.send(1,2,"Hello");
            mq.send(2,1,"Hi");
            while (!mq.empty()) {
                auto m = mq.receive();
                std::cout << m.from << "->" << m.to << ": " << m.content << "\n";
            }
        }

    } while (choice != 0);

    return 0;
}
