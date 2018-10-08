#pragma once

#include <stdio.h>
#include "BitmapFont.h"
#include "StringUtils.h"
#include <string>
#include <vector>

using namespace std;
//---------------------------------------------------------------------------------------------------
unsigned int GetRandomNum(int min, int max)
{
	unsigned int diff = ((max - min) + 1);
	return ((diff * rand()) / RAND_MAX) + min;
}
//---------------------------------------------------------------------------------------------------
float GetRandomFloat(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}
//---------------------------------------------------------------------------------------------------
RGBA GetPixelsAvgColor(PIXMAP *bmp, int x, int y, int w, int h)
{

	RGBA *runner;
	int r = 0, g = 0, b = 0, a = 0; //use ints so we dont have to worry about overflow when adding
	int counter = 0;
	for (unsigned int uiV = y; uiV < (y + h); ++uiV)
	{
		// reset coordinate for each row
		runner = &bmp->pixels[uiV * bmp->w + x];

		// read each row
		for (unsigned int uiH = x; uiH < (x + w); ++uiH)
		{
			r += runner->r;
			g += runner->g;
			b += runner->b;
			a += runner->a;
			runner++;
			counter++;
		}
	}
	r += (counter / 2);
	g += (counter / 2);
	b += (counter / 2);
	a += (counter / 2);

	r /= counter;
	g /= counter;
	b /= counter;
	a /= counter;

	return RGBA{ (unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a };
}

string GetArgValue(std::vector<std::string>args, std::string key)
{
	for (size_t i = 0; i < args.size(); i++)
	{
		vector<string> pair = StringUtils::Tokenize(args[i], "=");

		if (pair.size() < 2)
			continue;
		else if (pair[0] == key)
			return pair[1];
	}

	return "";
}
