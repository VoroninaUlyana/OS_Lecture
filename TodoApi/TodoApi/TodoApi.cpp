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
    void removeTask(int id) 
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "DELETE FROM tasks WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
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
    void updateStatus(int id, string status) 
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "UPDATE tasks SET status = ? WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    void updateFull(int id, string title, string desc, string status) 
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "UPDATE tasks SET title = ?, description = ?, status = ? WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, desc.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    json getById(int id) 
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT * FROM tasks WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, id);
        json res = json::object();
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            res = {
                {"id", sqlite3_column_int(stmt, 0)},
                {"title", (const char*)sqlite3_column_text(stmt, 1)},
                {"description", (const char*)sqlite3_column_text(stmt, 2)},
                {"status", (const char*)sqlite3_column_text(stmt, 3)}
            };
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
    CROW_ROUTE(app, "/tasks/<int>").methods("DELETE"_method)([&db](int id) 
        {
        db.removeTask(id);
        return crow::response(204);
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("PATCH"_method)([&db](const crow::request& req, int id) 
        {
        try 
        {
            auto body = json::parse(req.body);
            if (body.contains("status")) 
            {
                db.updateStatus(id, body["status"]);
                return crow::response(200, "Status updated");
            }
            return crow::response(400, "Missing status");
        }
        catch (...) 
        { 
            return crow::response(400, "Invalid JSON"); 
        }
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("PUT"_method)([&db](const crow::request& req, int id) 
        {
        try 
        {
            auto body = json::parse(req.body);
            db.updateFull(
                id,
                body.value("title", "Updated Title"),
                body.value("description", ""),
                body.value("status", "todo")
            );
            return crow::response(200, "Task updated");
        }
        catch (...) 
        { 
            return crow::response(400, "Invalid JSON"); 
        }
        });
    CROW_ROUTE(app, "/tasks/<int>").methods("GET"_method)([&db](int id) 
        {
        json res = db.getById(id);
        if (res.empty()) 
            return crow::response(404, "Task not found");
        return crow::response(res.dump());
        });
    app.port(18080).multithreaded().run();
    return 0;
}