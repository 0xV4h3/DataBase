#pragma once

#include "Executor.h"
#include "Planner.h"
#include <memory>

class Processor {
private:
	std::unique_ptr<Executor> executor;
	std::unique_ptr<Planner> planner;
};