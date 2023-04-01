#ifndef CALCULAITON_HPP
#define CALCULAITON_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <immintrin.h>

const size_t WINDOW_SIZE_X = 1200;
const size_t WINDOW_SIZE_Y = 1200;

float BORDER = 3;

const size_t MAX_ITERATION = 255;
const float MAX_DISTANCE = 100.f;

float dx = (2*BORDER)/WINDOW_SIZE_X;
float dy = (2*BORDER)/WINDOW_SIZE_Y;

#endif