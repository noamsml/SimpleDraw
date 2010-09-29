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

class ImageArea : public Layout
{
	public:
	Glib::ustring name;
	Cairo::RefPtr<Cairo::ImageSurface> drawing;
	Cairo::RefPtr<Cairo::SurfacePattern> drawingPattern;
	Cairo::RefPtr<Cairo::Context> drawingContext;

	//Display buffer
	//used for tools that need to display stuff on the screen that will
	//not be part of the drawing itself
	Cairo::RefPtr<Cairo::ImageSurface> buffer;
	Cairo::RefPtr<Cairo::SurfacePattern> bufferPattern;
	Cairo::RefPtr<Cairo::Context> bufferContext;
	Tool* tool;
	int width;
	int height;

	float scale;

	ImageArea(int width, int height, Glib::ustring name);

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

	void resize_img(double sc);
	Glib::RefPtr<Gdk::Window> get_dwindow();
};

#endif
