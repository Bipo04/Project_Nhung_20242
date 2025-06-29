#ifndef SCREENENDPRESENTER_HPP
#define SCREENENDPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenEndView;

class ScreenEndPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenEndPresenter(ScreenEndView& v);

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

    virtual ~ScreenEndPresenter() {}

private:
    ScreenEndPresenter();

    ScreenEndView& view;
};

#endif // SCREENENDPRESENTER_HPP
