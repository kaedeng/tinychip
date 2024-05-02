#include <SDL2/SDL.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cpu.h"
#include "display.h"


// i hate this code

void cpu::setPC(const uint16_t entry){
    _PC = entry;
}

char* cpu::getName() const{
    return _romName;
}

bool cpu::initCpu(char* name, int& gameState){
    /*
    uint32_t entry = 0x200;
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
    for (size_t i = 0; i < sizeof(font); ++i) {
    _ram[i] = font[i];
    }
    _romName = name;
    std::ifstream file(name);
    if(file.fail()){
        std::cerr << "Could not open rom: " << name << '\n';
        return false; 
    }
    file.seekg(0, std::ios::end);
    long long fileSize = file.tellg();
    long long availableSpace = sizeof _ram - entry;
    file.seekg(0, std::ios::beg);
    if (fileSize > availableSpace) {
        std::cerr << "Rom exceeds max size allowed in ram, rom is :" << name << '\n';
        return false;
    }
    if (file.read(reinterpret_cast<char*>(&_ram[entry]), fileSize).gcount() != fileSize){
        std::cerr << "idkbroke";
        return false;
    }
    file.close();
    setPC(0x200);
    _stackPtr = &_stack[0];
    gameState = 1;
    return true; 
    */
    const uint32_t entry_point = 0x200; // CHIP8 Roms will be loaded to 0x200
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0   
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1  
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2 
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4    
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
    memset(_ram, 0, sizeof(_ram));
    memset(_display, 0, sizeof(_display));
    memset(_pixels, 0, sizeof(_pixels));
    memset(_V, 0, sizeof(_V));
    // Load font 
    memcpy(&_ram[0], font, sizeof(font));
   

    // Open ROM file
    std::string filePath = "./games/" + std::string(name);
    FILE *rom = fopen(filePath.c_str(), "rb");
    if (!rom) {
        SDL_Log("Rom file %s is invalid or does not exist\n", name);
        return false;
    }

    // Get/check rom size
    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    const size_t max_size = sizeof _ram - entry_point;
    rewind(rom);

    if (rom_size > max_size) {
        //SDL_Log("Rom file %s is too big! Rom size: %llu, Max size allowed: %llu\n", name, (long long unsigned)rom_size, (long long unsigned)max_size);
        SDL_Log("Rom too big");
        return false;
    }

    // Load ROM
    if (fread(&_ram[entry_point], rom_size, 1, rom) != 1) {
        SDL_Log("Could not read Rom file %s into CHIP8 memory\n", name);
        return false;
    }
    fclose(rom);

    // Set achine defaults
    gameState = 1;     // Default machine state to on/running
    _PC = entry_point;    // Start program counter at ROM entry point
    _romName = name;
    _stackPtr = &_stack[0];
    return true;
    
}

