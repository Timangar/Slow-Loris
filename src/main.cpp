#pragma once
#include "perft.h"
#include "play.h"
#include "valnet.h"
#include "polnet.h"


int main(void)
{
	train(6000, 100, 0.2f, 1.0f);
	//play();
}