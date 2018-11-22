//
//  ReedSolomon.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 23/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef ReedSolomon_hpp
#define ReedSolomon_hpp

#include <stdio.h>
#include <vector>


class ReedSolomon {
    int elementSize;
public:
    ReedSolomon(int elementSize);
    bool encode(std::vector<int> &data, int errorCodeSize);
    bool decode(std::vector<int> &data, int errorCodeSize);
};

#endif /* ReedSolomon_hpp */
