#ifndef SCREENENDVIEW_HPP
#define SCREENENDVIEW_HPP

#include <gui_generated/screenend_screen/ScreenEndViewBase.hpp>
#include <gui/screenend_screen/ScreenEndPresenter.hpp>

class ScreenEndView : public ScreenEndViewBase
{
public:
    ScreenEndView();
    virtual ~ScreenEndView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREENENDVIEW_HPP
