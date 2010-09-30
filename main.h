#ifndef MAIN_H
#define MAIN_H

#include "gtklibs.h"
#include "image.h"
#include "tools.h"

class ToolColumn : public TreeModelColumnRecord {
	public:
		ToolColumn();
		
		TreeModelColumn<Glib::ustring> colname;
		TreeModelColumn<Tool*> tooldata;
};

class MainWindow : public Window
{
	private:
	VPaned tools;
	Frame toolbox;
	Frame settingbox;
	Notebook documents;
	HBox mainbox;
	VButtonBox toolbox_vert;
	VBox mainarea_layout;
	HBox minibar_layout;

	Entry scale_entry;
	Label scale_label;
	HBox scale_hbox;
	ColorButton choose_color;
	
	ToolColumn toolcol;
	Glib::RefPtr<ListStore> toolstore;
	TreeView toolview;
	
	Glib::RefPtr<ActionGroup> MenuActions;
	Glib::RefPtr<UIManager> MenuUI;
	
	
	void populate_tools();
	void populate_menus();
	void populate_window();
	TreeModel::Row add_tool(Glib::ustring name, Tool* tool); 
	
	public:
	MainWindow();
	void new_image_tab();
	void quit();
	ImageArea* get_current_tab();
	

	void scale_activated();
	void change_color();
	void tool_changed();
	void change_all_data(); //right now changes Image to match MainWindow. Can reverse later.
	
};



#endif
