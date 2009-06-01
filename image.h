#ifndef IMAGE_H
#define IMAGE_H

#include "gtklibs.h"

class ImageArea;

class Tool
{
	public:
	virtual void mouse_down(ImageArea* ia, double x, double y)=0;
	virtual void mouse_up(ImageArea* ia, double x, double y)=0;
	virtual void mouse_drag(ImageArea* ia, double x, double y)=0;
	virtual ~Tool();
};

class ImageArea : public DrawingArea
{
	public:
	Cairo::RefPtr<Cairo::ImageSurface> drawing;
	Cairo::RefPtr<Cairo::SurfacePattern> dpattern;
	Cairo::RefPtr<Cairo::Context> dcontext;

	//Display buffer
	Cairo::RefPtr<Cairo::ImageSurface> buffer;
	Cairo::RefPtr<Cairo::SurfacePattern> bpattern;
	Cairo::RefPtr<Cairo::Context> bcontext;
	Tool* tool;
	int width;
	int height;

	int scale;

	ImageArea(int width, int height);

	bool on_button_press_event(GdkEventButton* buttons);
	bool on_button_release_event(GdkEventButton* buttons);
	bool on_motion_notify_event(GdkEventMotion* buttons);
	bool on_expose_event(GdkEventExpose* expose);

	void update_stuff(Cairo::RefPtr<Cairo::Context> to, Cairo::RefPtr<Cairo::Pattern> from, double x, double y, double w, double h, bool sc);
	void update_drawing();
	void update_drawing(double x, double y, double w, double h);
	void update_buffer();
	void update_buffer(double x, double y, double w, double h);
	void update_from_buffer();
	void update_from_buffer(double x, double y, double w, double h);

	void clear_window();
	void set_color(double r, double g, double b);

	double trans_x(double x);
	double trans_y(double y);

	double untrans_x(double x);
	double untrans_y(double y);

	void change_tool(Tool* t);
};

#endif
