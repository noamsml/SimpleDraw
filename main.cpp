#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

MainWindow::MainWindow() : 	mainbox(false,10), toolbox("Tools"),
							settingbox("Tool settings"), randomtestarea(300,300), scale_label("Scale")
{
	set_size_request(800, 600);
	set_title("SimpleDraw");
	add(mainbox);

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
	randomtestarea.change_tool(new FreeHand());

}


void MainWindow::scale_activated()
{
	int scl = atoi(scale_entry.get_text().c_str());
	printf("%d\n", scl);
	randomtestarea.scale = scl;
	randomtestarea.clear_window();
	randomtestarea.update_drawing();
}

void MainWindow::change_color()
{
	Gdk::Color c = choose_color.get_color();
	randomtestarea.set_color(c.get_red_p(), c.get_green_p(), c.get_blue_p());
}

int main(int argc, char** argv)
{
	Main app(argc, argv);
	MainWindow mw;
	mw.show_all();
	app.run(mw);
	return 0;
}
