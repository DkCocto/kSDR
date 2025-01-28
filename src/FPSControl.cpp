#include <chrono>
#include <thread>

class FPSControl {
private:
    float targetFPS;
    bool holdFps;

    std::chrono::steady_clock::time_point startTime;
    int frames;
    int result;

    std::chrono::steady_clock::time_point startTimeFpsControl;
    std::chrono::steady_clock::time_point endTimeFpsControl;
    long deltaFpsControl;

    long targetMs;

public:
    FPSControl(float targetFPS = 60.0f, bool holdFps = true)
        : targetFPS(targetFPS), holdFps(holdFps), frames(0), result(0) {
        startTime = std::chrono::steady_clock::now();
        startTimeFpsControl = startTime;
        targetMs = 1000 / targetFPS;
    }

    void setTargetFPS(float targetFPS) {
        this->targetFPS = targetFPS;
        targetMs = (targetFPS > 0) ? 1000 / targetFPS : 1;
    }

    void tick() {
        frames++;
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count() >= 1) {
            startTime = now;
            result = frames;
            frames = 0;
        }

        if (holdFps) {
            endTimeFpsControl = std::chrono::steady_clock::now();
            deltaFpsControl = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeFpsControl - startTimeFpsControl).count();
            if (deltaFpsControl < targetMs) {
                std::this_thread::sleep_for(std::chrono::milliseconds(targetMs - deltaFpsControl));
            }
            startTimeFpsControl = std::chrono::steady_clock::now();
        }
    }

    int getResult() const {
        return result;
    }
};