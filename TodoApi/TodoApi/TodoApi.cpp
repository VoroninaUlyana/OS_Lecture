#define CROW_MAIN
#define ASIO_STANDALONE
#include <sqlite3.h>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp> 
#include "crow_all.h"
#include <iostream>
using namespace std;
using json = nlohmann::json;
class Storage 
{
    sqlite3* db;
public:
    Storage() 
    {
        sqlite3_open("todo.db", &db);
        sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, description TEXT, status TEXT);", 0, 0, 0);
    }
    ~Storage() 
    { 
        sqlite3_close(db); 
    }
    int add(string t, string d, string s) 
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO tasks (title, description, status) VALUES (?, ?, ?);", -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, t.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, d.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, s.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int id = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
        return id;
    }
    json getAll() 
    {
        json res = json::array();
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT * FROM tasks;", -1, &stmt, 0);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            res.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"title", (const char*)sqlite3_column_text(stmt, 1)},
                {"status", (const char*)sqlite3_column_text(stmt, 3)}
                });
        }
        sqlite3_finalize(stmt);
        return res;
    }
};
int main() 
{
    crow::SimpleApp app;
    Storage db;
    CROW_ROUTE(app, "/")([]() 
        {
        return "To-Do API is running!";
        });
    CROW_ROUTE(app, "/tasks").methods("GET"_method)([&db]() 
        {
        return crow::response(db.getAll().dump());
        });
    CROW_ROUTE(app, "/tasks").methods("POST"_method)([&db](const crow::request& req) 
        {
        try 
        {
            auto body = json::parse(req.body);
            int id = db.add(
                body.value("title", "Untitled"),
                body.value("description", ""),
                body.value("status", "todo")
            );
            json res = body;
            res["id"] = id;
            return crow::response(201, res.dump());
        }
        catch (...) 
        { 
            return crow::response(400, "Invalid JSON"); 
        }
        });
    /*CROW_ROUTE(app, "/tasks").methods("GET"_method)([]()
        {
        json res = json::array();
        lock_guard<mutex> lock(tasks_mutex);
        for (const auto& t : tasks) {
            res.push_back({ {"id", t.id}, {"title", t.title}, {"status", t.status} });
        }
        return crow::response(res.dump());
        });
    CROW_ROUTE(app, "/tasks/<int>")([](int id) 
        {
        lock_guard<mutex> lock(tasks_mutex);
        for (const auto& t : tasks) 
        {
            if (t.id == id) return crow::response(t.to_json().dump());
        }
        return crow::response(404, "Task not found");
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("DELETE"_method)([](int id) 
        {
        lock_guard<mutex> lock(tasks_mutex);
        auto it = remove_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
        if (it != tasks.end()) 
        {
            tasks.erase(it, tasks.end());
            return crow::response(204);
        }
        return crow::response(404, "Task not found");
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("PATCH"_method)([](const crow::request& req, int id) 
        {
        try 
        {
            auto body = json::parse(req.body);
            lock_guard<mutex> lock(tasks_mutex);
            for (auto& t : tasks) 
            {
                if (t.id == id) 
                {
                    if (body.contains("status")) 
                    {
                        t.status = body["status"];
                    }
                    return crow::response(200, t.to_json().dump());
                }
            }
            return crow::response(404, "Task not found");
        }
        catch (...) 
        {
            return crow::response(400, "Invalid JSON");
        }
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("PUT"_method)([](const crow::request& req, int id) 
        {
        try 
        {
            auto body = json::parse(req.body);
            lock_guard<mutex> lock(tasks_mutex);
            for (auto& t : tasks) 
            {
                if (t.id == id) 
                {
                    t.title = body.value("title", t.title);
                    t.description = body.value("description", t.description);
                    t.status = body.value("status", t.status);
                    return crow::response(200, t.to_json().dump());
                }
            }
            return crow::response(404, "Task not found");
        }
        catch (...) 
        { 
            return crow::response(400, "Invalid JSON"); 
        }
        });*/
    app.port(18080).multithreaded().run();
    return 0;
}