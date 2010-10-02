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
	
	MenuUI = UIManager::create();
	MenuUI->insert_action_group(MenuActions);
	MenuUI->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "	   <menuitem action='Open'/>"
    "    </menu></menubar>"
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
							scale_range(0.5, 4, 0.5), open_dialog("Open a PNG Image")
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
	int i;
	std::cout << w << " :: " << h << std::endl;
	ImageArea* wid = new ImageArea(w,h, &gs, "");
	//TOFIX: no reference-counting on this pointer (?)
	std::cout << "Newdoc" << std::endl;
	std::cout << (i = documents.append_page(*wid, "New Document")) << std::endl;
	documents.show_all();
	documents.set_current_page(i);
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
	return (ImageArea*)documents.get_nth_page(page);
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
	int i;
	open_dialog.run();
	i = documents.append_page(*(new ImageArea(open_dialog.get_filename(), &gs)), "Openened File");
	documents.set_current_page(i);
	documents.show_all();
}
	

int main(int argc, char** argv)
{
	Main app(argc, argv);
	MainWindow mw;
	app.run(mw);
	return 0;
}
