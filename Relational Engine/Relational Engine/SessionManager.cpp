#include "SessionManager.hpp"

Session::Session(const uint64_t& SessionID) {
    id = SessionID;
    //parser = std::make_unique<Parser>(Parser());
    //analizer = std::make_unique<Analyzer>(Analyzer());
    //optimizer = std::make_unique<Optimizer>(Optimizer());
    //processor = std::make_unique<Processor>(Processor());
}

Session::Session(const uint64_t SessionID, const std::string& query) : Session(SessionID) {
    setQuery(query);
}

void Session::setQuery(const std::string& query) {
    if (lexer) 
        lexer.reset(new Lexer(query));
    else
    lexer = std::make_unique<Lexer>(Lexer(query));
}

std::atomic<uint64_t> SessionManager::sessionCounter{ 0 };

uint64_t SessionManager::generateSessionID() {
    return ++sessionCounter;  
}

uint64_t SessionManager::createSession() {
    uint64_t SessionID = generateSessionID();
    {
        std::unique_lock<std::shared_mutex> lock(sessionsMutex);
        sessions.emplace(SessionID, std::make_unique<Session>(SessionID));
    }
    return SessionID;
}

Session* SessionManager::getSession(const uint64_t SessionID) {
    std::shared_lock<std::shared_mutex> lock(sessionsMutex);
    auto it = sessions.find(SessionID);
    return (it != sessions.end()) ? it->second.get() : nullptr;
}