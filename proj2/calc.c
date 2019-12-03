#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdbool.h>
#include <ctype.h>


#define WIDHT 200
#define HEIGHT 300
#define SIZE_OF_BUFFER 1000

typedef struct Keys {
    GtkWidget *button;
    gchar *label;
    gchar value;
    guint gdk_key;
    guint x1;
    guint x2;
    guint y1;
    guint y2;
} Key;

#define NUM_ROW_KEY_BASIC 4
#define NUM_COLUMN_KEY_BASIC 3
#define NUM_KEY_BASIC NUM_ROW_KEY_BASIC * NUM_COLUMN_KEY_BASIC
Key basic_buttons[NUM_KEY_BASIC] = {
        {NULL, "_1", '1', GDK_KEY_1, 0, 1, 0, 1},
        {NULL, "_2", '2', GDK_KEY_2, 1, 2, 0, 1},
        {NULL, "_3", '3', GDK_KEY_3, 2, 3, 0, 1},
        //{NULL, "_+", "+", GDK_KEY_plus, 3, 4, 0, 1},
        {NULL, "_4", '4', GDK_KEY_4, 0, 1, 1, 2},
        {NULL, "_5", '5', GDK_KEY_5, 1, 2, 1, 2},
        {NULL, "_6", '6', GDK_KEY_6, 2, 3, 1, 2},
        {NULL, "_7", '7', GDK_KEY_7, 0, 1, 2, 3},
        {NULL, "_8", '8', GDK_KEY_8, 1, 2, 2, 3},
        {NULL, "_9", '9', GDK_KEY_9, 2, 3, 2, 3},
        {NULL, "_↩", '9', GDK_KEY_BackSpace, 0, 1, 3, 4},
        {NULL, "_0", '0', GDK_KEY_0, 1, 2, 3, 4},
        {NULL, "_=", '9', GDK_KEY_equal, 2, 3, 3, 4},
};


int operators[] = {'+', '-', '\0'};


enum Mode {BASIC, SCIENTIFIC, PROGRAMMER};
enum Calculation {OPERAND1, OPERATOR, OPERAND2};

enum Calculation calculation_state = OPERAND1;
GtkWidget *output_operand1;
GtkWidget *output_operator;
GtkWidget *output_operand2;
GtkWidget *input;
char *result;


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
    free(result);
    gtk_main_quit();
}

bool is_number(const char* str){
    char *end;
    strtod(str, &end);
    if(strcmp(end, "") != 0){
        return false;
    }
    return true;
}

bool is_operator(char c){
    for (int i = 0; operators[i] > 0 ; i++){
        if (c == operators[i] ) {
            return true;
        }
    }

    return false;
}


void valid_input_double(){
    const gchar *str = gtk_entry_get_text(GTK_ENTRY(input));
    if (!is_number((char *) str)){
        size_t len = strlen(str);
        char new_str[len];
        strncpy(new_str, str, len-1);
        new_str[len-1] = '\0';
        gtk_entry_set_text(GTK_ENTRY(input), new_str);
        gtk_entry_set_position(GTK_ENTRY(input), (gint) len);
    }
}


int float_to_string(double num, char **str){
    char buffer[SIZE_OF_BUFFER];
    int ret = snprintf(buffer, sizeof buffer, "%.10g", num);

    if (ret < 0) {
        return EXIT_FAILURE;
    }

    *str = buffer;
    return EXIT_SUCCESS;
}

bool do_math(int operator, double operand1, double operand2, char **result){
    bool fail = false;
    double res = 0;
    switch (operator){
        case '+':
            res = operand1 + operand2;
            break;
        case '-':
            res = operand1 - operand2;
            break;
        default:
            break;
    }

    if (!fail){
        char *r;
        float_to_string(res, &r);
        strcpy(*result, r);
    }

    return fail;
}


