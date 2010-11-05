#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

void MainWindow::populate_tools()
{
	toolstore = ListStore::create(toolcol);
	toolview.set_model(toolstore);
	toolview.append_column("Name", toolcol.colname);
	
	
	FreeHand* starttool = new FreeHand();
	gs.tool = starttool;
	settingbox.add(*(gs.tool->get_settings_pane()));
	Glib::RefPtr<TreeSelection> sel = toolview.get_selection();
	sel->select(add_tool("Free Hand", starttool));
	add_tool("Draw Line", new DrawLine());
	add_tool("Rectangle", new DrawRect());
	sel->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tool_changed));
}
	
void MainWindow::populate_menus()
{
	MenuActions = ActionGroup::create();
	MenuActions->add(Action::create("MenuFile", "_File"));
	MenuActions->add(Action::create("New", Stock::NEW),
		sigc::mem_fun(*this, &MainWindow::new_image_dialog));
	MenuActions->add(Action::create("Open", Stock::OPEN),
		sigc::mem_fun(*this, &MainWindow::open_image_dialog));
	MenuActions->add(Action::create("Save", Stock::SAVE),
		sigc::mem_fun(*this, &MainWindow::save_image));
	MenuActions->add(Action::create("SaveAs", Stock::SAVE_AS),
		sigc::mem_fun(*this, &MainWindow::save_image_dialog));
	MenuActions->add(Action::create("Close", Stock::CLOSE),
		sigc::mem_fun(*this, &MainWindow::close_document));
	MenuActions->add(Action::create("MenuEdit", "_Edit"));
	MenuActions->add(Action::create("Undo", Stock::UNDO),
		sigc::mem_fun(*this, &MainWindow::undo_current));
	MenuActions->add(Action::create("Redo", Stock::REDO),
		sigc::mem_fun(*this, &MainWindow::redo_current));
	
		
	MenuUI = UIManager::create();
	MenuUI->insert_action_group(MenuActions);
	MenuUI->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "	   <menuitem action='Open'/>"
    "	   <menuitem action='Save'/>"
    "	   <menuitem action='SaveAs'/>"
    "	   <menuitem action='Close'/>"
    "    </menu><menu action='MenuEdit'>					"
    "<menuitem action='Undo' /><menuitem action='Redo' /></menu>"
    "</menubar>"
    "</ui>");
    menubar = MenuUI->get_widget("/MenuBar");
	add_accel_group(MenuUI->get_accel_group());
}

void MainWindow::populate_window()
{
	set_size_request(800, 600);
	set_title("SimpleDraw");
	add(winbox);
	
	toolbox.add(toolview);
	toolview.set_size_request(-1, 200);
	
	tools.add1(toolbox);
	tools.add2(settingbox);
	tools.set_size_request(200, -1);

	
	mainbox.pack_start(tools, false, false);
	mainbox.pack_end(mainarea_layout, true, true);
	mainarea_layout.pack_start(documents, true, true);
	mainarea_layout.pack_end(minibar_layout, false, false);

	gs.color = Gdk::Color("000000");
	gs.scale = 1;
	
	minibar_layout.set_spacing(5);
	minibar_layout.pack_start(scale_label, false, false);
	minibar_layout.pack_start(scale_range, false, false);
	minibar_layout.pack_end(choose_color, false, false);
	
	winbox.pack_start(*menubar, PACK_SHRINK);
	winbox.pack_start(newdoc, PACK_SHRINK);
	winbox.pack_end(mainbox, true, true);
	scale_range.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::scale_activated));
	choose_color.signal_color_set().connect(sigc::mem_fun(*this, &MainWindow::change_color));
	newdoc.new_doc_made.connect(sigc::mem_fun(*this, &MainWindow::new_image_tab));
	scale_range.set_value(1);
	scale_range.set_size_request(200, -1);
	
	menubar->show_all();
	mainbox.show_all();
	winbox.show();
	show();
	
}



MainWindow::MainWindow() : 	mainbox(false,10), toolbox("Tools"),
							settingbox("Tool settings"), scale_label("Scale", ALIGN_LEFT, ALIGN_BOTTOM),
							scale_range(0.5, 4, 0.5), save_dialog(1)
{	
	populate_tools();
	populate_menus();
	populate_window();
}


void MainWindow::scale_activated()
{
	ImageArea* image = get_current_tab();
	
	
	
	gs.scale = scale_range.get_value();
	
	if (image)
	{
		image->clear_window();
		image->update_drawing();
	}
}

void MainWindow::change_color()
{
		Gdk::Color c = choose_color.get_color();
		gs.color = c;
}

void MainWindow::tool_changed()
{
	TreeModel::iterator sel = toolview.get_selection()->get_selected();
	std::cout << (*sel)[toolcol.colname] << " SELECTED\n";
	Tool* t = (*sel)[toolcol.tooldata];
	gs.tool = t;
	settingbox.remove();
	settingbox.add(*(t->get_settings_pane()));
	settingbox.show_all();
}



TreeModel::Row MainWindow::add_tool(Glib::ustring name, Tool* tool)
{
	TreeModel::iterator tooltriter;
	TreeModel::Row toolrow;
		
	tooltriter = toolstore->append();
	toolrow = *tooltriter;
	toolrow[toolcol.colname] = name;
	toolrow[toolcol.tooldata] = tool;
	
	return toolrow;
}

void MainWindow::new_image_tab(int w, int h)
{
	ImageArea* wid = new ImageArea(w,h, &gs, "");
	add_new_tab(wid, "Unsaved Document");
}

void MainWindow::quit()
{
	exit(0);
}

