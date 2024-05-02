#include "cpu.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <iostream>

// i hate this code

void cpu::setPC(const uint16_t entry){
    _PC = entry;
}

bool cpu::initCpu(std::string name, int& gameState){
    uint16_t entry = 0x200;
    uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0 -> 11110000 
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1    10010000
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2    10010000
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3    10010000
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4    11110000
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
    };
    std::copy(font, font + sizeof(font), _ram);
    _romName = name;
    std::ifstream file("./src/" + name);
    if(file.fail()){
        std::cerr << "Could not open rom: " << name << '\n';
        return false; 
    }
    file.seekg(0, std::ios::end);
    long long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (fileSize > (long long)(sizeof(_ram) - entry)) {
        std::cerr << "Rom exceeds max size allowed in ram, rom is :" << name << '\n';
        return false;
    }
    if (file.read(reinterpret_cast<char*>(&_ram[entry]), fileSize).gcount() != fileSize) { // read into rom fuckery || POTENTIAL ERROR? might not be reading all into ram
        std::cerr << "Could not read into rom file, rom is :" << name << '\n';
        return false;
    }
    file.close();
    setPC(0x200);
    _stackPtr = &_stack[0];
    gameState = 1;
    return true;
}

void cpu::emulateInst(display& disp) { // learned too much bit shit going to cry now
    //gets next opcode
    _instruct.code = (uint16_t)(_ram[_PC] << 8 | _ram[_PC+1]); // bit fuckery, need to record 2 bytes of info and change from little endian to big endian ... -> 0X -> X0 -> XY
    _PC ++;
    _PC ++;
    _instruct.NNN = (uint16_t)(_instruct.code & 0x0FFF); // & masking, removes specified bits ... DXYN -> 0XYN
    _instruct.NN = (uint16_t)(_instruct.code & 0x0FF); // DXYN -> 00YN
    _instruct.N = (uint16_t)(_instruct.code & 0x0F); // DXYN -> 000N
    _instruct.X = (uint16_t)((_instruct.code >> 8) & 0x0F); // DXYN -> 000X
    _instruct.Y = (uint16_t)((_instruct.code >> 4) & 0x0F); // DXYN -> 000Y

    switch((uint16_t)(_instruct.code >> 12) & 0x0F ){
        case 0x00: // checks all instructs with 0x0 in front, for ex. 00E0 - CLS and 00EE - RET
            if(_instruct.NN == 0xE0) { // 00E0 - CLS
                for (uint64_t i = 0; i < sizeof(_display); i++) {
                    _display[i] = false;
                }
            }
            else if(_instruct.NN == 0xEE) { // 00EE - RET
                _PC = (uint16_t)(*_stackPtr - 1);
                (*_stackPtr)--;
            }
            break;  
        case 0x02:
            *_stackPtr = _PC; // stores current addr in stack
            (*_stackPtr)++; // next index at stack
            _PC = _instruct.NNN; // PC set to subroutine's address, next opcode will be recieved from that subroutine
            break;
        case 0x06: // 6xkk - LD Vx, byte
            _V[_instruct.X] = _instruct.NN;
            break;
        case 0x07:
            // 0x7XNN: Set register VX += NN
            _V[_instruct.X] = (uint8_t)(_V[_instruct.X] + _instruct.NN);
            break;
        case 0x0A: // ANNN - LD I, addr
            _index = _instruct.NNN;
            break;
        case 0x0D: // Dxyn - DRW Vx, Vy, nibble
            uint8_t xCoord = uint8_t(_V[_instruct.X] % 64);
            uint8_t yCoord = uint8_t(_V[_instruct.Y] % 32);
            uint8_t xOrig = xCoord;
            _V[0xF] = 0;
            for(uint8_t i = 0; i < _instruct.N; i++){
                uint8_t spriteData = _ram[_index + i];
                xCoord = xOrig;
                for(int8_t j = 7; j>=0; j--){
                    bool *check = &_display[yCoord * 64 + xCoord]; // check display pixel if on
                    bool spriteBit = (spriteData & (1 << j)); // check ram data if on
                    if(*check && spriteBit){ // if both on
                        _V[0xF] = 1; // set carry flag
                    }
                    // XOR bit shit to turn on/off because both cant be on or else I'll die.
                    *check ^= spriteBit;
                    if (++xCoord >= 64) break; // safety before going out of screen in X axis
                }
                if (++yCoord >= 32) break; // safety before going out of screen in Y axis
            }
            disp.draw = true;
            break;
    }
}
