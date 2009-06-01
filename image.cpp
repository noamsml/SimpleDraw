#include "image.h"
#include <stdio.h>

ImageArea::ImageArea (int width, int height)
{
	this->width = width;
	this->height = height;
	tool = NULL;
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK );


	drawing = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
	dcontext = Cairo::Context::create(drawing);
	dpattern = Cairo::SurfacePattern::create(drawing);


	//Initial image for your enjoyment
	dcontext->set_source_rgb(255,255,255);
	dcontext->rectangle(0,0,width, height);
	dcontext->fill();
	dcontext->set_source_rgb(0,0,0);

	dpattern->set_filter(Cairo::FILTER_FAST);

	buffer = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
	bcontext = Cairo::Context::create(buffer);
	bpattern = Cairo::SurfacePattern::create(buffer);
	bpattern->set_filter(Cairo::FILTER_FAST);
	scale = 1;
}

bool ImageArea::on_button_press_event(GdkEventButton* buttons)
{
		if (buttons -> button == 1)
		{
			if (tool) tool->mouse_down(this, trans_x(buttons->x), trans_y(buttons->y));
		}
}

bool ImageArea::on_button_release_event(GdkEventButton* buttons)
{
		if (buttons -> button == 1)
		{
			if (tool) tool->mouse_up(this, trans_x(buttons->x), trans_y(buttons->y));
		}
}


bool ImageArea::on_motion_notify_event(GdkEventMotion* buttons)
{
	if (buttons->state && (1<<8))
	{
		if (tool) tool->mouse_drag(this, trans_x(buttons->x), trans_y(buttons->y));
	}
}

bool ImageArea::on_expose_event(GdkEventExpose* expose)
{
	clear_window();
	update_drawing(expose->area.x, expose->area.y, expose->area.width, expose->area.height);
}

void ImageArea::clear_window()
{
	Cairo::RefPtr<Cairo::Context> wcontext = get_window() -> create_cairo_context();
	wcontext->set_source_rgb(0.6,0.6,0.6);
	wcontext->paint();
}


void ImageArea::update_stuff(Cairo::RefPtr<Cairo::Context> to, Cairo::RefPtr<Cairo::Pattern> from,
	double x, double y, double w, double h, bool sc=false)
{
	if (sc) to->scale(scale,scale);
	to->set_source(from);
	to->rectangle(x,y,w,h);
	to->fill();
}

void ImageArea::update_drawing()
{
	update_stuff(get_window()->create_cairo_context(), dpattern, 0,0,width,height,true);
}

void ImageArea::update_drawing(double x, double y, double w, double h)
{
	update_stuff(get_window()->create_cairo_context(), dpattern, x,y,w,h,true);
}

void ImageArea::update_buffer()
{
	update_stuff(bcontext, dpattern, 0,0,width,height,false);
}

void ImageArea::update_buffer(double x, double y, double w, double h)
{
	update_stuff(bcontext, dpattern, x,y,w,h,false);
}

void ImageArea::update_from_buffer()
{
	update_stuff(get_window()->create_cairo_context(), bpattern, 0,0,width,height,true);
}

void ImageArea::update_from_buffer(double x, double y, double w, double h)
{
	update_stuff(get_window()->create_cairo_context(), bpattern, x,y,w,h,true);
}

double ImageArea::trans_x(double x) { return x / scale; }
double ImageArea::trans_y(double y) { return y / scale; }
double ImageArea::untrans_x(double x) { return x*scale; }
double ImageArea::untrans_y(double y) { return y*scale; }

void ImageArea::change_tool(Tool* t)
{
	if (tool) delete tool;
	tool = t;
}

void ImageArea::set_color(double r, double g, double b)
{
	printf("Called with %f %f %f\n", r, g,b);
	dcontext->set_source_rgb(r,g,b);
}

Tool::~Tool() {}
