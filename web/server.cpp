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
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .origin("*");

    app.loglevel(crow::LogLevel::Warning);

    // ============================================================
    // THAY ĐỔI TẠI ĐÂY: XỬ LÝ FILE GIAO DIỆN (STATIC FILES)
    // ============================================================

    // 1. Route cho trang chủ (http://ip:18080/)
    CROW_ROUTE(app, "/")
    ([](const crow::request& req, crow::response& res) {
        res.set_static_file_info("web/static/index.html");
        res.end();
    });

    // 2. Route tự động cho CSS, JS, Images (ví dụ: web/style.css)
    CROW_ROUTE(app, "/static/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename) {
        res.set_static_file_info("web/static/" + filename);
        res.end();
    });
    
    // ============================================================
    // CÁC ROUTE API CŨ CỦA ÔNG (GIỮ NGUYÊN)
    // ============================================================

    CROW_ROUTE(app, "/upload_video").methods("POST"_method)
    ([](const crow::request& req) {
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
    ([](std::string filename) {
        std::string path = "uploads/" + filename;
        if (!fs::exists(path)) return crow::response(404, "File not found");

        crow::response res;
        res.set_static_file_info(path);
        res.add_header("Content-Type", "video/mp4"); 
        return res;
    });

    CROW_ROUTE(app, "/courses")([] {
        crow::json::wvalue res;
        int i = 0;
        Course* cur = store.first();
        if (!cur) return res;
        do {
            res[i]["id"] = cur->id;
            res[i]["title"] = cur->title;
            res[i]["rating"] = cur->rating;
            i++;
        } while (store.can_next() && (cur = store.next()));
        return res;
    });

    CROW_ROUTE(app, "/course/<int>")
    ([](int id){
        crow::json::wvalue res;
        Course* c = store.get_by_id(id);
        if (!c) return crow::response(404);
        res["id"] = c->id;
        res["title"] = c->title;
        res["rating"] = c->rating;
        for (size_t i = 0; i < c->lessons.size(); i++) {
            res["lessons"][i]["title"] = c->lessons[i].title;
            res["lessons"][i]["video_url"] = c->lessons[i].video_url;
        }
        auto rec_ids = graph.recommend_related(id, 4);
        for (size_t i = 0; i < rec_ids.size(); i++) {
            Course* rc = store.get_by_id(rec_ids[i]);
            if (rc) {
                res["recommendations"][i]["id"] = rc->id;
                res["recommendations"][i]["title"] = rc->title;
                res["recommendations"][i]["rating"] = rc->rating;
            }
        }
        return crow::response(res);
    });

    CROW_ROUTE(app, "/search")
    ([](const crow::request& req){
        crow::json::wvalue res;
        auto prefix = req.url_params.get("prefix");
        if (!prefix) return res;
        auto results = store.autocomplete_title(prefix);
        for (size_t i = 0; i < results.size(); i++) {
            res[i] = results[i];
        }
        return res;
    });

    CROW_ROUTE(app, "/recommend/<int>")
    ([](int id){
        crow::json::wvalue res;
        auto rec = graph.recommend_related(id);
        for (size_t i = 0; i < rec.size(); i++) {
            Course* c = store.get_by_id(rec[i]);
            if (c) res[i] = c->title;
        }
        return res;
    });

    CROW_ROUTE(app, "/recommend_friends/<int>")
    ([&](int userId){
        crow::json::wvalue res;
        auto friends = user_graph.recommend_friends(userId, 5);
        for (size_t i = 0; i < friends.size(); i++) {
          res[i] = friends[i]; 
        }
        return res;
    });

    CROW_ROUTE(app, "/send_message").methods("POST"_method)
    ([&](const crow::request& req){
        auto data = crow::json::load(req.body);
        if (!data) return crow::response(400);
        
        int to = data["to"].i();
        user_mailboxes[to].send(data["from"].i(), to, data["content"].s());
        
        return crow::response(200, "Sent");
    });

    CROW_ROUTE(app, "/receive_message/<int>")
    ([&](int myId){
        crow::json::wvalue res;
        if (user_mailboxes.count(myId) && !user_mailboxes[myId].empty()) {
            Message m = user_mailboxes[myId].receive();
            res["from"] = m.from;
            res["content"] = m.content;
            res["to"] = m.to;
        }
        return res;
    });

    CROW_ROUTE(app, "/user_info/<int>")
    ([&](int id){
        crow::json::wvalue res;
        res["id"] = id;
        res["name"] = "User " + std::to_string(id);
        return res;
    });

    CROW_ROUTE(app, "/add_friend").methods("POST"_method)
    ([&](const crow::request& req){
        auto data = crow::json::load(req.body);
        user_graph.add_friendship(data["from"].i(), data["to"].i());
        return crow::response(200, "Friend added");
    });

    CROW_ROUTE(app, "/get_friends/<int>")
    ([&](int userId){
        crow::json::wvalue res;
        std::vector<int> friends = user_graph.recommend_friends(userId, 10); 
        for (size_t i = 0; i < friends.size(); i++) {
             res[i]["id"] = friends[i];
             res[i]["name"] = "User " + std::to_string(friends[i]);
        }
        return res;
    });

    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([&](const crow::request& req){
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
        
        std::cout << "Đã đăng ký: " << user << std::endl;
        return crow::response(200, "Success");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)
    ([&](const crow::request& req){
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

    std::cout << "Server running at http://0.0.0.0:18080\n";
    app.port(18080).multithreaded().run();
}