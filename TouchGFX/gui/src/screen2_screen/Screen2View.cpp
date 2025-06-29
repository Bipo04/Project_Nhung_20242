#include <gui/screen2_screen/Screen2View.hpp>
#include "cmsis_os2.h"
#include "main.h" 

extern osMessageQueueId_t Queue2Handle;

Screen2View::Screen2View()
{
    tickCount = 0;
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

void Screen2View::handleTickEvent()
{
    tickCount++;
    switch(tickCount % 18) {
        case 0:  // Phase 1 - First image disappears
            image6.setVisible(false);
            image7.setVisible(true);
            image5.setVisible(true);
            break;
            
        case 3:  // Phase 2 - Second image disappears
            image6.setVisible(false);
            image7.setVisible(false);
            image5.setVisible(true);
            break;
            
        case 6:  // Phase 3 - Third image disappears
            image6.setVisible(false);
            image7.setVisible(false);
            image5.setVisible(false);
            break;
            
        case 9:  // Phase 4 - First image appears
            image6.setVisible(true);
            image7.setVisible(false);
            image5.setVisible(false);
            break;
            
        case 12:  // Phase 5 - Second image appears
            image6.setVisible(true);
            image7.setVisible(true);
            image5.setVisible(false);
            break;
            
        case 15:  // Phase 6 - Third image appears
            image6.setVisible(true);
            image7.setVisible(true);
            image5.setVisible(true);
            break;
    }
    invalidate();
}
