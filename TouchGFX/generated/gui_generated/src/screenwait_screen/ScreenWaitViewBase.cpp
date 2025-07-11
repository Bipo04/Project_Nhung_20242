/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/screenwait_screen/ScreenWaitViewBase.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>

ScreenWaitViewBase::ScreenWaitViewBase() :
    flexButtonCallback(this, &ScreenWaitViewBase::flexButtonCallbackHandler)
{
    __background.setPosition(0, 0, 240, 320);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    add(__background);

    image1.setXY(0, 0);
    image1.setBitmap(touchgfx::Bitmap(BITMAP_CHOOSE_MODE_QUES_ID));
    add(image1);

    flexButton1.setBoxWithBorderPosition(0, 0, 70, 29);
    flexButton1.setBorderSize(5);
    flexButton1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0, 102, 153), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 51, 102), touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton1.setAlpha(0);
    flexButton1.setAction(flexButtonCallback);
    flexButton1.setPosition(26, 111, 70, 29);
    add(flexButton1);

    flexButton2.setBoxWithBorderPosition(0, 0, 75, 29);
    flexButton2.setBorderSize(5);
    flexButton2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0, 102, 153), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 51, 102), touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton2.setAlpha(0);
    flexButton2.setAction(flexButtonCallback);
    flexButton2.setPosition(141, 111, 75, 29);
    add(flexButton2);
}

ScreenWaitViewBase::~ScreenWaitViewBase()
{

}

void ScreenWaitViewBase::setupScreen()
{

}

void ScreenWaitViewBase::flexButtonCallbackHandler(const touchgfx::AbstractButtonContainer& src)
{
    if (&src == &flexButton1)
    {
        //Interaction1
        //When flexButton1 clicked change screen to Screen1
        //Go to Screen1 with no screen transition
        application().gotoScreen1ScreenNoTransition();
    }
    if (&src == &flexButton2)
    {
        //Interaction2
        //When flexButton2 clicked change screen to Screen3
        //Go to Screen3 with no screen transition
        application().gotoScreen3ScreenNoTransition();
    }
}
