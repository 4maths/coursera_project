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

struct Lesson {
    int id;
    std::string title;
    std::string video_url; // Link video (Local path, S3 hoặc YouTube)
};

struct Course {
    int id;
    std::string title;
    Domain domain;
    float rating;
    std::string thumbnail_url;
    
    // For learning path (DAG) và Content
    std::vector<int> prereq_ids; 
    std::vector<Lesson> lessons;  // Danh sách bài học trong khóa
    std::vector<int> related_ids; // Để gợi ý nhanh
};

// CHỈ GIỮ LẠI MỘT ĐỊNH NGHĨA USER DUY NHẤT
struct User {
    int id;
    std::string username; // Tên tài khoản để đăng nhập
    std::string password; // Mật khẩu
    std::string name;     // Tên hiển thị

    // Fields of interest
    std::vector<Domain> interests;

    // Learning data
    std::unordered_set<int> enrolled_courses;
    std::unordered_set<int> completed_courses;

    // Social (ID của bạn bè trong đồ thị)
    std::unordered_set<int> friends;
};

struct Message {
    int from;
    int to;
    std::string content;
};