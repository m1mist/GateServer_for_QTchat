#pragma once

#include "global.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem :public Singleton<LogicSystem> {
public:
    friend class Singleton<LogicSystem>;
    ~LogicSystem();
    bool HandleGet(const std::string&, std::shared_ptr<HttpConnection>);
    bool HandlePost(const std::string&, std::shared_ptr<HttpConnection>);
    void RegisterGet(const std::string&, const HttpHandler& handler);
    void RegisterPost(const std::string&, const HttpHandler& handler);
private:
    LogicSystem();
    std::map<std::string, HttpHandler> post_handlers_;
    std::map<std::string, HttpHandler> get_handlers_;
};

