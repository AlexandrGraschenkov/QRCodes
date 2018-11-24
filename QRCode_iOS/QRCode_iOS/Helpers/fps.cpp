//
//  fps.cpp
//  TerminalBlockDetector
//
//  Created by Alexander Graschenkov on 17.05.2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "fps.hpp"

using namespace std;

FPS::FPS() {}

void FPS::tick() {
    m_time.push_back(std::chrono::high_resolution_clock::now());
}

double FPS::getFPS() {
    if (m_time.size() < 2) {
        return 0;
    }
    while (m_time.size() > 50) {
        m_time.pop_front();
    }
    
    auto elapsed = m_time.back() - m_time.front();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    ms = max((long long)1, ms);
    // max(ms, 1) - Не красивое решение, но позволяет избежать деления на 0
    double m_fps = ((double)m_time.size()) / (ms / 1000.0);
    return m_fps;
}






FPSIteration::FPSIteration () {}

void FPSIteration::start() {
    started = true;
    start_time = std::chrono::high_resolution_clock::now();
}

void FPSIteration::end() {
    if (!started) { return; }
    
    started = false;
    time_type end_time = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    last_times.push_back(ms / 1000.0);
    if (last_times.size() > 50) {
        last_times.erase(last_times.begin());
    }
}

double FPSIteration::getFPS() {
    double sum_times = 0;
    for (double t : last_times) {
        sum_times += t;
    }
    if (sum_times == 0) {
        return 0;
    }
    return (double)last_times.size() / sum_times;
}
