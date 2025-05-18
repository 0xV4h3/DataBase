#pragma once

#include "Executor.hpp"
#include "Planner.hpp"
#include <memory>

class Processor {
private:
	std::unique_ptr<Executor> executor;
	std::unique_ptr<Planner> planner;
};