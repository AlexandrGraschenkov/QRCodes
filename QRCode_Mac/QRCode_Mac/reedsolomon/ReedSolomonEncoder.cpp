//
//  ReedSolomonEncoder.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 23/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "ReedSolomonEncoder.h"
#include "ReedSolomonException.h"

#include <iostream>
#include <memory>

using namespace std;
using namespace RS;

ReedSolomonEncoder::~ReedSolomonEncoder() {
}

ReedSolomonEncoder::ReedSolomonEncoder(Ref<GenericGF> fld) {
    this->field = fld;
    
    Ref<GenericGFPoly> elem = Ref<GenericGFPoly>(new GenericGFPoly(fld, ArrayRef<int>(new Array<int>(1))));
    elem->getCoefficients()[0] = 1;
    cachedGenerators.push_back(elem);
}

Ref<GenericGFPoly> ReedSolomonEncoder::buildGenerator(int degree) {
    if (degree >= cachedGenerators.size()) {
        Ref<GenericGFPoly> lastGenerator = cachedGenerators[cachedGenerators.size() - 1];
        for (int d = (int)cachedGenerators.size(); d <= degree; d++) {
            ArrayRef<int> coef = ArrayRef<int>(new Array<int>(2));
            coef[0] = 1;
            coef[1] = field->exp(d - 1 + field->getGeneratorBase());
            Ref<GenericGFPoly> other = Ref<GenericGFPoly>(new GenericGFPoly(field, coef));
            Ref<GenericGFPoly> nextGenerator = lastGenerator->multiply(other);
            cachedGenerators.push_back(nextGenerator);
            lastGenerator = nextGenerator;
        }
    }
    return cachedGenerators[degree];
}

void ReedSolomonEncoder::encode(ArrayRef<int> toEncode, int ecBytes) {
    if (ecBytes <= 0) {
        throw ReedSolomonException("No error correction bytes");
    }
    int dataBytes = toEncode->size() - ecBytes;
    if (dataBytes <= 0) {
        throw ReedSolomonException("No data bytes provided");
    }
    
    Ref<GenericGFPoly> generator = buildGenerator(ecBytes);
    ArrayRef<int> infoCoefficients = ArrayRef<int>(new Array<int>(dataBytes));
    for (int i = 0; i < dataBytes; i++) {
        infoCoefficients[i] = toEncode[i];
    }
    Ref<GenericGFPoly> info = Ref<GenericGFPoly>(new GenericGFPoly(field, infoCoefficients));
    info = info->multiplyByMonomial(ecBytes, 1);
    Ref<GenericGFPoly> reminder = info->divide(generator)[1];
    auto coefficients = reminder->getCoefficients();
    int coeficentsSize = coefficients->size();
    int numZeroCoefficients = ecBytes - coeficentsSize;
    for (int i = 0; i < numZeroCoefficients; i++) {
        toEncode[dataBytes + i] = 0;
    }
    for (int i = 0; i < coeficentsSize; i++) {
        toEncode[dataBytes + numZeroCoefficients + i] = coefficients[i];
    }
}

