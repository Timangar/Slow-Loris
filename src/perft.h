#pragma once
#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <thread>

int perf(chess_environment& env, int depth);
void perft();