void cpu::emulateInst(display& disp) {
    //gets next opcode
    _instruct.code = (uint16_t)(_ram[_PC] << 8 | _ram[_PC+1]); // bit fuckery, need to record 2 bytes of info and change from little endian to big endian ... -> 0X -> X0 -> XY
    _PC = (uint16_t)(_PC + 2);  
    _instruct.NNN = (uint16_t)(_instruct.code & 0x0FFF); // & masking, removes specified bits ... DXYN -> 0XYN
    _instruct.NN = (uint16_t)(_instruct.code & 0x0FF); // DXYN -> 00YN
    _instruct.N = (uint16_t)(_instruct.code & 0x0F); // DXYN -> 000N
    _instruct.X = (uint16_t)((_instruct.code >> 8) & 0x0F); // DXYN -> 000X
    _instruct.Y = (uint16_t)((_instruct.code >> 4) & 0x0F); // DXYN -> 000Y

    switch((uint16_t)(_instruct.code >> 12) & 0x0F ){
        case 0x00: // checks all instructs with 0x0 in front, for ex. 00E0 - CLS and 00EE - RET
            if(_instruct.NN == 0xE0) { // 00E0 - CLS
                //memset(&_display[0], false, sizeof _display);
                disp.draw = true; // Will update screen on next 60hz tick
            }
            else if(_instruct.NN == 0xEE) { // 00EE - RET
                _PC = *--_stackPtr;
            }
            else {}
            break;  
        case 0x01:
            // 0x1NNN: Jump to address NNN
            _PC = _instruct.NNN;    // Set program counter so that next opcode is from NNN
            break;
        case 0x02:
            *_stackPtr++ = _PC;  
            _PC = _instruct.NNN;
            break;
        case 0x03:
            // 0x3XNN: Check if VX == NN, if so, skip the next instruction
            if (_V[_instruct.X] == _instruct.NN)
                _PC = (uint16_t)(_PC + 2);       // Skip next opcode/instruction
            break;
        case 0x04:
            // 0x4XNN: Check if VX != NN, if so, skip the next instruction
            if (_V[_instruct.X] != _instruct.NN)
                _PC = (uint16_t)(_PC + 2);       // Skip next opcode/instruction
            break;

        case 0x05:
            if (_instruct.N != 0) break;
            if (_V[_instruct.X] == _V[_instruct.Y]){
                _PC = (uint16_t)(_PC + 2);
            }
            break;
        case 0x06: // 6xkk - LD Vx, byte
            _V[_instruct.X] = _instruct.NN;
            break;
        case 0x07:
            // 0x7XNN: Set register VX += NN
            _V[_instruct.X] = (uint8_t)(_V[_instruct.X] + _instruct.NN);
            break;
        case 0x08:
            switch(_instruct.N){
                case 0:
                    // 0x8XY0: Set register VX = VY
                    _V[_instruct.X] = _V[_instruct.Y];
                    break;

                case 1:
                    // 0x8XY1: Set register VX |= VY
                    _V[_instruct.X] |= _V[_instruct.Y];
                    break;

                case 2:
                    // 0x8XY2: Set register VX &= VY
                     _V[_instruct.X] &=  _V[_instruct.Y];
                    break;

                case 3:
                    // 0x8XY3: Set register VX ^= VY
                     _V[_instruct.X] ^=  _V[_instruct.Y];
                    break;

                case 4:
                    // 0x8XY4: Set register VX += VY, set VF to 1 if carry, 0 if not 
                    if(_V[_instruct.Y] <= _V[_instruct.X]) {_V[0xF] = 1;}
                    _V[_instruct.X] = (uint8_t)(_V[_instruct.X] + _V[_instruct.Y]);
                    break;

                case 5: 
                    // 0x8XY5: Set register VX -= VY, set VF to 1 if there is not a borrow (result is positive/0)
                    //if(_V[_instruct.Y] <= _V[_instruct.X]){_V[0xF] = 1;}
                    _V[0xF] = (_V[_instruct.Y] <= _V[_instruct.X]);
                    _V[_instruct.X] = (uint8_t)(_V[_instruct.X] - _V[_instruct.Y]);
                    break;

                case 6:
                    // 0x8XY6: Set register VX >>= 1, store shifted off bit in VF
                    _V[0xF] = _V[_instruct.X] & 1;
                    _V[_instruct.X] = (uint8_t)(_V[_instruct.X] >> 1); // Set VX = VY result
                    break;

                case 7:
                    // 0x8XY7: Set register VX = VY - VX, set VF to 1 if there is not a borrow (result is positive/0)
                    _V[0xF] = _V[_instruct.X] <= _V[_instruct.Y];
                    _V[_instruct.X] = (uint8_t)(_V[_instruct.Y] - _V[_instruct.X]);
                    break;
                
                case 0xE:
                    // 0x8XYE: Set register VX <<= 1, store shifted off bit in VF
                    _V[0xF] = (_V[_instruct.X] & 0x80) >> 7;
                    _V[_instruct.X] = (uint8_t)(_V[_instruct.X] << 1); // Set VX = VY result
                    break;
                default:
                    break;
            }   
            break;
        case 0x09:
            // 0x9XY0: Check if VX != VY; Skip next instruction if so
            if (_V[_instruct.X] != _V[_instruct.Y])
                _PC = (uint16_t)(_PC + 2);
            break;

        case 0x0A: // ANNN - LD I, addr
            _index = _instruct.NNN;
            break;
        case 0x0B:
            // 0xBNNN: Jump to V0 + NNN
            _PC =(uint16_t)(_V[0] + _instruct.NNN);
            break;
        case 0x0E:
            if (_instruct.NN == 0x9E) {
                // 0xEX9E: Skip next instruction if key in VX is pressed
                if (keymap[_V[_instruct.X]])
                    _PC = (uint16_t)(_PC + 2);
            } 
            else if (_instruct.NN == 0xA1) {
                // 0xEXA1: Skip next instruction if key in VX is not pressed
                if (!keymap[_V[_instruct.X]])
                    _PC = (uint16_t)(_PC + 2);
            }
            break;
        case 0x0F:
            switch(_instruct.NN){
                case 0x0A: {
                    static bool keyPress = false;
                    static uint8_t key = 0xFF;
                    for (uint8_t i = 0; key == 0xFF && i < sizeof keymap; i++) 
                        if (keymap[i]) {
                            key = i;    // Save pressed key to check until it is released
                            keyPress = true;
                            break;
                        }
                    if (!keyPress) {_PC = (uint16_t)(_PC - 2);}
                    else {
                        // A key has been pressed, also wait until it is released to set the key in VX
                        if (keymap[key])     // "Busy loop" CHIP8 emulation until key is released
                            _PC = (uint16_t)(_PC - 2);
                        else {
                            _V[_instruct.X] = key;     // VX = key 
                            key = 0xFF;                        // Reset key to not found 
                            keyPress = false;           // Reset to nothing pressed yet
                        }
                    }
                    break;

                case 0x1E:
                    _index = (uint16_t)(_index + _V[_instruct.X]);
                    break;
                case 0x07:
                    _V[_instruct.X] = _delayTimer;
                    break;
                case 0x15:
                    _delayTimer = _V[_instruct.X];
                    break;
                case 0x18:
                    _soundTimer = _V[_instruct.X];
                    break;
                case 0x29:
                    _index = (uint16_t)(_V[_instruct.X]*5);
                    break; // Add break statement here
                case 0x55:
                    for (uint8_t i = 0; i <= _instruct.X; i++)  {
                        _ram[_index + i] = _V[i]; // Increment I each time
                    }
                    break;
                case 0x65:
                    for (uint8_t i = 0; i <= _instruct.X; i++) {
                        _V[i] = _ram[_index+i]; // Increment I each time
                    }
                    break;
                }
                case 0x33: 
                    uint8_t bcd = _V[_instruct.X];
                    _ram[_index+2] = bcd % 10;
                    bcd /= 10;
                    _ram[_index+1] = bcd % 10;
                    bcd /= 10;
                    _ram[_index] = bcd;
                    break;
            }
            break;
        
        case 0x0C:
            // 0xCXNN: Sets register VX = rand() % 256 & NN (bitwise AND)
            _V[_instruct.X] = (uint8_t)((rand() % 256) & _instruct.NN);
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

void cpu::updateTimers(){
    if(_delayTimer > 0) {_delayTimer--;}
    if(_soundTimer > 0) {
        _soundTimer--;
        //play sound
    }
    else {
        //no sound
    }
}