void key_pressedCB(GtkWidget *widget, GdkEventKey *event, gchar data) {
    static double operand1;
    static char operator;
    static double operand2;
    if (result == NULL) {
        result = malloc(1000 * sizeof(char));
        if (result == NULL){
            perror("Error: Malloc\n");
            quitCB(NULL, NULL);
        }
    }

    printf("State: %d\n", calculation_state);
    bool equals_pressed = false;
    char data_in[2] = {data, '\0'};
    char *in = data_in;
    if (event != NULL){
        in = event->string;
        if (event->keyval == GDK_KEY_Return){
            equals_pressed = true;
        }
    } else {
        const gchar *str = gtk_entry_get_text(GTK_ENTRY(input));
        char buffer[SIZE_OF_BUFFER];
        strcpy(buffer, str);
        size_t len = strlen(buffer);
        if(len + 1 <= SIZE_OF_BUFFER){
            buffer[len] = in[0];
            buffer[len+1] = '\0';
        }
        gtk_entry_set_text(GTK_ENTRY(input), buffer);
    }


    valid_input_double(); // valid input here

    const gchar *str = gtk_entry_get_text(GTK_ENTRY(input));
    printf("Entry input: %s\n", str);
    printf("Data input: %c\n", in[0]);

    switch(calculation_state){
        case OPERAND1:
            if (is_operator(in[0]) && strcmp(str, "")){
                operator = in[0];
                operand1 = atof(str);
                calculation_state = OPERATOR;
                gtk_label_set_text(GTK_LABEL(output_operand1), str);
                gtk_label_set_text(GTK_LABEL(output_operator), in);
                gtk_label_set_text(GTK_LABEL(output_operand2), "");
                gtk_entry_set_text(GTK_ENTRY(input), "");
            }
            break;
        case OPERATOR:
            if (is_operator(in[0])){
                operator = in[0];
                gtk_label_set_text(GTK_LABEL(output_operator), in);
                gtk_entry_set_text(GTK_ENTRY(input), "");
                break;
            }

            if (is_number(str)){
               calculation_state = OPERAND2;
            }
            break;
        case OPERAND2:
            if (equals_pressed){
                operand2 = atof(str);
                do_math(operator, operand1, operand2, &result);
                gtk_label_set_text(GTK_LABEL(output_operand2), str);
                gtk_entry_set_text(GTK_ENTRY(input), result);
                size_t len = strlen(result);
                gtk_entry_set_position(GTK_ENTRY(input), (guint) len);
                calculation_state = OPERAND1;
            }
            break;
        default:
            fprintf(stderr, "Error: State mashine failed!\n");
            quitCB(NULL, NULL);
    }
}


void key_clickedCB(GtkWidget *widget, gpointer data){
    key_pressedCB(widget, NULL, *(gchar *) data);
}

void add_keys(GtkTable *t, GtkAccelGroup *accel_group){
    GtkWidget *b;
    for (int i = 0; i < NUM_KEY_BASIC; i++){
        b = gtk_button_new_with_mnemonic(basic_buttons[i].label);
        basic_buttons[i].button = b;
        gtk_table_attach_defaults(t, b,
                                  basic_buttons[i].x1,
                                  basic_buttons[i].x2,
                                  basic_buttons[i].y1,
                                  basic_buttons[i].y2);
        gtk_widget_add_accelerator(b, "clicked", accel_group, basic_buttons[i].gdk_key, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
        g_signal_connect(b, "clicked", G_CALLBACK(key_clickedCB), &basic_buttons[i].value);
    }

};


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

    // Ouput box
    output_operand1  = gtk_label_new("");
    output_operator  = gtk_label_new("");
    output_operand2  = gtk_label_new("");

    // Input box
    input = gtk_entry_new();
    g_signal_connect(input, "key_release_event", G_CALLBACK(key_pressedCB), NULL);

    // Buttons
    GtkWidget *table_buttons = gtk_table_new(NUM_COLUMN_KEY_BASIC, NUM_ROW_KEY_BASIC, TRUE);
    add_keys(GTK_TABLE(table_buttons), accel_group);



    // Container settings
    GtkWidget *main_window_box = gtk_vbox_new(FALSE, spacing);
    gtk_container_add(GTK_CONTAINER(main_window_box), menu_bar);
    GtkWidget *outputs = gtk_hbox_new(FALSE, spacing);
    gtk_container_add(GTK_CONTAINER(outputs), output_operand1);
    gtk_container_add(GTK_CONTAINER(outputs), output_operator);
    gtk_container_add(GTK_CONTAINER(outputs), output_operand2);

    gtk_container_add(GTK_CONTAINER(main_window_box), outputs);
    gtk_container_add(GTK_CONTAINER(main_window_box), input);
    gtk_container_add(GTK_CONTAINER(main_window_box), table_buttons);
    gtk_container_child_set(GTK_CONTAINER(main_window_box), GTK_WIDGET(menu_bar), "expand", FALSE, NULL);

    gtk_container_add(GTK_CONTAINER(window), main_window_box);

    // Show all
    gtk_widget_show_all(window);

//    gtk_widget_hide(basic_buttons[0].button);
//    gtk_widget_show(basic_buttons[0].button);
}


int main( int argc, char *argv[] )
{

    gtk_init(&argc, &argv);
    initialize_app();

    gtk_main();

    return 0;
}
