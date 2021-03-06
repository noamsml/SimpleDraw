#ifndef MAIN_H
#define MAIN_H

#include "gtklibs.h"
#include "image.h"
#include "tools.h"

class PngFileChooser : public FileChooserDialog
{
	public:
	PngFileChooser(int save=0);
};

class ToolColumn : public TreeModelColumnRecord {
	public:
		ToolColumn();
		
		TreeModelColumn<Glib::ustring> colname;
		TreeModelColumn<Tool*> tooldata;
};

class NewDocumentBox : public HBox {
	private:
		Label wlabel;
		Entry wentry;
		Label hlabel;
		Entry hentry;
		Button newbtn;
		
		void rebounce_signal();
	public:
		sigc::signal<void, int, int > new_doc_made;
		NewDocumentBox();
		
		
};

class MainWindow : public Window
{
	private:
	VPaned tools;
	Frame toolbox;
	Frame settingbox;
	Notebook documents;
	VBox winbox;
	HBox mainbox;
	VButtonBox toolbox_vert;
	VBox mainarea_layout;
	HBox minibar_layout;
	NewDocumentBox newdoc;

	HScale scale_range;
	Label scale_label;
	HBox scale_hbox;
	ColorButton choose_color;
	PngFileChooser open_dialog;
	PngFileChooser save_dialog;
	
	ToolColumn toolcol;
	Glib::RefPtr<ListStore> toolstore;
	TreeView toolview;
	Widget* menubar;
	
	GlobalSettings gs;
	
	Glib::RefPtr<ActionGroup> MenuActions;
	Glib::RefPtr<UIManager> MenuUI;
	
	
	void populate_tools();
	void populate_menus();
	void populate_window();
	TreeModel::Row add_tool(Glib::ustring name, Tool* tool); 
	
	public:
	MainWindow();
	void new_image_tab(int w, int h);
	void new_image_dialog();
	void open_image_dialog();
	void save_image();
	void save_image_dialog();
	void open_image_from_string(Glib::ustring str);
	void close_document();
	
	void undo_current();
	void redo_current();
	
	void quit();
	
	ImageArea* get_current_tab();
	void set_current_tabstr(Glib::ustring str);
	int add_new_tab(ImageArea* ia, Glib::ustring tabname);
	
	

	void scale_activated();
	void change_color();
	void tool_changed();
	
};



#endif
