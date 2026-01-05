#pragma once
#include <string>
#include <vector>
#include <unordered_set>

/*
 * Data models for a multi-domain learning platform.
 */

enum class Domain {
    Mathematics,
    Physics,
    Chemistry,
    Literature,
    English,
    Biology,
    ComputerScience,
    DataStructures,
    CloudDevOps
};

struct Course {
    int id;
    std::string title;
    Domain domain;
    float rating;

    // For learning path (DAG)
    std::vector<int> prereq_ids;
};

struct User {
    int id;
    std::string name;

    // Fields of interest
    std::vector<Domain> interests;

    // Learning data
    std::unordered_set<int> enrolled_courses;
    std::unordered_set<int> completed_courses;

    // Social
    std::unordered_set<int> friends;
};

struct Message {
    int from;
    int to;
    std::string content;
};
