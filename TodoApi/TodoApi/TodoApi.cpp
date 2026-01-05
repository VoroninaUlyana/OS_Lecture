#define CROW_MAIN
#define ASIO_STANDALONE
#include <sqlite3.h>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp> 
#include "crow_all.h"
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <thread>
#include <condition_variable>
using namespace std;
using json = nlohmann::json;
queue<string> message_queue;
mutex queue_mtx;
condition_variable queue_cv;
bool stop_worker = false;
crow::response send_error(int code, const std::string& msg) 
{
    nlohmann::json j;
    j["error"] = msg;
    j["code"] = code;
    auto res = crow::response(code, j.dump());
    res.set_header("Content-Type", "application/json");
    return res;
}
bool isValidStatus(const std::string& s) 
{
    return s == "todo" || s == "in_progress" || s == "done";
}
class Storage 
{
    sqlite3* db;
    unordered_map<int, json> cache;
    mutex db_mtx;
public:
    Storage() 
    {
        if (sqlite3_open("todo.db", &db) != SQLITE_OK) 
        {
            cerr << "CRITICAL: Database connection failed!" << endl;
            throw runtime_error("DB connection error");
        }
        const char* sql = "CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, description TEXT, status TEXT);";
        if (sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK) 
        {
            cerr << "CRITICAL: Table creation failed!" << endl;
        }
    }
    ~Storage() 
    { 
        sqlite3_close(db); 
    }
    bool exists(int id) 
    {
        lock_guard<mutex> lock(db_mtx);
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT 1 FROM tasks WHERE id = ?;", -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, id);
        bool found = (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
        return found;
    }
    void clearCache(int id) 
    { 
        cache.erase(id); 
    }
    int add(string t, string d, string s) 
    {
        lock_guard<mutex> lock(db_mtx);
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO tasks (title, description, status) VALUES (?, ?, ?);", -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, t.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, d.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, s.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE) 
        {
            sqlite3_finalize(stmt);
            return -1;
        }
        int id = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
        return id;
    }
    void removeTask(int id) 
    {
        lock_guard<mutex> lock(db_mtx);
        clearCache(id);
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "DELETE FROM tasks WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    json getAll() 
    {
        lock_guard<mutex> lock(db_mtx);
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
        clearCache(id);
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "UPDATE tasks SET status = ? WHERE id = ?;", -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    void updateFull(int id, string title, string desc, string status) 
    {
        clearCache(id);
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
        if (cache.count(id)) 
        {
            cout << "[CACHE] Hit for ID: " << id << endl;
            return cache[id];
        }
        lock_guard<mutex> lock(db_mtx);
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
            cache[id] = res;
        }
        sqlite3_finalize(stmt);
        return res;
    }
};
struct Middleware : crow::ILocalMiddleware 
{
    struct context 
    {
        chrono::steady_clock::time_point start_time;
    };
    chrono::steady_clock::time_point last_request_time;
    void before_handle(crow::request& req, crow::response& res, context& ctx) 
    {
        ctx.start_time = chrono::steady_clock::now();
        auto now = chrono::steady_clock::now();
        auto diff = chrono::duration_cast<chrono::milliseconds>(now - last_request_time).count();
        if (diff < 50) 
        {
            res.code = 429;
            res.body = "{\"error\": \"Too Many Requests\"}";
            res.end();
            return;
        }
        last_request_time = now;
        cout << "[METRIC] Incoming: " << crow::method_name(req.method) << " " << req.url << endl;
        auto api_key = req.get_header_value("X-API-Key");
        if (api_key != "secret123") 
        {
            res.code = 401;
            res.body = "{\"error\": \"Unauthorized: Invalid API Key\"}";
            res.end();
        }
    }
    void after_handle(crow::request& req, crow::response& res, context& ctx) 
    {
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - ctx.start_time).count();
        cout << "[METRIC] Status: " << res.code
            << " | Time: " << duration << "ms"
            << " | Method: " << crow::method_name(req.method) << endl;
    }
};
void background_worker() 
{
    while (true) 
    {
        string task_title;
        {
            unique_lock<mutex> lock(queue_mtx);
            queue_cv.wait(lock, [] { return !message_queue.empty() || stop_worker; });
            if (stop_worker && message_queue.empty()) 
                break;
            task_title = message_queue.front();
            message_queue.pop();
        }
        try 
        {
            this_thread::sleep_for(chrono::seconds(3));
            cout << "[WORKER] Background processing finished for task: " << task_title << endl;
        }
        catch (...)
        {
            cerr << "[WORKER] Error during background task!" << endl;
        }
    }
}
int main() 
{
    crow::App<Middleware> app;
    Storage db;
    thread worker_thread(background_worker);
    CROW_ROUTE(app, "/")([]() 
        {
        return "To-Do API is online!";
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
            string title = body.value("title", "Untitled");
            int id = db.add(
                title,
                body.value("description", ""),
                body.value("status", "todo")
            );
            {
                lock_guard<mutex> lock(queue_mtx);
                message_queue.push(title);
            }
            queue_cv.notify_one();
            json res = body;
            res["id"] = id;
            res["_notice"] = "Task created and queued for processing";
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
    stop_worker = true; 
    queue_cv.notify_all();
    if (worker_thread.joinable())
    {
        worker_thread.join();
    }
    return 0;
}