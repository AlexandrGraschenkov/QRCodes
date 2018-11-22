//
//  ReedSolomonEncoder.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 23/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef ReedSolomonEncoder_hpp
#define ReedSolomonEncoder_hpp

#include <memory>
#include <vector>
#include "Counted.h"
#include "Array.h"
#include "GenericGFPoly.h"
#include "GenericGF.h"

namespace RS {

class GenericGFPoly;
class GenericGF;

class ReedSolomonEncoder {
private:
    Ref<GenericGF> field;
    std::vector<Ref<GenericGFPoly>> cachedGenerators;
public:
    ReedSolomonEncoder(Ref<GenericGF> fld);
    ~ReedSolomonEncoder();
    
    void encode(ArrayRef<int> received, int ecBytes);
    
private:
    Ref<GenericGFPoly> buildGenerator(int degree);
};

}

#endif /* ReedSolomonEncoder_hpp */
