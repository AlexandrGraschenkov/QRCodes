//
//  alphabet.hpp
//  QRCode_Mac
//
//  Created by Alexander Graschenkov on 22/11/2018.
//  Copyright © 2018 Alex Development. All rights reserved.
//

#ifndef alphabet_hpp
#define alphabet_hpp

#include <stdio.h>
#include <string>

class Alphabet {
    std::string symbols;
    std::string emptySymbol;
    int messageSize;
    int correctSize;
    int bitSize;
    int fieldSize;
    std::string errorMessage;
    
    std::vector<int> str2data(std::string message) const;
    std::string data2str(const std::vector<int> &messageData) const;
    std::vector<bool> data2bits(std::vector<int> data) const;
    std::vector<int> bits2data(std::vector<bool> bits) const;
    
public:
    Alphabet(int messageSize,
             double correctPercentSize = 0.3,
             std::string fillEmptySpaces = "",
             std::string symbols = "");
    
    int totalMessageSize() const;
    int totalMessageSizeBits() const;
    std::string getError();
    
    std::string decode(std::vector<bool> bits);
    std::vector<bool> encode(std::string str);
};

#endif /* Alphabet_hpp */
