#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include <fstream>
#include "opcode.h"
#include "display.h"

class display; // forward declaration because idk why but display.h is not working? 

class cpu{
    public:
    void setPC(const uint16_t); // Should start Program Counter to entry point of rom
    bool initCpu(std::string, int&);
    void emulateInst(display&);
    bool _display[64*32]; // keeps track of state on pixels in display, 64x32 bits
    bool _pixels[64*32]; // keeps track of pixels
    private:
    uint8_t _ram[4096]; // Memory
    uint8_t _V[16]; // general purpose registers, V0-VF
    uint16_t _index; // Mem address store
    uint16_t _stack[12]; // mem stack
    uint16_t* _stackPtr;
    uint8_t _delayTimer; // decrements at 60Hz when >0
    uint8_t _soundTimer; // decrements at 60Hz and plays tone when >0
    bool _keymap[16]; // mapping the key layout
    std::string _romName; // Rom name, pass by CLA or std::cin
    uint16_t _PC;
    opcode _instruct;
};

#endif