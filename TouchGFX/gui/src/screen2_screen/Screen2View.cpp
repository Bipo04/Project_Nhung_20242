#include <gui/screen2_screen/Screen2View.hpp>
#include "cmsis_os2.h"
#include "main.h" 

extern osMessageQueueId_t Queue2Handle;

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::turnOnMode1()
{
    // Implementation for turning on mode 1
    uint8_t mode = 0; 
    uint32_t count = osMessageQueueGetCount(Queue2Handle);
    if (count < 1) 
    {
        osMessageQueuePut(Queue2Handle, &mode, 0, 10);
    }
}

void Screen2View::turnOnMode2()
{
    // Implementation for turning on mode 2
    uint8_t mode = 1; 
    uint32_t count = osMessageQueueGetCount(Queue2Handle);
    if (count < 1) 
    {
        osMessageQueuePut(Queue2Handle, &mode, 0, 10);
    }
}
