#include "RelationalEngine.hpp"

RelationalEngine::RelationalEngine() {
	manager = std::make_unique<SessionManager>(SessionManager());
}