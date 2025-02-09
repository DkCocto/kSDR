#pragma once
#include <vector>

class Delay {
    int len = 0;
    std::vector<float> buffer;
    int index = 0;  // Указатель на текущую позицию

public:
    Delay(int len) : len(len), buffer(len + 1, 0.0f) {}

    float filter(float value) {
        index = (index + 1) % (len + 1); // Циклический сдвиг
        buffer[index] = value;
        return buffer[(index + 1) % (len + 1)]; // Возвращаем задержанный элемент
    }
};

/*#pragma once
class Delay {

    int len = 0;
    float* xv = nullptr;

public:
    Delay(int len) {
        this->len = len;
        xv = new float[len + 1];
    }

    ~Delay() {
        delete[] xv;
    }

    float filter(float value) {
        for (int i = 0; i < len; i++) xv[i] = xv[i + 1];
        xv[len] = value;
        return xv[0];
    }
};*/