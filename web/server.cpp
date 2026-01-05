#include "crow_all.h"
#include <iostream>

#include "../core/include/CourseStore.hpp"
#include "../core/include/CourseGraph.hpp"

// ===== Global core objects =====
CourseStore store;
CourseGraph graph;

// ===== Init sample data =====
void init_data() {
    Course c1;
    c1.id = 1;
    c1.title = "Calculus I";
    c1.rating = 4.8;
    store.add_course(c1);

    Course c2;
    c2.id = 2;
    c2.title = "Linear Algebra";
    c2.rating = 4.7;
    store.add_course(c2);

    Course c3;
    c3.id = 3;
    c3.title = "Data Structures";
    c3.rating = 4.9;
    store.add_course(c3);

    Course c4;
    c4.id = 4;
    c4.title = "English Grammar";
    c4.rating = 4.5;
    store.add_course(c4);

    graph.add_related(1, 2);
    graph.add_related(3, 2);
}

int main() {
    init_data();

    crow::SimpleApp app;

    // ===== GET /courses =====
    CROW_ROUTE(app, "/courses")([] {
        crow::json::wvalue res;
        int i = 0;

        Course* cur = store.first();
        if (!cur) return res;

        res[i]["id"] = cur->id;
        res[i]["title"] = cur->title;
        res[i]["rating"] = cur->rating;
        i++;

        while (store.can_next()) {
            cur = store.next();
            res[i]["id"] = cur->id;
            res[i]["title"] = cur->title;
            res[i]["rating"] = cur->rating;
            i++;
        }

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

    // ===== GET /recommend/1 =====
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
