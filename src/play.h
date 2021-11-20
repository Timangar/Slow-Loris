#pragma once
#include "chess_environment.h"
#include "agent.h"
#include "tools.h"
#include <thread>

void input(chess_environment* env, agent& agent);
void play();
void train(int epochs, int render_intervall, float esilon = 0.5f, float decay = 0.999f);
void arena(std::string engine1, std::string engine2, int epochs, int render_interval);