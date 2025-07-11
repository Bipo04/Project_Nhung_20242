#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
protected:
    uint32_t tickCount;
    virtual void turnOnMode1() override;  // Override phương thức từ lớp cơ sở
    virtual void turnOnMode2() override;  // Override phương thức từ lớp cơ sở
};

#endif // SCREEN2VIEW_HPP
