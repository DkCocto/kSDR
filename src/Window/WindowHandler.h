#pragma once
#include <cmath>
#include <iostream>
#include <vector>
#include "../Env.h"

class WindowHandler {
private:
    int len;
    std::vector<float> weightArray;
    std::string currentWindowType;
    float currentParam;

public:
    // Установка длины окна (половина длины FFT)
    void setLen(int fftLen) {
        len = fftLen / 2;
    }

    // Переинициализация окна, вызовется при необходимости
    void reinitializeWindow(const std::string& windowType, float param = 5.0f) {
        currentWindowType = windowType;
        currentParam = param;
        initWindow();
    }

    // Метод для получения весов окна
    const std::vector<float>& getWeights() {
        return weightArray;
    }

private:
    // Инициализация окна в зависимости от типа
    void initWindow() {
        weightArray.clear();

        if (currentWindowType == "Blackman") {
            initBlackman();
        }
        else if (currentWindowType == "Hamming") {
            initHamming();
        }
        else if (currentWindowType == "Hann") {
            initHann();
        }
        else if (currentWindowType == "Rectangular") {
            initRectangular();
        }
        else if (currentWindowType == "Bartlett") {
            initBartlett();
        }
        else if (currentWindowType == "Kaiser") {
            initKaiser(currentParam); // Параметр beta для окна Кайзера
        }
        else if (currentWindowType == "Gaussian") {
            initGaussian(currentParam); // Параметр alpha для окна Гаусса
        }
        else if (currentWindowType == "FlatTop") {
            initFlatTop();
        }
        else {
            std::cerr << "Unknown window type: " << currentWindowType << std::endl;
        }
    }

    // Окно Блэкмана (Blackman)
    void initBlackman() {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = (0.42 - 0.5 * cosf(2.0 * M_PI * i / len) + 0.08 * cosf(4.0 * M_PI * i / len));
            if (weightArray[i] < 0.0) weightArray[i] = 0.0;
        }
    }

    // Окно Хэмминга (Hamming)
    void initHamming() {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = 0.54 - 0.46 * cosf(2.0 * M_PI * i / len);
        }
    }

    // Окно Ханна (Hann)
    void initHann() {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = 0.5 * (1.0 - cosf(2.0 * M_PI * i / len));
        }
    }

    // Прямоугольное окно (Rectangular)
    void initRectangular() {
        weightArray.resize(len, 1.0f);
    }

    // Окно Бартлетта (Bartlett)
    void initBartlett() {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = 1.0f - std::fabs(2.0f * i / (len - 1) - 1.0f);
        }
    }

    // Окно Кайзера (Kaiser)
    void initKaiser(float beta) {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            float alpha = (2.0f * i / (len - 1)) - 1.0f;
            weightArray[i] = i == 0 || i == len - 1 ? 0.0f :
                std::cyl_bessel_i(0, beta * std::sqrt(1 - alpha * alpha)) / std::cyl_bessel_i(0, beta);
        }
    }

    // Окно Гаусса (Gaussian)
    void initGaussian(float alpha) {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = exp(-0.5f * std::pow((i - len / 2) / (alpha * len / 2), 2));
        }
    }

    // Окно Flat-top
    void initFlatTop() {
        weightArray.resize(len);
        for (int i = 0; i < len; i++) {
            weightArray[i] = 0.21557895 - 0.41663158 * cosf(2.0 * M_PI * i / (len - 1)) + 0.277263158 * cosf(4.0 * M_PI * i / (len - 1))
                - 0.083578947 * cosf(6.0 * M_PI * i / (len - 1)) + 0.006947368 * cosf(8.0 * M_PI * i / (len - 1));
        }
    }
};

