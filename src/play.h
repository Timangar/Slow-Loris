#pragma once
#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <thread>

void render(chess_environment& env);
void input(chess_environment& env, agent& agent);
void play();