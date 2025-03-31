#include "RelationalEngine.h"

RelationalEngine::RelationalEngine() {
	manager = std::make_unique<SessionManager>(SessionManager());
}