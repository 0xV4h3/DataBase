#pragma once

#include "SessionManager.h"

class RelationalEngine {
private:
	std::unique_ptr<SessionManager> manager;
public:
	explicit RelationalEngine();
};