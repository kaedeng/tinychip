#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>

class opcode {
    public:
    uint16_t code; 
    uint16_t NNN; // 12 bit address
    uint8_t NN; // 8 bit const
    uint8_t N; // 4 bit const
    uint8_t X; // 4 bit register identifier
    uint8_t Y; // 4 bit register identifier
};

#endif