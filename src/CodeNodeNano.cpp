#include "CodeNodeNano.hpp"

#include <utility>

CodeNodeNano::CodeNodeNano() :
    cpu(read, write, cycle),
    cyclesCounter(0),
    cyclesTarget(0),
    clockPaused(false)
{
    poweredOn = false;
}

void CodeNodeNano::tick()
{
    if(!poweredOn || clockPaused) return;

    cyclesTarget += CLOCK_FREQUENCY / GAME_TICK_RATE;

    gpio.tickInterrupts();

    currentInstance = this;
    cpu.Run(cyclesTarget - cyclesCounter, cyclesCounter);

    gpio.swapBuffers();
}

void CodeNodeNano::cycle()
{
    if(!poweredOn) return;

    cyclesTarget += 1;

    if(cyclesTarget % (CLOCK_FREQUENCY / GAME_TICK_RATE) == 0)
        gpio.tickInterrupts();

    currentInstance = this;
    cpu.Run(cyclesTarget - cyclesCounter, cyclesCounter);
    
    if(cyclesTarget % (CLOCK_FREQUENCY / GAME_TICK_RATE) == 0)
        gpio.swapBuffers();
}

void CodeNodeNano::reset()
{
    currentInstance = this;
    ram.reset();
    // rom.reset();
    gpio.reset();
    cpu.Reset();
    cyclesCounter = 0;
    cyclesTarget = 0;
}

void CodeNodeNano::powerOn()
{
    poweredOn = true;
    reset();
}

void CodeNodeNano::powerOff()
{
    poweredOn = false;
}

bool CodeNodeNano::isPoweredOn() const
{
    return poweredOn;
}

mos6502& CodeNodeNano::CPU()
{
    return cpu;
}

CNGPIO<CodeNodeNano::GPIO_NUM_PINS>& CodeNodeNano::GPIO()
{
    return gpio;
}

CNRAM<CodeNodeNano::RAM_SIZE>& CodeNodeNano::RAM()
{
    return ram;
}

CNROM<CodeNodeNano::ROM_SIZE>& CodeNodeNano::ROM()
{
    return rom;
}

CodeNodeNano* CodeNodeNano::currentInstance = nullptr;

uint8_t CodeNodeNano::read(uint16_t address)
{
    if(currentInstance == nullptr) return 0;

    currentInstance->m_busAddress = address;
    currentInstance->m_busData = 0;
    currentInstance->m_busRw = false;

    if(0xFFFF - ROM_SIZE < address)
    {
        currentInstance->m_busData = currentInstance->rom.read(address - (0x10000 - ROM_SIZE));
        return currentInstance->m_busData;
    }
    else if(0x7000 <= address && address < (0x7000 + currentInstance->gpio.size()))
    {
        currentInstance->m_busData = currentInstance->gpio.read(address - 0x7000);
        return currentInstance->m_busData;
    }

    return (currentInstance->m_busData = currentInstance->ram.read(address));
}

void CodeNodeNano::write(uint16_t address, uint8_t value)
{
    if(currentInstance == nullptr) return;

    currentInstance->m_busAddress = address;
    currentInstance->m_busData = value;
    currentInstance->m_busRw = true;

    if(0xFFFF - ROM_SIZE < address)
    {
        currentInstance->rom.write(address - (0x10000 - ROM_SIZE), value);
        return;
    }
    else if(0x7000 <= address && address < (0x7000 + currentInstance->gpio.size()))
    {
        currentInstance->gpio.write(address - 0x7000, value);
        return;
    }


    currentInstance->ram.write(address, value);
}

void CodeNodeNano::cycle(mos6502* cpu)
{
    if(currentInstance == nullptr) return;

    bool shouldInterrupt = false;

    shouldInterrupt |= currentInstance->gpio.shouldInterrupt();

    if(shouldInterrupt)
        cpu->IRQ();
}