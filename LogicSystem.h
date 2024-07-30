#pragma once

#include "global.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem :public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
    void RegisterGet(std::string, HttpHandler handler);
private:
    LogicSystem();
    std::map<std::string, HttpHandler> post_handlers_;
    std::map<std::string, HttpHandler> get_handlers_;
};

