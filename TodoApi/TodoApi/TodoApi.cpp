#define CROW_MAIN
#define ASIO_STANDALONE
#include <vector>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp> 
#include "crow_all.h"
using namespace std;
using json = nlohmann::json;
struct Task 
{
    int id;
    string title;
    string description;
    string status;
    json to_json() const 
    {
        return json{ {"id", id}, {"title", title}, {"description", description}, {"status", status} };
    }
};
vector<Task> tasks;
mutex tasks_mutex;
int next_id = 1;
int main() 
{
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([]() 
        {
        return "To-Do API is running!";
        });
    CROW_ROUTE(app, "/tasks").methods("GET"_method)([]() 
        {
        json res = json::array();
        lock_guard<mutex> lock(tasks_mutex);
        for (const auto& t : tasks) {
            res.push_back({ {"id", t.id}, {"title", t.title}, {"status", t.status} });
        }
        return crow::response(res.dump());
        });
    CROW_ROUTE(app, "/tasks").methods("POST"_method)([](const crow::request& req) 
        {
        try 
        {
            auto body = json::parse(req.body);
            Task t;
            {
                lock_guard<std::mutex> lock(tasks_mutex);
                t.id = next_id++;
                t.title = body.value("title", "Untitled");
                t.description = body.value("description", "");
                t.status = body.value("status", "todo");
                tasks.push_back(t);
            }
            return crow::response(201, t.to_json().dump());
        }
        catch (...) 
        {
            return crow::response(400, "Invalid JSON");
        }
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
    app.port(18080).multithreaded().run();
    return 0;
}