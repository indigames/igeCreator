///////////////////////////////////////////////////////////////
//Pyxie game engine
//
//  Copyright Kiharu Shishikura 2019. All rights reserved.
///////////////////////////////////////////////////////////////
#pragma once
#include "Python.h"
namespace pyxie
{
	uint8_t* createTextImage(const char* word, const char* fontpath, int fontsize, int& texw, int& texh, float pitch);
	bool calcTextSize(const char* word, const char* fontpath, int fontsize, int& outW, int& outH, float pitch);
	uint8_t* createCheckeredTexture(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, int texWidth, int texHeight, int format);
	uint8_t* createColorTexture(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, int texWidth, int texHeight, int format);
	void FlipRGBY(uint8_t* data, int width, int height);
	void FlipRGBAY(uint8_t* data, int width, int height);
	void FlipY(uint8_t* inimg, uint8_t* outimg, int w, int h);
	void savePYXI(uint8_t* pixels, int w, int h, int format, const char* path);

}