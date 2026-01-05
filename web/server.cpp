#include "crow_all.h"
#include "crow/middlewares/cors.h"
#include <iostream>

#include "../core/include/CourseStore.hpp"
#include "../core/include/CourseGraph.hpp"

// ===== Global core objects =====
CourseStore store;
CourseGraph graph;

// ===== Init sample data =====
void init_data() {
    // Thêm Domain vào vị trí thứ 3, rating chuyển xuống vị trí thứ 4
    // Đảm bảo dùng đúng enum class Domain từ models.hpp
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
    // Gọi hàm khởi tạo dữ liệu
    init_data();

    using App = crow::App<crow::CORSHandler>;
    App app;

    // ===== CORS CONFIG =====
    // Cấu hình này rất quan trọng để Frontend (HTML) có thể gọi được API
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("*")
        .methods("GET"_method)
        .origin("*");

    app.loglevel(crow::LogLevel::Warning);

    // ===== GET /courses =====
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

    // ===== GET /search?prefix=cal =====
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

    // ===== GET /recommend/<id> =====
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

    std::cout << "Server running at http://localhost:18080\n";
    app.port(18080).multithreaded().run();
}