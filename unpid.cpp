#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;

#include "unpid.h"

bool isInRange(float value, float min, float max) {
    return value >= min && value <= max;
}

int randRange(int min, int max) {
    return floor(rand() % (max - min + 1) + min);
}

int randomBelievableIncrement() {
    return randRange(1, 10);
}

bool isEstimatedEqual(int value, int estimate) {
    return value == estimate || value == estimate + 1 || value == estimate - 1;
}