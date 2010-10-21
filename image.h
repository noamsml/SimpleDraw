#ifndef IMAGE_H
#define IMAGE_H

#include "gtklibs.h"
#define UNDOLEN 5

class ImageArea;
class Tool;
class GlobalSettings;

class Tool
{
	public:
	virtual void mouse_down(ImageArea* ia,  double x, double y)=0;
	virtual void mouse_up(ImageArea* ia,  double x, double y)=0;
	virtual void mouse_drag(ImageArea* ia,  double x, double y)=0;
	virtual Widget* get_settings_pane()=0;
	virtual ~Tool();
};

class GlobalSettings
{
	public:
		Gdk::Color color;
		float scale;
		Tool* tool;
};

class ImageArea : public Layout
{
	protected:
	void general_init(GlobalSettings* gs);
	
	public:
	Cairo::RefPtr<Cairo::ImageSurface> undo_history[5];
	int undo_start, undo_end, undo_current;
	
	void savepoint();
	Cairo::RefPtr<Cairo::ImageSurface> imgclone(Cairo::RefPtr<Cairo::ImageSurface> original);
	
	Cairo::RefPtr<Cairo::ImageSurface> drawing;
	Cairo::RefPtr<Cairo::SurfacePattern> drawingPattern;
	Cairo::RefPtr<Cairo::Context> drawingContext;
	
	
	//Display buffer
	//used for tools that need to display stuff on the screen that will
	//not be part of the drawing itself
	Cairo::RefPtr<Cairo::ImageSurface> buffer;
	Cairo::RefPtr<Cairo::SurfacePattern> bufferPattern;
	Cairo::RefPtr<Cairo::Context> bufferContext;
	
	GlobalSettings* gs;
	Glib::ustring fname;
	
	int width;
	int height;
	
	ImageArea(int width, int height, GlobalSettings* settings, Glib::ustring name="");
	ImageArea(Glib::ustring pngpath, GlobalSettings* settings);

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
	void undo();
	void redo();

	void clear_window();
	void set_color(Gdk::Color);

	double trans_x(double x);
	double trans_y(double y);

	double untrans_x(double x);
	double untrans_y(double y);
	
	void save_to_png(Glib::ustring fname);

	void resize_img(double sc);
	Glib::RefPtr<Gdk::Window> get_dwindow();
};

#endif
