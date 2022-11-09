#pragma once

class Average {
private:
    double avg = 1.0;
    double len = 20.0;

public:
     
    Average(double len) {
        this->len = len;
    }

    double process(double value) {
        if (isinf(value)) return 1.0;
        avg -= avg / len;
        avg += value / len;
        return avg;
    }

    double getAvg() {
        return avg;
    }
};