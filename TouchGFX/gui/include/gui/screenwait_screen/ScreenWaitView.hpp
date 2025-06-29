#ifndef SCREENWAITVIEW_HPP
#define SCREENWAITVIEW_HPP

#include <gui_generated/screenwait_screen/ScreenWaitViewBase.hpp>
#include <gui/screenwait_screen/ScreenWaitPresenter.hpp>

class ScreenWaitView : public ScreenWaitViewBase
{
public:
    ScreenWaitView();
    virtual ~ScreenWaitView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREENWAITVIEW_HPP
