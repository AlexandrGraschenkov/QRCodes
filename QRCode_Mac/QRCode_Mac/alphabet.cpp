//
//  alphabet.cpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 22/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#include "alphabet.hpp"
#include <vector>
#include <iostream>

#include "reedsolomon/ReedSolomon.h"

using namespace std;

Alphabet::Alphabet(int aMessageSize, double correctPercentSize, std::string aFillEmptySpaces, std::string aSymbols) {
    if (aSymbols == "") {
        aSymbols = " _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    }
    if (aFillEmptySpaces == "") {
        aFillEmptySpaces = " ";
    }
    
    // в зависимости от длинны словаря считаем сколько нам бит потребуется для записи символа в двоичной системе
    symbols = aSymbols;
    bitSize = 0;
    fieldSize = 1;
    while (fieldSize < aSymbols.size()) {
        fieldSize *= 2;
        bitSize += 1;
    }
    emptySymbol = aFillEmptySpaces;
    messageSize = aMessageSize;
    correctSize = aMessageSize * correctPercentSize;
}

int Alphabet::totalMessageSize() const {
    return messageSize + correctSize;
}

int Alphabet::totalMessageSizeBits() const {
    return (messageSize + correctSize) * bitSize;
}

std::string Alphabet::getError() {
    return errorMessage;
}

std::string Alphabet::decode(std::vector<bool> bits) {
    vector<int> data = bits2data(bits);
    
    ReedSolomon coder(fieldSize);
    if (!coder.decode(data, correctSize)) {
        errorMessage = "Error on decode data";
        return "";
    }
    string str = data2str(data);
    return str;
}

std::vector<bool> Alphabet::encode(std::string str) {
    vector<int> data = str2data(str);
    
    ReedSolomon coder(fieldSize);
    if (!coder.encode(data, correctSize)) {
        errorMessage = "Error on encode data";
        return vector<bool>();
    }
    vector<bool> bits = data2bits(data);
    return bits;
}

std::vector<int> Alphabet::str2data(std::string message) const {
    while (message.size() < messageSize) {
        message += emptySymbol;
    }
    while (message.size() > messageSize) {
        message.erase(message.end());
    }
    vector<int> data;
    for (int i = 0; i < message.size(); i++) {
        int symbolIdx = (int)symbols.find(message.substr(i, 1));
        if (symbolIdx == std::string::npos) {
            symbolIdx = 0;
        }
        data.push_back(symbolIdx);
    }
    return data;
}

std::string Alphabet::data2str(const std::vector<int> &messageData) const {
    string result = "";
    for (int idx : messageData) {
        result += symbols[idx];
    }
    
    
    while (result.size() && result.substr(result.size()-1, 1) == emptySymbol) {
        result.pop_back();
    }
    return result;
}

std::vector<bool> Alphabet::data2bits(std::vector<int> data) const {
    vector<bool> bits;
    bits.resize(data.size() * bitSize);
    int offset = 0;
    for (int val : data) {
        int idx = bitSize-1;
        while(idx >= 0) {
            bits[idx + offset] = (val&1);
            val>>=1;
            idx--;
        }
        offset += bitSize;
    }
    return bits;
}

std::vector<int> Alphabet::bits2data(std::vector<bool> bits) const {
    vector<int> data;
    data.resize(bits.size() / bitSize);
    for (int i = 0; i < data.size(); i++) {
        int val = 0;
        int power = 1;
        for (int bitIdx = bitSize-1; bitIdx >= 0; bitIdx--) {
            int idx = bitIdx + i*bitSize;
            if (bits[idx]) {
                val += power;
            }
            power *= 2;
        }
        data[i] = val;
    }
    return data;
}
