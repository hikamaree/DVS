#include "ui.h"

void show_notification(AppData *data, const char *message) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Custom Popup", GTK_WINDOW(data->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_OK, NULL);
    GtkWidget *label = gtk_label_new(message);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void select_device(GtkWidget* widget, gpointer d) {
	int data = GPOINTER_TO_INT(d);
	device = data;
}

void refresh(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	if(data->visualizer != NULL) {
		start_stop(NULL, data);
	}

	refresh_devices(data->stream);

	GList *children = gtk_container_get_children(GTK_CONTAINER(data->devices));
	for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
		GtkWidget *menuItem = GTK_WIDGET(iter->data);
		gtk_container_remove(GTK_CONTAINER(data->devices), menuItem);
		g_object_unref(menuItem);
	}
	g_list_free(children);

	int cnt = Pa_GetDeviceCount();
	const PaDeviceInfo* device_info;
	for (int device = 0; device < cnt; device++) {
		device_info = Pa_GetDeviceInfo(device);
		if(device_info->maxInputChannels > 0 && device_info->maxInputChannels < 8) {
			GtkWidget *menuItem = gtk_menu_item_new_with_label(device_info->name);
			g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(select_device), GINT_TO_POINTER(device));
			gtk_menu_shell_append(GTK_MENU_SHELL(data->devices), menuItem);
			gtk_widget_show(menuItem);
		}
	}
	device = -1;
}

void set_radius(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->radius = gtk_range_get_value(GTK_RANGE(widget));
}

void set_space(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->space = gtk_range_get_value(GTK_RANGE(widget));
}

void set_speed(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->stream->speed = gtk_range_get_value(GTK_RANGE(widget));
}

void create_window(AppData* data) {
	data->visualizer = NULL;
	data->devices = gtk_menu_new();
	data->radius = 100;
	data->space = 200;
	data->stream->speed = 400;

	data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 400);
	g_signal_connect(G_OBJECT(data->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	data->header = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(data->header), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(data->header), "Directional Audio Visualizer");
	gtk_window_set_titlebar(GTK_WINDOW(data->window), data->header);

	GtkWidget *menubar = gtk_menu_bar_new();
	GtkWidget *file_menu_item = gtk_menu_item_new_with_label("Device");
	refresh(NULL, data);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), data->devices);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_menu_item);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), menubar);

	data->start_stop = gtk_button_new_with_label("Start");
	GtkWidget *button2 = gtk_button_new_with_label("Refresh");
	g_signal_connect(G_OBJECT(data->start_stop), "clicked", G_CALLBACK(start_stop), data);
	g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(refresh), data);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), data->start_stop);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), button2);

	data->grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(data->grid), TRUE);

	data->device_name = gtk_label_new("");

	GtkWidget *radius = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(radius), "value-changed", G_CALLBACK(set_radius), data);
	gtk_range_set_value(GTK_RANGE(radius), data->radius);

	GtkWidget *space = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(space), "value-changed", G_CALLBACK(set_space), data);
	gtk_range_set_value(GTK_RANGE(space), data->space);

	GtkWidget *speed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(speed), "value-changed", G_CALLBACK(set_speed), data);
	gtk_range_set_value(GTK_RANGE(speed), data->stream->speed);

	gtk_grid_attach(GTK_GRID(data->grid), data->device_name, 0, 0, 6, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Radius"), 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), radius, 1, 1, 5, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Distance"), 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), space, 1, 2, 5, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Speed"), 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), speed, 1, 3, 5, 1);

	gtk_container_add (GTK_CONTAINER (data->window), data->grid);

	gtk_widget_show_all(data->window);
}
