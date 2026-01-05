#include <iostream>
#include "CourseGraph.hpp"

int main() {
    CourseGraph g;

    // All course IDs
    std::vector<int> all_courses = {1, 2, 3, 4};

    // Learning path
    g.add_prerequisite(1, 2);
    g.add_prerequisite(2, 4);
    g.add_prerequisite(3, 4);

    std::cout << "Learning path:\n";
    for (int c : g.learning_path(all_courses)) {
        std::cout << "  Course " << c << "\n";
    }
}