ToolColumn::ToolColumn()
{
	add(colname);
	add(tooldata);
}

ImageArea* MainWindow::get_current_tab()
{
	int page = documents.get_current_page();
	if (page < 0) return NULL;
	return (ImageArea*)(((ScrolledWindow*)documents.get_nth_page(page))->get_child());
}

void MainWindow::set_current_tabstr(Glib::ustring msg)
{
	int page = documents.get_current_page();
	Widget* w = documents.get_nth_page(page);
	if (page < 0) return;
	return ((Label*)documents.get_tab_label(*w))->set_text(msg);
}

int MainWindow::add_new_tab(ImageArea* ia, Glib::ustring tabname)
{
	int i;
	ScrolledWindow* sw = new ScrolledWindow();
	sw->add(*ia);
	i = documents.append_page(*sw, tabname);
	if (i < 0) 
	{
		delete sw;
		return 0;
	}
	
	documents.show_all();
	documents.set_current_page(i);
	return 1;
}

void MainWindow::close_document()
{
	int i = documents.get_current_page();
	MessageDialog changed("YOU WILL GO TO THE DEPTHS OF HELL AND LOSE YOUR SOUL IF YOU DO NOT SAVE", true, MESSAGE_WARNING, BUTTONS_YES_NO);
	if (i >= 0)
	{
		
		ScrolledWindow *sa = (ScrolledWindow*)documents.get_nth_page(i);
		ImageArea *ia = (ImageArea*)sa->get_child();
		if (!ia->change_made || (changed.run() == -8))
		{
			documents.remove(*sa);
			sa->remove();
			delete ia;
			delete sa;
			ia->change_made = 0;
		}
		
	}
}
	

void MainWindow::new_image_dialog()
{
	std::cout << "new image dialog!" << std::endl;
	newdoc.show();
}



NewDocumentBox::NewDocumentBox() :
	wlabel("Width: "), hlabel("Height: "),newbtn("New Document")
{
	pack_start(wlabel, false, false, 20);
	pack_start(wentry, false, false);
	pack_start(hlabel, false, false, 20);
	pack_start(hentry, false, false);
	pack_end(newbtn, false, false);
	
	newbtn.signal_clicked().connect(mem_fun(*this, &NewDocumentBox::rebounce_signal));
	set_border_width(2);
	show_all_children();
}

void NewDocumentBox::rebounce_signal()
{
	int w,h;
	std::cout << "Rebouncing" << std::endl;
	hide();
	if (sscanf(wentry.get_text().c_str(), "%d", &w) && sscanf(hentry.get_text().c_str(), "%d", &h))
	{
		new_doc_made.emit(w,h);
	}
	else
	{
		std::cout << "Newdoc FAIL" << std::endl;
	}
}

void MainWindow::open_image_dialog()
{
	if (open_dialog.run())
	{
		open_image_from_string(open_dialog.get_filename());
	}
	open_dialog.hide();
}

void MainWindow::undo_current()
{
	get_current_tab()->undo();
}

void MainWindow::redo_current()
{
	get_current_tab()->redo();
}

void MainWindow::open_image_from_string(Glib::ustring str)
{
	int loc;
	Glib::ustring fname = str;
	MessageDialog errmsg("Open failed", false, MESSAGE_ERROR);

	if ((loc = fname.rfind('/')) > 0)
	{
		fname = fname.substr(loc+1);
	}
	
	try
	{
		add_new_tab(new ImageArea(str, &gs), fname);
	}
	catch (std::exception& e)
	{
		errmsg.run();
	}
}

void MainWindow::save_image()
{
	ImageArea* image_to_save = get_current_tab();
	
	MessageDialog errmsg("Save failed", false, MESSAGE_ERROR);
	if (image_to_save->fname == "") save_image_dialog();
	else {
		try {
			image_to_save->save_to_png(image_to_save->fname);
		}
		catch (std::exception& e)
		{
			errmsg.run();
		}
	}
}

void MainWindow::save_image_dialog()
{
	int loc;
	ImageArea* image_to_save;
	MessageDialog errmsg("Save failed", false, MESSAGE_ERROR);
	Glib::ustring fname;
	if (save_dialog.run())
	{
		try
		{
			fname = save_dialog.get_filename();
			if ((loc = fname.rfind('/')) > 0)
			{
				fname = fname.substr(loc+1);
			}
			
			image_to_save = get_current_tab();
			if (!image_to_save) throw new std::exception();
			image_to_save->save_to_png(save_dialog.get_filename());
			set_current_tabstr(fname);
		}
		catch (std::exception& e)
		{
			save_dialog.hide();
			errmsg.run();
		}
	}
	save_dialog.hide();
}
	
PngFileChooser::PngFileChooser(int save) : 
	FileChooserDialog((save) ? "Save" : "Open", (save) ? FILE_CHOOSER_ACTION_SAVE : FILE_CHOOSER_ACTION_OPEN)
{
	FileFilter pngfilter;
	FileFilter anyfilter;
	add_button(Stock::CANCEL, 0);
	add_button((save) ? Stock::SAVE : Stock::OPEN, 1);
	
	pngfilter.set_name("PNG Image");
	pngfilter.add_pattern("*.png");
	add_filter(pngfilter);
	anyfilter.set_name("Anything");
	anyfilter.add_pattern("*.*");
	add_filter(anyfilter);
}
	
	
	

int main(int argc, char** argv)
{
	Main app(argc, argv);
	MainWindow mw;
	for (int i = 1; i < argc; i++)
	{
		mw.open_image_from_string(argv[i]);
	}
	app.run(mw);
	return 0;
}
