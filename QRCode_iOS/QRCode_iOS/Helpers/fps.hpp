//
//  fps.hpp
//  TerminalBlockDetector
//
//  Created by Alexander Graschenkov on 17.05.2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef fps_hpp
#define fps_hpp

#include <stdio.h>
#include <chrono>
#include <list>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_type;

class FPS {
    
public:
    FPS();
    
    void tick();
    double getFPS();
    
private:
    std::list<time_type> m_time;
};




// считает FPS толко между строчками start() и end()
class FPSIteration {
    
public:
    FPSIteration();
    
    void start();
    void end();
    double getFPS();
    
private:
    std::vector<double> last_times;
    time_type start_time;
    bool started;
};

#endif /* fps_hpp */
