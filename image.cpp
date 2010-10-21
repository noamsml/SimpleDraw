#include "image.h"
#include <stdio.h>

inline int umod(int a, int b)
{
	if (a < 0) return b + (a % b);
	return a % b;
}

ImageArea::ImageArea (int width, int height, GlobalSettings* gs, Glib::ustring name)
{
	this->width = width;
	this->height = height;
	this->fname = name;



	drawing = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
	buffer = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
	
	general_init(gs);
	
	//Initial image for your enjoyment
	drawingContext->set_source_rgb(255,255,255);
	drawingContext->paint();
	savepoint();
}

ImageArea::ImageArea(Glib::ustring pngname, GlobalSettings* gs)
{
	drawing = Cairo::ImageSurface::create_from_png(pngname);
	this->width = drawing->get_width();
	this->height = drawing->get_height();
	this->fname = pngname;
	buffer = Cairo::ImageSurface::create(drawing->get_format(), this->width, this->height);
	general_init(gs);
	savepoint();
}

void ImageArea::general_init(GlobalSettings* gs)
{
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK );
	this->gs = gs;
	
	bufferContext = Cairo::Context::create(buffer);
	bufferPattern = Cairo::SurfacePattern::create(buffer);
	bufferPattern->set_filter(Cairo::FILTER_GOOD);
	drawingContext = Cairo::Context::create(drawing);
	drawingPattern = Cairo::SurfacePattern::create(drawing);
	drawingPattern->set_filter(Cairo::FILTER_GOOD);
	
	undo_start = undo_current = undo_end = 0;
}

bool ImageArea::on_button_press_event(GdkEventButton* buttons)
{
		set_color(gs->color);
		if (buttons -> button == 1)
		{
			if (gs->tool) gs->tool->mouse_down(this, trans_x(buttons->x), trans_y(buttons->y));
		}
}

bool ImageArea::on_button_release_event(GdkEventButton* buttons)
{
		set_color(gs->color);
		if (buttons -> button == 1)
		{
			if (gs->tool) gs->tool->mouse_up(this, trans_x(buttons->x), trans_y(buttons->y));
		}
}

void ImageArea::savepoint()
{
	undo_history[undo_current] = imgclone(drawing);
	undo_end = undo_current = (undo_current+1) % UNDOLEN;
	if (undo_end == undo_start) undo_start = (undo_start+1) % UNDOLEN;
	printf("%d %d %d\n", undo_start, undo_current, undo_end);
}

void ImageArea::undo()
{
	if (undo_current != undo_start)
	{
		undo_current = umod(undo_current-1, UNDOLEN);
		printf("%d %d %d\n", undo_start, undo_current, undo_end);
		//TODO:resize fix
		Cairo::RefPtr<Cairo::Pattern> cptrn = Cairo::SurfacePattern::create(undo_history[umod(undo_current-1,UNDOLEN)]);
		drawingContext->set_source(cptrn);
		drawingContext->paint();
		update_drawing();
	}
}

void ImageArea::redo()
{
	if (undo_current != undo_end)
	{
		undo_current = (undo_current+1)%UNDOLEN;
		printf("%d %d %d\n", undo_start, undo_current, undo_end);
		//TODO:resize fix
		Cairo::RefPtr<Cairo::Pattern> cptrn = Cairo::SurfacePattern::create(undo_history[umod(undo_current-1,UNDOLEN)]);
		drawingContext->set_source(cptrn);
		drawingContext->paint();
		update_drawing();
	}
}

Cairo::RefPtr<Cairo::ImageSurface> ImageArea::imgclone(Cairo::RefPtr<Cairo::ImageSurface> original)
{
	Cairo::RefPtr<Cairo::ImageSurface> clone = Cairo::ImageSurface::create(original->get_format(), 
							original->get_width(), original->get_height());
	Cairo::RefPtr<Cairo::Context> ccontext = Cairo::Context::create(clone);
	Cairo::RefPtr<Cairo::Pattern> cptrn = Cairo::SurfacePattern::create(original);
	ccontext->set_source(cptrn);
	ccontext->paint();
	return clone;
}

bool ImageArea::on_motion_notify_event(GdkEventMotion* buttons)
{
	set_color(gs->color);
	if (buttons->state && (1<<8))
	{
		if (gs->tool) gs->tool->mouse_drag(this, trans_x(buttons->x), trans_y(buttons->y));
	}
}

bool ImageArea::on_expose_event(GdkEventExpose* expose)
{
	clear_window();
	update_drawing(); //FIXME: Cannot do partial updates for now
}

void ImageArea::clear_window()
{
	Cairo::RefPtr<Cairo::Context> wcontext = get_dwindow() -> create_cairo_context();
	wcontext->set_source_rgb(0.6,0.6,0.6);
	wcontext->paint();
}


void ImageArea::update_stuff(Cairo::RefPtr<Cairo::Context> to, Cairo::RefPtr<Cairo::Pattern> from,
	double x, double y, double w, double h, bool sc=false)
{
	if (sc) to->scale(gs->scale,gs->scale);
	to->set_source(from);
	to->rectangle(x,y,w,h);
	to->fill();
}

void ImageArea::update_drawing()
{
	update_stuff(get_dwindow()->create_cairo_context(), drawingPattern, 0,0,width,height,true);
	set_size(width*gs->scale, height*gs->scale);
}

void ImageArea::update_drawing(double x, double y, double w, double h)
{
	update_stuff(get_dwindow()->create_cairo_context(), drawingPattern, x,y,w,h,true);
	set_size(width*gs->scale, height*gs->scale);
}

void ImageArea::update_buffer()
{
	update_stuff(bufferContext, drawingPattern, 0,0,width,height,false);
}

void ImageArea::update_buffer(double x, double y, double w, double h)
{
	update_stuff(bufferContext, drawingPattern, x,y,w,h,false);
}

void ImageArea::update_from_buffer()
{
	update_stuff(get_dwindow()->create_cairo_context(), bufferPattern, 0,0,width,height,true);
}


void ImageArea::update_from_buffer(double x, double y, double w, double h)
{
	update_stuff(get_dwindow()->create_cairo_context(), bufferPattern, x,y,w,h,true);
}

double ImageArea::trans_x(double x) { return x / gs->scale; }
double ImageArea::trans_y(double y) { return y / gs->scale; }
double ImageArea::untrans_x(double x) { return x*gs->scale; }
double ImageArea::untrans_y(double y) { return y*gs->scale; }

void ImageArea::set_color(Gdk::Color c)
{
	drawingContext->set_source_rgb(c.get_red_p(),c.get_green_p(),c.get_blue_p());
}

Glib::RefPtr<Gdk::Window> ImageArea::get_dwindow()
{
	return get_bin_window();
}

void ImageArea::save_to_png(Glib::ustring fname)
{
	drawing->write_to_png(fname);
	this->fname = fname;
}

Tool::~Tool() {}
