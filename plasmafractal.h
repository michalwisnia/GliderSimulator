//Original code by GitHub user Vesther https://github.com/Vesther/Cloud-Fractal

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>

#include <Windows.h>
#include "model.h"


class PlasmaFractal {
private:
    int size; // This is required to be 2^n + 1
    int range; // Degree of randomness / roughness. Larger values generate rougher images.
    int map[513][513];// The 2D array of integer values into which our fractal will be written
    // to nie jest eleganckie rozwiązanie, ale nie zamierzam się bić z c++
public:
    float verts[3 * 512 * 512 * 4 * 2]; // 3 * (size-1)^2 * 4 * 2
    float colors[3 * 512 * 512 * 4 * 2];
    float normals[3 * 512 * 512 * 4 * 2];
    int vert_count;
    PlasmaFractal();
    int rnd(int min, int max);
    void diamond(int sideLength);
    void average(int x, int y, int sideLength);
    void square(int sideLength);
    void fractal();
    void clamp(int* val, int min, int max);
    void clamp_map();
    void print_map();
    void save_to_file();
    void transform_to_opengl();
    void draw_solid();
};