//
//  ReedSolomon.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 23/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "ReedSolomon.h"
#include "ReedSolomonEncoder.h"
#include "ReedSolomonDecoder.h"
#include "ReedSolomonException.h"

using namespace std;
using namespace RS;

Ref<GenericGF> privateGetGenericGF(int elementSize) {
    if (elementSize <= 16) {
        return GenericGF::AZTEC_PARAM;
    } else if (elementSize <= 64) {
        return GenericGF::AZTEC_DATA_6;
    } else if (elementSize <= 256) {
        return GenericGF::AZTEC_DATA_8;
    } else if (elementSize <= 1024) {
        return GenericGF::AZTEC_DATA_10;
    } else if (elementSize <= 4096) {
        return GenericGF::AZTEC_DATA_12;
    }
    return Ref<GenericGF>();
}

ReedSolomon::ReedSolomon(int elementSize) {
    this->elementSize = elementSize;
}

bool ReedSolomon::encode(std::vector<int> &data, int errorCodeSize) {
    auto field = privateGetGenericGF(elementSize);
    if (field.empty()) {
        return false;
    }
    
    int totalCount = (int)data.size() + errorCodeSize;
    ArrayRef<int> arr = ArrayRef<int>(new Array<int>(totalCount));
    for (int i = 0; i < data.size(); i++) {
        arr[i] = data[i];
    }
    
    ReedSolomonEncoder encoder(field);
    try {
        encoder.encode(arr, errorCodeSize);
    } catch (ReedSolomonException) {
        return false;
    }
    data.resize(totalCount);
    for (int i = totalCount - errorCodeSize; i < totalCount; i++) {
        data[i] = arr[i];
    }
    return true;
}

bool ReedSolomon::decode(std::vector<int> &data, int errorCodeSize) {
    auto field = privateGetGenericGF(elementSize);
    if (field.empty()) {
        return false;
    }
    
    int totalCount = (int)data.size() + errorCodeSize;
    ArrayRef<int> arr = ArrayRef<int>(new Array<int>(totalCount));
    for (int i = 0; i < data.size(); i++) {
        arr[i] = data[i];
    }
    
    ReedSolomonDecoder decoder(field);
    try {
        decoder.decode(arr, errorCodeSize);
    } catch (ReedSolomonException) {
        return false;
    }
    data.resize(data.size() - errorCodeSize);
    for (int i = 0; i < data.size(); i++) {
        data[i] = arr[i];
    }
    return true;
}

