#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>


#define WIDHT 200
#define HEIGHT 300


void clearCB(GtkWidget *widget, gpointer data){
    //clear
}


void quitCB(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}


void initialize_app(){
    GdkGeometry size_hints;
    size_hints.min_width = WIDHT;
    size_hints.min_height = HEIGHT;
    gint spacing = 3;

    // Window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calc");
    g_signal_connect(window, "destroy", G_CALLBACK(quitCB), NULL);
    gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &size_hints, GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

    // Menu bar
    GtkWidget *menu_bar = gtk_menu_bar_new();
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    // Menu bar - file
    GtkWidget *menu_file = gtk_menu_new();
    GtkWidget *menu_file_item = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_file_item), menu_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_file_item);

    menu_file_item = gtk_menu_item_new_with_mnemonic("_Clear");
    gtk_widget_add_accelerator(menu_file_item, "activate", accel_group, GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(menu_file_item, "activate", G_CALLBACK(clearCB), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_item);

    menu_file_item = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_widget_add_accelerator(menu_file_item, "activate", accel_group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(menu_file_item, "activate", G_CALLBACK(quitCB), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_item);

    // Container settings
    GtkWidget *main_window_box = gtk_vbox_new(FALSE, spacing);
    gtk_container_add(GTK_CONTAINER(main_window_box), menu_bar);
    gtk_container_child_set(GTK_CONTAINER(main_window_box), GTK_WIDGET(menu_bar), "expand", FALSE, NULL);

    gtk_container_add(GTK_CONTAINER(window), main_window_box);

    // Show all
    gtk_widget_show_all(window);
}


int main( int argc, char *argv[] )
{

    gtk_init(&argc, &argv);
    initialize_app();
    
    gtk_main();
    
    return 0;
}
