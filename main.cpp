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
	
	MenuUI = UIManager::create();
	MenuUI->insert_action_group(MenuActions);
	MenuUI->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <menuitem action='New'/>"
    "    </menu></menubar>"
    "</ui>");
    menubar = MenuUI->get_widget("/MenuBar");
	
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
	documents.append_page(*(new ImageArea(300, 300)), "Hello world");
	documents.append_page(*(new ImageArea(100, 300)), "Hello world2");
	mainarea_layout.pack_start(documents, true, true);
	mainarea_layout.pack_end(minibar_layout, false, false);

	minibar_layout.pack_start(scale_label, false, false);
	minibar_layout.pack_start(scale_entry, false, false);
	minibar_layout.pack_end(choose_color, false, false);
	winbox.pack_start(*menubar, PACK_SHRINK);
	winbox.pack_start(newdoc, PACK_SHRINK);
	winbox.pack_end(mainbox, true, true);
	scale_entry.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::scale_activated));
	choose_color.signal_color_set().connect(sigc::mem_fun(*this, &MainWindow::change_color));
	newdoc.new_doc_made.connect(sigc::mem_fun(*this, &MainWindow::new_image_tab));
	
}



MainWindow::MainWindow() : 	mainbox(false,10), toolbox("Tools"),
							settingbox("Tool settings"), scale_label("Scale")
{	
	populate_tools();
	populate_menus();
	populate_window();
	
}


void MainWindow::scale_activated()
{
	float scl;
	ImageArea* image = get_current_tab();
	
	int conv = sscanf(scale_entry.get_text().c_str(), "%f", &scl);
	if (!conv) 
	{
		scale_entry.set_text("1");
		scl = 1;
	}
	if (image)
	{
		image->scale = scl;
		image->clear_window();
		image->update_drawing();
	}
}

void MainWindow::change_color()
{
	ImageArea* image = get_current_tab();
	if (image)
	{
		Gdk::Color c = choose_color.get_color();
		image->set_color(c.get_red_p(), c.get_green_p(), c.get_blue_p());
	}
}

void MainWindow::tool_changed()
{
	ImageArea* image = get_current_tab();
	if (image)
	{
		TreeModel::iterator sel = toolview.get_selection()->get_selected();
		std::cout << (*sel)[toolcol.colname] << " SELECTED\n";
		Tool* t = (*sel)[toolcol.tooldata];
		image->change_tool(t);
	}
}

void MainWindow::change_all_data()
{
	change_color();
	tool_changed();
	scale_activated(); //Also draws the image. Powah!
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
	//TOFIX: no reference-counting on this pointer
	std::cout << "Newdoc" << std::endl;
	documents.append_page(*(new ImageArea(w,h)), "New Document");
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
}

void NewDocumentBox::rebounce_signal()
{
	int w,h;
	std::cout << "Rebouncing" << std::endl;
	if (sscanf(wentry.get_text().c_str(), "%d", &w) && sscanf(hentry.get_text().c_str(), "%d", &h))
	{
		new_doc_made.emit(w,h);
	}
	else
	{
		std::cout << "Newdoc FAIL" << std::endl;
	}
}

int main(int argc, char** argv)
{
	Main app(argc, argv);
	MainWindow mw;
	mw.show_all();
	app.run(mw);
	return 0;
}
