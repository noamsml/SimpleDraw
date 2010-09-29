#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>


MainWindow::MainWindow() : 	mainbox(false,10), toolbox("Tools"),
							settingbox("Tool settings"), randomtestarea(300,300), scale_label("Scale")
{
	
	
	set_size_request(800, 600);
	set_title("SimpleDraw");
	add(mainbox);
	
	toolstore = ListStore::create(toolcol);
	toolview.set_model(toolstore);
	toolview.append_column("Name", toolcol.colname);
	
	
	FreeHand* starttool = new FreeHand();
	randomtestarea.change_tool(starttool);
	
	Glib::RefPtr<TreeSelection> sel = toolview.get_selection();
	sel->select(add_tool("Free Hand", starttool));
	add_tool("Draw Line", new DrawLine());
	sel->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::tool_changed));
	
	
	toolbox.add(toolview);
	toolview.set_size_request(-1, 200);
	
	tools.add1(toolbox);
	tools.add2(settingbox);
	tools.set_size_request(200, -1);

	mainbox.pack_start(tools, false, false);
	mainbox.pack_end(mainarea_layout, true, true);
	mainarea_layout.pack_start(documents, true, true);
	mainarea_layout.pack_end(minibar_layout, false, false);

	minibar_layout.pack_start(scale_label, false, false);
	minibar_layout.pack_start(scale_entry, false, false);
	minibar_layout.pack_end(choose_color, false, false);

	scale_entry.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::scale_activated));
	choose_color.signal_color_set().connect(sigc::mem_fun(*this, &MainWindow::change_color));

	documents.append_page(randomtestarea, "test");
	
}


void MainWindow::scale_activated()
{
	float scl;
	int conv = sscanf(scale_entry.get_text().c_str(), "%f", &scl);
	if (!conv) 
	{
		scale_entry.set_text("1");
		scl = 1;
	}
	randomtestarea.scale = scl;
	randomtestarea.clear_window();
	randomtestarea.update_drawing();
}

void MainWindow::change_color()
{
	Gdk::Color c = choose_color.get_color();
	randomtestarea.set_color(c.get_red_p(), c.get_green_p(), c.get_blue_p());
}

void MainWindow::tool_changed()
{
	TreeModel::iterator sel = toolview.get_selection()->get_selected();
	std::cout << (*sel)[toolcol.colname] << " SELECTED\n";
	Tool* t = (*sel)[toolcol.tooldata];
	randomtestarea.change_tool(t);
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

ToolColumn::ToolColumn()
{
	add(colname);
	add(tooldata);
}

int main(int argc, char** argv)
{
	Main app(argc, argv);
	MainWindow mw;
	mw.show_all();
	app.run(mw);
	return 0;
}
