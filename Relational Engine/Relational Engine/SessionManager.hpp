#pragma once

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Analyzer.hpp"
#include "Optimizer.hpp"
#include "Processor.hpp"
#include <atomic>
#include <cstdint>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

class Session {
private:
	uint64_t id;
public:
	explicit Session(const uint64_t& SessionID);
	explicit Session(const uint64_t SessionID, const std::string& query);
	void setQuery(const std::string& query);

	std::unique_ptr<Lexer> lexer;
	std::unique_ptr<Parser> parser;
	std::unique_ptr<Analyzer> analizer;
	std::unique_ptr<Optimizer> optimizer;
	std::unique_ptr<Processor> processor;
};

class SessionManager {
private:
	static std::atomic<uint64_t> sessionCounter;
	std::unordered_map<uint64_t, std::unique_ptr<Session>> sessions;
	mutable std::shared_mutex sessionsMutex;
public:
	explicit SessionManager() {};
	uint64_t createSession();
	Session* getSession(const uint64_t SessionID);
	static uint64_t generateSessionID();
};

