#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdbool.h>


#define WIDHT 200
#define HEIGHT 300

void changeModeCB(GtkWidget *widget, gpointer data){
    gboolean t = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    if (t){
        g_print("Mode");
    }
}


void helpCB(GtkWidget *widget, gpointer data){
    g_print("Help");
}


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


    GtkWidget *menu_item;
    // Menu bar - file
    GtkWidget *menu_file = gtk_menu_new();
    menu_item = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

    menu_item = gtk_menu_item_new_with_mnemonic("_Clear");
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(menu_item, "activate", G_CALLBACK(clearCB), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item);

    menu_item = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(menu_item, "activate", G_CALLBACK(quitCB), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item);

    // Menu bar - mode
    GtkWidget *menu_mode = gtk_menu_new();
    menu_item = gtk_menu_item_new_with_mnemonic("_Mode");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu_mode);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

    GtkWidget *check_menu_item;
    GSList *radio_group = NULL;
    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "_Basic");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeCB), NULL);

    radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(check_menu_item));
    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "_Scientific");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeCB), NULL);


    radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(check_menu_item));
    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "_Programmer");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeCB), NULL);


    //Menu bar - help
    GtkWidget *menu_about = gtk_menu_new();
    menu_item = gtk_menu_item_new_with_mnemonic("_About");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu_about);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

    menu_item = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_KEY_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(menu_item, "activate", G_CALLBACK(helpCB), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_about), menu_item);


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
