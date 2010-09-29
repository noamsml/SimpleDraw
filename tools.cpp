#include "tools.h"

void FreeHand::mouse_down(ImageArea* ia, double x, double y)
{
	lastx = x;
	lasty = y;
}

void FreeHand::mouse_up(ImageArea* ia, double x, double y)
{
	ia->drawingContext->save();
	ia->drawingContext->set_line_cap(Cairo::LINE_CAP_ROUND);
	ia->drawingContext->move_to(lastx,lasty);
	ia->drawingContext->line_to(x,y);
	ia->drawingContext->set_line_width(width);
	ia->drawingContext->stroke();
	ia->drawingContext->restore();

	//sorry
	ia->update_buffer();
	ia->update_from_buffer();
}

FreeHand::FreeHand()
{
	width = 15;
}

void FreeHand::mouse_drag(ImageArea* ia, double x, double y)
{
	double boxx = (x < lastx) ? x - 10 : lastx - 10;
	double boxy = (y < lasty) ? y - 10 : lasty - 10;
	double boxw = (x < lastx) ? lastx - x + 20 : x - lastx + 20;
	double boxh = (y < lasty) ? lasty - y + 20 : y - lasty + 20;

	ia->drawingContext->save();
	ia->drawingContext->set_line_cap(Cairo::LINE_CAP_ROUND);
	ia->drawingContext->move_to(lastx,lasty);
	ia->drawingContext->line_to(x,y);
	ia->drawingContext->set_line_width(width);
	ia->drawingContext->stroke();
	ia->drawingContext->restore();

	lastx=x;
	lasty=y;
	ia->update_drawing(boxx, boxy, boxw, boxh);
}



DrawLine::DrawLine()
{
	width = 5;
}


void DrawLine::mouse_down(ImageArea* ia, double x, double y)
{
	firstx = x;
	firsty = y;
	lastx = x;
	lasty = y;
}

void DrawLine::mouse_drag(ImageArea* ia, double x, double y)
{

	ia->update_buffer();

	Cairo::RefPtr<Cairo::Context> wcontext = ia->bufferContext;
	wcontext->save();
	wcontext->set_source(ia->drawingContext->get_source());
	wcontext->set_line_cap(Cairo::LINE_CAP_ROUND);
	wcontext->move_to(firstx,firsty);
	wcontext->line_to(x,y);
	wcontext->set_line_width(width);
	wcontext->stroke();
	wcontext->restore();

	ia->update_from_buffer();
	lastx = x;
	lasty = y;
}

void DrawLine::mouse_up(ImageArea* ia, double x, double y)
{
	ia->drawingContext->save();
	ia->drawingContext->set_line_cap(Cairo::LINE_CAP_ROUND);
	ia->drawingContext->move_to(firstx,firsty);
	ia->drawingContext->line_to(x,y);
	ia->drawingContext->set_line_width(width);
	ia->drawingContext->stroke();
	ia->drawingContext->restore();

	ia->update_drawing();
	lastx = x;
	lasty = y;
}

