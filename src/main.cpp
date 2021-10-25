#pragma once
#include "perft.h"
#include "play.h"
#include "valnet.h"


int main(void)
{
	train(100000, 1, 0.5f, 0.99f);
	//play();
}