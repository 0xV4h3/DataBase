#pragma once

#include "SessionManager.hpp"

class RelationalEngine {
private:
	std::unique_ptr<SessionManager> manager;
public:
	explicit RelationalEngine();
};