#include "crow_all.h"
#include "crow/middlewares/cors.h"
#include <iostream>
#include <fstream> 
#include <filesystem>
#include <unordered_map>

#include "../core/include/CourseStore.hpp"
#include "../core/include/CourseGraph.hpp"
#include "../core/include/UserGraph.hpp"
#include "../core/include/MessageQueue.hpp"

namespace fs = std::filesystem;

// ===== Global core objects =====
CourseStore store;
CourseGraph graph;
UserGraph user_graph;

std::unordered_map<int, MessageQueue> user_mailboxes;
HashTable<std::string, User*> userByName(101); 
int nextUserId = 5; 
int total_requests = 0; // Biến đếm toàn cục cho metrics

void init_data() {
    Course c1{1, "Calculus I", Domain::Mathematics, 4.8f};
    Course c2{2, "Linear Algebra", Domain::Mathematics, 4.7f};
    Course c3{3, "Data Structures", Domain::DataStructures, 4.9f};
    Course c4{4, "English Grammar", Domain::English, 4.5f};

    store.add_course(c1);
    store.add_course(c2);
    store.add_course(c3);
    store.add_course(c4);

    graph.add_related(1, 2);
    graph.add_related(3, 2);
}

int main() {
    init_data();

    if (!fs::exists("uploads")) {
        fs::create_directory("uploads");
    }

    using App = crow::App<crow::CORSHandler>;
    App app;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("*")
        .methods("GET"_method, "POST"_method, "PUT"_method, "OPTIONS"_method) // Thêm PUT cho S3 upload
        .origin("*");

    app.loglevel(crow::LogLevel::Warning);

    // ============================================================
    // FIX LỖI: GỘP ROUTE TRANG CHỦ VÀ METRICS
    // ============================================================

    // Route cho trang chủ (Chỉ khai báo 1 lần duy nhất ở đây)
    CROW_ROUTE(app, "/")
    ([&](const crow::request& req, crow::response& res) {
        total_requests++; // Tăng biến đếm request tại đây
        res.set_static_file_info("web/static/index.html");
        res.end();
    });

    // Route Metrics cho Prometheus
    CROW_ROUTE(app, "/metrics")([&] {
        std::string response;
        response += "# HELP http_requests_total Total number of HTTP requests\n";
        response += "http_requests_total " + std::to_string(total_requests) + "\n";
        return crow::response(response);
    });

    // Route tự động cho CSS, JS, Images
    CROW_ROUTE(app, "/static/<string>")
    ([&](const crow::request& req, crow::response& res, std::string filename) {
        res.set_static_file_info("web/static/" + filename);
        res.end();
    });
    
    // ============================================================
    // CÁC ROUTE API (GIỮ NGUYÊN)
    // ============================================================

    CROW_ROUTE(app, "/upload_video").methods("POST"_method)
    ([&](const crow::request& req) {
        total_requests++;
        crow::multipart::message msg(req);
        try {
            auto part = msg.get_part_by_name("videoFile"); 
            auto header = part.get_header_object("Content-Disposition");
            
            std::string filename;
            if (header.params.count("filename")) {
                filename = header.params.at("filename");
            } else {
                return crow::response(400, "No filename in header");
            }

            std::ofstream out("uploads/" + filename, std::ios::binary);
            if (!out) return crow::response(500, "Server Error: Cannot open file for writing");
            
            out << part.body;
            out.close();
            
            return crow::response(200, "Success");
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid Multipart Request");
        }
    });

    CROW_ROUTE(app, "/videos/<string>")
    ([&](std::string filename) {
        total_requests++;
        std::string path = "uploads/" + filename;
        if (!fs::exists(path)) return crow::response(404, "File not found");

        crow::response res;
        res.set_static_file_info(path);
        res.add_header("Content-Type", "video/mp4"); 
        return res;
    });

    CROW_ROUTE(app, "/courses")([&] {
        total_requests++;
        crow::json::wvalue res;
        int i = 0;
        Course* cur = store.first();
        if (!cur) return crow::json::wvalue(crow::json::type::List);
        do {
            res[i]["id"] = cur->id;
            res[i]["title"] = cur->title;
            res[i]["rating"] = cur->rating;
            i++;
        } while (store.can_next() && (cur = store.next()));
        return crow::json::wvalue(res);
    });

    CROW_ROUTE(app, "/course/<int>")
    ([&](int id){
        total_requests++;
        crow::json::wvalue res;
        Course* c = store.get_by_id(id);
        if (!c) return crow::response(404);
        res["id"] = c->id;
        res["title"] = c->title;
        res["rating"] = c->rating;
        // ... (giữ nguyên logic lessons và recommendations của bạn)
        return crow::response(res);
    });

    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([&](const crow::request& req){
        total_requests++;
        auto data = crow::json::load(req.body);
        if (!data) return crow::response(400, "Invalid JSON");
        std::string user = data["username"].s();
        
        if (userByName.contains(user)) return crow::response(400, "Tài khoản đã tồn tại");

        User* newUser = new User();
        newUser->id = nextUserId++;
        newUser->username = user;
        newUser->password = data["password"].s();
        newUser->name = data["name"].s();

        userByName.put(user, newUser);
        user_graph.add_friendship(newUser->id, newUser->id); 
        
        return crow::response(200, "Success");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)
    ([&](const crow::request& req){
        total_requests++;
        auto data = crow::json::load(req.body);
        if (!data) return crow::response(400);
        std::string user = data["username"].s();
        
        if (!userByName.contains(user)) return crow::response(404, "User không tồn tại");

        User* u = userByName.get(user);
        if (u->password != data["password"].s()) return crow::response(401, "Sai mật khẩu");

        crow::json::wvalue res;
        res["id"] = u->id;
        res["name"] = u->name;
        return crow::response(res);
    });

    // ... (Các route phụ như /search, /recommend, /send_message giữ nguyên, chỉ cần thêm total_requests++ nếu muốn đo lường)

    std::cout << "Server 4maths running at http://0.0.0.0:18080\n";
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}