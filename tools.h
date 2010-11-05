/*TOOLS
 * This is the file where all the tools used for drawing on the canvas are defined.
 * The base class for all these is in image.h, mostly for codependency reasons.
 * */

#ifndef TOOLS_H
#define TOOLS_H

#include "gtklibs.h"
#include "image.h"

class LineSettingsPane : virtual public VBox
{
	public:
	Label widthLabel;
	HScale widthEntry;
	
	float get_line_width();
	LineSettingsPane();
};

class FilledSettingsPane : virtual public VBox
{
	public:
	CheckButton filledEntry;
	
	int get_filled();
	FilledSettingsPane();
};

class RectSettingsPane : virtual public LineSettingsPane, virtual public FilledSettingsPane
{
};

class FreeHand : public Tool
{
	public:
	LineSettingsPane lsp;
	double lastx;
	double lasty;

	FreeHand();
	void mouse_down(ImageArea* ia, double x, double y);
	void mouse_up(ImageArea* ia, double x, double y);
	void mouse_drag(ImageArea* ia, double x, double y);
	Widget* get_settings_pane();
};


class DrawLine : public Tool
{
	public:
	LineSettingsPane lsp;
	double firstx;
	double firsty;
	double lastx;
	double lasty;
	double width;

	DrawLine();
	void mouse_down(ImageArea* ia, double x, double y);
	void mouse_up(ImageArea* ia, double x, double y);
	void mouse_drag(ImageArea* ia, double x, double y);
	Widget* get_settings_pane();
};


class DrawRect : public Tool
{
	public:
	RectSettingsPane lsp;
	double firstx;
	double firsty;

	DrawRect();
	void mouse_down(ImageArea* ia, double x, double y);
	void mouse_up(ImageArea* ia, double x, double y);
	void mouse_drag(ImageArea* ia, double x, double y);
	Widget* get_settings_pane();
};
#endif
