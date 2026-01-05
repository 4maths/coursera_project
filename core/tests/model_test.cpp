#include <iostream>
#include "models.hpp"

int main() {
    Course c1{1, "Calculus I", Domain::Mathematics, 4.6f, {}};
    Course c2{2, "Calculus II", Domain::Mathematics, 4.5f, {1}};
    Course c3{3, "English Grammar", Domain::English, 4.7f, {}};

    User u1{1, "Thai", {Domain::Mathematics, Domain::ComputerScience}, {}, {}, {}};
    User u2{2, "Dung", {Domain::English}, {}, {}, {}};

    Message m{1, 2, "Hi, want to study together?"};

    std::cout << "Course: " << c1.title << "\n";
    std::cout << "User: " << u1.name << "\n";
    std::cout << "Message: " << m.content << "\n";

    return 0;
}
