#ifndef SCREENWAITPRESENTER_HPP
#define SCREENWAITPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenWaitView;

class ScreenWaitPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenWaitPresenter(ScreenWaitView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ScreenWaitPresenter() {}

private:
    ScreenWaitPresenter();

    ScreenWaitView& view;
};

#endif // SCREENWAITPRESENTER_HPP
