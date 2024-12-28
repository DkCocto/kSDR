#pragma once

#include "Env.h"
#include <chrono>
#include "Config.h"
#include "KalmanFilter.h"


class Transform {
public:
    // ����������� ����� ��� ���������� ��������� ��������������
    static float make(float value, float srcX1, float srcX2, float dstY1, float dstY2) {
        return (value - srcX1) / (srcX2 - srcX1) * (dstY2 - dstY1) + dstY1;
    }
};

class ALC {

    KalmanFilter* power = new KalmanFilter(1, 1.0);

    float currentOutputPower = 0;

    float inputSignalLevel = 50;     // ��������� �������� ��������

    //float inputLevel = 50;

    float smoothingFactorFast = 0.4f;
    float smoothingFactorSlow = 0.05f;
    //float aggressiveFactor = 2.0f; // ����������� ��� ����������� �������������
    //float maxDeviationFactor = 1.2f; // ���������� ���������� (1.2 �������� 20% ������ ������� ��������)

    // ���������� ����������� ��� �������� "��������" � ������� ��������
    float tolerance = 0.3f; // �������� ��������� �������, ���� ���������� �� ������� �� ����� 0.5

    // ���������� ��� �������� �������
    std::chrono::time_point<std::chrono::steady_clock> lastAdjustmentTime;
    bool reachedTargetPower = false;
    //bool canIncreasePower = false;
    int timeToAllowIncrease = 2000; // ����� � ������������� �� ���������� ���������� �������� (��������, 5 ������)

    Config* config;

public:
    ALC(Config* config) {
        lastAdjustmentTime = std::chrono::steady_clock::now();
        this->config = config;
    }

    std::pair<float, float> generatePair() {
        std::pair<float, float> pair;
        pair.first = inputSignalLevel;
        pair.second = Transform::make(inputSignalLevel, 0, 120, 0, 20);
        return pair;
    }

    std::pair<float, float> process(float currentOutputPower) {
        float targetPower = (float)config->transmit.outputPower;
        this->currentOutputPower = currentOutputPower;
        auto currentTime = std::chrono::steady_clock::now();

        if (reachedTargetPower) {
            std::chrono::duration<float> elapsedTime = currentTime - lastAdjustmentTime;
            if (elapsedTime.count() < timeToAllowIncrease / 1000.0f) {
                if (currentOutputPower > targetPower) {
                    fastAjust(targetPower);
                    return generatePair();
                } else {
                    return generatePair();
                }
            } else {
                reachedTargetPower = false;
            }
        }
       

        if (currentOutputPower > targetPower) {
            fastAjust(targetPower);
        }
        else if (currentOutputPower < targetPower) {
            slowAjust(targetPower);
        }

        reachedTargetPower = isPowerFine(targetPower);

        if (reachedTargetPower) lastAdjustmentTime = currentTime;

        return generatePair();
    }

    void fastAjust(float targetPower) {
        float powerError = targetPower - currentOutputPower;
        float adjustment = smoothingFactorFast * powerError;
        inputSignalLevel += adjustment;
        // ������������ �������� � ��������� �� 1 �� 20
        if (inputSignalLevel > 120) inputSignalLevel = 120;
        if (inputSignalLevel < 0) inputSignalLevel = 0;
    }
    void slowAjust(float targetPower) {
        float powerError = targetPower - currentOutputPower;
        float adjustment = smoothingFactorSlow * powerError;
        inputSignalLevel += adjustment;
        // ������������ �������� � ��������� �� 1 �� 20
        if (inputSignalLevel > 120) inputSignalLevel = 120;
        if (inputSignalLevel < 0) inputSignalLevel = 0;
    }
    bool isPowerFine(float targetPower) {
        return std::fabs(currentOutputPower - targetPower) <= tolerance;
    }
};