#pragma once
#include "perft.h"
#include "play.h"
#include "valnet.h"


int main(void)
{
	train(100000, 100, 0.7f, 0.999f);
}