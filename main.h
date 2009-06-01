#ifndef MAIN_H
#define MAIN_H

#include "gtklibs.h"
#include "image.h"
#include "tools.h"

class MainWindow : public Window
{
	public:
	VPaned tools;
	Frame toolbox;
	Frame settingbox;
	Notebook documents;
	HBox mainbox;
	VButtonBox toolbox_vert;
	ImageArea randomtestarea;
	VBox mainarea_layout;
	HBox minibar_layout;

	Entry scale_entry;
	Label scale_label;
	HBox scale_hbox;
	ColorButton choose_color;

	MainWindow();

	void scale_activated();
	void change_color();
};


#endif
