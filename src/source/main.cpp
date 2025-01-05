// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "../headers/Application/Application.h"
#include "../headers/Engine Core/Entity/ECS.h"

int main() {
    srand((uint32_t)time(0));

    Application app{};
    app.start();

    return 0;
}