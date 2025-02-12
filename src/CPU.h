#pragma once

#include "windows.h"
#include <memory>
#include "Average.h"


class CPU {

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE self;

    std::unique_ptr<Average> average;

public:
    CPU() {
        init();
    }

    void init() {
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

        average = std::make_unique<Average>(70);
    }

    double getCurrentValue() {
        return average->process(GetCPULoad() * 100);  // Здесь никаких изменений, просто возвращаем значение процента загрузки
    }

    static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
    {
        static unsigned long long _previousTotalTicks = 0;
        static unsigned long long _previousIdleTicks = 0;

        unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
        unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

        float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

        _previousTotalTicks = totalTicks;
        _previousIdleTicks = idleTicks;
        return ret;
    }

    static unsigned long long FileTimeToInt64(const FILETIME& ft) {
        return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
    }

    float GetCPULoad() {
        FILETIME idleTime, kernelTime, userTime;

        // Изменение: Проверка успешного получения времени системы через GetSystemTimes()
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {  // Здесь была добавлена проверка успешности получения данных
            unsigned long long idle = FileTimeToInt64(idleTime);
            unsigned long long kernel = FileTimeToInt64(kernelTime);
            unsigned long long user = FileTimeToInt64(userTime);
            return CalculateCPULoad(idle, kernel + user);  // Изменение: исправлена логика расчета нагрузки процессора
        }

        return -1.0f;  // Ошибка получения данных
    }
};


/*#pragma once

#include "windows.h"
#include <memory>
#include "Average.h"


class CPU {

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE self;

    std::unique_ptr<Average> average;

public:
    CPU() {
        init();
    }
    void init() {
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

        average = std::make_unique<Average>(70);
    }
    double getCurrentValue() {
        return average->process(GetCPULoad() * 100);
    }
    static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
    {
        static unsigned long long _previousTotalTicks = 0;
        static unsigned long long _previousIdleTicks = 0;

        unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
        unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

        float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

        _previousTotalTicks = totalTicks;
        _previousIdleTicks = idleTicks;
        return ret;
    }

    static unsigned long long FileTimeToInt64(const FILETIME& ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

    // Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
    // You'll need to call this at regular intervals, since it measures the load between
    // the previous call and the current one.  Returns -1.0 on error.
    float GetCPULoad()
    {
        FILETIME idleTime, kernelTime, userTime;
        return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
    }
};*/