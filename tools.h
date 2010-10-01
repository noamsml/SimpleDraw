/*TOOLS
 * This is the file where all the tools used for drawing on the canvas are defined.
 * The base class for all these is in image.h, mostly for codependency reasons.
 * */

#ifndef TOOLS_H
#define TOOLS_H

#include "gtklibs.h"
#include "image.h"

class LineSettingsPane : public VBox
{
	public:
	Label widthLabel;
	Entry widthEntry;
	
	sigc::signal<void, LineSettingsPane*> width_changed;
	float get_width();
	LineSettingsPane();
};

class FreeHand : public Tool
{
	public:
	double lastx;
	double lasty;
	double width;

	FreeHand();
	void mouse_down(ImageArea* ia, double x, double y);
	void mouse_up(ImageArea* ia, double x, double y);
	void mouse_drag(ImageArea* ia, double x, double y);
};


class DrawLine : public Tool
{
	public:
	double firstx;
	double firsty;
	double lastx;
	double lasty;
	double width;
	Cairo::RefPtr<Cairo::ImageSurface> buffer;
	Cairo::RefPtr<Cairo::SurfacePattern> ptn;

	DrawLine();
	void mouse_down(ImageArea* ia, double x, double y);
	void mouse_up(ImageArea* ia, double x, double y);
	void mouse_drag(ImageArea* ia, double x, double y);
};
#endif
