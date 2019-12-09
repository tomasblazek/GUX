#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdbool.h>
#include <ctype.h>


#define MIN_WIDHT 200
#define MIN_HEIGHT 300
#define MAX_WIDHT 300
#define MAX_HEIGHT 450
#define SIZE_OF_BUFFER 1000

typedef struct Keys {
    GtkWidget *button;
    gchar *label;
    gint gdk_key;
    gint gdk_key2;
    guint x1;
    guint x2;
    guint y1;
    guint y2;
} Key;

// my keys
#define KEY_UNDECLARED (-1)
#define KEY_CLEAR (-2)
#define KEY_NEGATIVE (-1)


#define NUM_ROW_KEY_BASIC 5
#define NUM_COLUMN_KEY_BASIC 4
Key basic_buttons[] = {
        {NULL, "↩",     GDK_KEY_BackSpace,  KEY_UNDECLARED, 0, 1, 0, 1},
        {NULL, "CE",    KEY_CLEAR,          KEY_UNDECLARED, 1, 2, 0, 1},
        {NULL, "(-1)",  KEY_NEGATIVE,       KEY_UNDECLARED, 3, 4, 0, 1},
        {NULL, "1",     GDK_KEY_1,          GDK_KEY_KP_1, 0, 1, 1, 2},
        {NULL, "2",     GDK_KEY_2,          GDK_KEY_KP_2, 1, 2, 1, 2},
        {NULL, "3",     GDK_KEY_3,          GDK_KEY_KP_3, 2, 3, 1, 2},
        {NULL, "+",     GDK_KEY_plus,       GDK_KEY_KP_Add, 3, 4, 1, 2},
        {NULL, "4",     GDK_KEY_4,          GDK_KEY_KP_4, 0, 1, 2, 3},
        {NULL, "5",     GDK_KEY_5,          GDK_KEY_KP_5, 1, 2, 2, 3},
        {NULL, "6",     GDK_KEY_6,          GDK_KEY_KP_6, 2, 3, 2, 3},
        {NULL, "-",     GDK_KEY_minus,      GDK_KEY_KP_Subtract, 3, 4, 2, 3},
        {NULL, "7",     GDK_KEY_7,          GDK_KEY_KP_7, 0, 1, 3, 4},
        {NULL, "8",     GDK_KEY_8,          GDK_KEY_KP_8, 1, 2, 3, 4},
        {NULL, "9",     GDK_KEY_9,          GDK_KEY_KP_9, 2, 3, 3, 4},
        {NULL, "×",     GDK_KEY_asterisk,   GDK_KEY_KP_Multiply, 3, 4, 3, 4},
        {NULL, ",",     GDK_KEY_comma,      GDK_KEY_KP_Decimal, 0, 1, 4, 5},
        {NULL, "0",     GDK_KEY_0,          GDK_KEY_KP_0, 1, 2, 4, 5},
        {NULL, "=",     GDK_KEY_Return,     GDK_KEY_KP_Enter, 2, 3, 4, 5},
        {NULL, "÷",     GDK_KEY_slash,      GDK_KEY_KP_Divide, 3, 4, 4, 5},
};

#define NUM_ROW_KEY_SCIENTIFIC 1
#define NUM_COLUMN_KEY_SCIENTIFIC 4
Key scientific_buttons[] = {
        {NULL, "!", GDK_KEY_BackSpace, KEY_UNDECLARED, 0, 1, 0, 1}, //TODO GDK key
        {NULL, "x²", GDK_KEY_BackSpace, KEY_UNDECLARED, 1, 2, 0, 1},
        {NULL, "√", GDK_KEY_BackSpace, KEY_UNDECLARED, 2, 3, 0, 1},
        {NULL, "log", GDK_KEY_BackSpace, KEY_UNDECLARED, 3, 4, 0, 1},
};


#define NUM_ROW_KEY_PROGRAMMER 1
#define NUM_COLUMN_KEY_PROGRAMMER 4
Key programmer_buttons[] = {
        {NULL, "AND", GDK_KEY_BackSpace, KEY_UNDECLARED, 0, 1, 0, 1}, //TODO GDK key
        {NULL, "OR", GDK_KEY_BackSpace, KEY_UNDECLARED, 1, 2, 0, 1},
        {NULL, "NOT", GDK_KEY_BackSpace, KEY_UNDECLARED, 2, 3, 0, 1},
        {NULL, "XOR", GDK_KEY_BackSpace, KEY_UNDECLARED, 3, 4, 0, 1},
};

gint operators[] = {GDK_KEY_plus, GDK_KEY_KP_Add,
                    GDK_KEY_minus, GDK_KEY_KP_Subtract,
                    GDK_KEY_asterisk, GDK_KEY_KP_Multiply,
                    GDK_KEY_slash, GDK_KEY_KP_Divide};


//enum Mode {BASIC, SCIENTIFIC, PROGRAMMER} mode; TODO delete
enum Calculation {OPERAND1, OPERATOR, OPERAND2};

enum Calculation calculation_state = OPERAND1;
GtkWidget *output_operand1;
GtkWidget *output_operator;
GtkWidget *output_operand2;
GtkWidget *input;


void quitCB(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

GtkWidget* get_gtk_container_by_name(GList *containers, gchar *name){
    GtkWidget *container = NULL;
    while (containers != NULL) {
        if(0 == strcmp(gtk_widget_get_name(GTK_WIDGET(containers->data)), name)) {
            container = GTK_WIDGET(containers->data);
        }
        containers = g_list_next(containers);
    }

    return container;
}


void changeModeBasicCB(GtkWidget *widget, gpointer data) {
}

void changeModeScientificCB(GtkWidget *widget, gpointer data){
    gboolean state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    GList *containers = gtk_container_get_children(GTK_CONTAINER(data));

    GtkWidget *container = get_gtk_container_by_name(containers, "table-scientific-buttons");

    if(state){
        gtk_widget_show(container);
    } else {
        gtk_widget_hide(container);
    }
}

void changeModeProgrammerCB(GtkWidget *widget, gpointer data) {
    gboolean state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    GList *containers = gtk_container_get_children(GTK_CONTAINER(data));

    GtkWidget *container = get_gtk_container_by_name(containers, "table-programmer-buttons");

    if (state) {
        gtk_widget_show(container);
    } else {
        gtk_widget_hide(container);
    }
}


void helpCB(GtkWidget *widget, gpointer data){
    GtkWidget *window = gtk_widget_get_parent(gtk_widget_get_parent(widget));
    const gchar *authors[] = { "Tomáš Blažek (xblaze31)", NULL};

    gtk_show_about_dialog(
            GTK_WINDOW(window),
            "program-name", "Calc",
            "comments", "Project calculator created for course GUX. Implemented by using the gtk+2.0 toolkit.",
            "copyright", "Copyright © 2019 by Tomáš Blažek",
            "authors", authors,
            NULL);
}


void clearCB(GtkWidget *widget, gpointer data){
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(input));
    const gchar *text = gtk_entry_buffer_get_text(buffer);
    gtk_entry_buffer_delete_text(GTK_ENTRY_BUFFER(buffer), 0, (guint) strlen(text));
    gtk_label_set_text(GTK_LABEL(output_operand1), "");
    gtk_label_set_text(GTK_LABEL(output_operator), "");
    gtk_label_set_text(GTK_LABEL(output_operand2), "");
    calculation_state = OPERAND1;
}


bool is_number(const char* str){
    char *end;
    strtod(str, &end);
    if(strcmp(end, "") != 0){
        return false;
    }
    return true;
}

bool is_operator(gint c){
    for (int i = 0; i < sizeof(operators)/sizeof(gint); i++){
        if (c == operators[i] ) {
            return true;
        }
    }

    return false;
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

bool do_math(int operator, double operand1, double operand2, double **result){
    bool fail = false;
    double res = 0;
    switch (operator){
        case '+':
            res = operand1 + operand2; break;
        case '-':
            res = operand1 - operand2; break;
        case '*':
            res = operand1 * operand2; break;
        case '/':
            res = operand1 / operand2; break;
        default:
            fail = true;
            break;
    }

    **result = res;

    return fail;
}


void calc_mashine(Key *key){
    static double operand1;
    static int operator;
    static double operand2;
    double *result = malloc(sizeof(double));
    gchar* result_str;
    if(result == NULL){
        perror("Error: Malloc math result!");
        quitCB(NULL, NULL);
    }

    const gchar *input_str = gtk_entry_get_text(GTK_ENTRY(input));

    printf("State: %d\n", calculation_state);
    printf("Entry input: %s\n", input_str);
    printf("Data input: %c\n", key->gdk_key);

    switch(calculation_state){
        case OPERAND1:
            if (is_operator(key->gdk_key) && strcmp(input_str, "") != 0){
                operator = key->gdk_key;
                operand1 = atof(input_str);
                calculation_state = OPERATOR;
                gtk_label_set_text(GTK_LABEL(output_operand1), input_str);
                gtk_label_set_text(GTK_LABEL(output_operator), key->label);
                gtk_label_set_text(GTK_LABEL(output_operand2), "");
                gtk_entry_set_text(GTK_ENTRY(input), "");
            }
            break;
        case OPERATOR:
            if (is_operator(key->gdk_key)){
                operator = key->gdk_key;
                gtk_label_set_text(GTK_LABEL(output_operator), key->label);
                gtk_entry_set_text(GTK_ENTRY(input), "");
                break;
            }

            if (is_number(input_str)){
                calculation_state = OPERAND2;
            }
            break;
        case OPERAND2:
            if (key->gdk_key == GDK_KEY_Return || key->gdk_key2 == GDK_KEY_KP_Enter){
                operand2 = atof(input_str);
                if (do_math(operator, operand1, operand2, &result)){
                    gtk_label_set_text(GTK_LABEL(output_operand1), "");
                    gtk_label_set_text(GTK_LABEL(output_operator), "Error");
                    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(input));
                    gtk_entry_buffer_delete_text(GTK_ENTRY_BUFFER(buffer), 0, (guint) strlen(input_str));
                    calculation_state = OPERAND1;
                    break;
                }
                float_to_string(*result, &result_str);
                guint len = (guint) strlen(result_str);
                gtk_label_set_text(GTK_LABEL(output_operand2), input_str);
                GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(input));
                gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer), result_str, len);
                g_signal_emit_by_name(input, "move-cursor", GTK_MOVEMENT_LOGICAL_POSITIONS, len, FALSE, NULL);
                calculation_state = OPERAND1;
            }
            break;
        default:
            fprintf(stderr, "Error: State mashine failed!\n");
            free(result);
            quitCB(NULL, NULL);
    }
    free(result);
}

Key* get_button(gint in) {
    size_t num_keys = sizeof(basic_buttons) / sizeof(Key);
    for(int i = 0; i < num_keys; i++){
        if (basic_buttons[i].gdk_key == in || basic_buttons[i].gdk_key2 == in){
            return &basic_buttons[i];
        }
    }
    return NULL;
}





void button_clickedCB(GtkWidget *widget, gpointer data){
    gint in = *(gint *) data;
    gchar *result_str;
    Key *key = get_button(in);
    if (key == NULL){
        return;
    }
    calc_mashine(key);

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(input));
    const gchar *text = gtk_entry_buffer_get_text(buffer);
    guint cursor;
    g_object_get(input, "cursor-position", &cursor, NULL);

    if((in >= GDK_KEY_0 && in <= GDK_KEY_9) || in == GDK_KEY_comma) {
        char append_str[] = {(char) in, '\0'};
        size_t new_text_len = strlen(text) + sizeof(append_str);
        char new_text[new_text_len];
        strncpy(new_text, text, cursor);
        new_text[cursor] = '\0';
        strcat(new_text, append_str);
        strcat(new_text, text + cursor);
        if(!is_number(new_text)){ // check if new entry text will be number
            return;
        }
        guint len = (guint) strlen((*key).label);
        gtk_entry_buffer_insert_text(GTK_ENTRY_BUFFER(buffer), cursor, (*key).label, len); // add symbol to entry
        g_signal_emit_by_name(input, "move-cursor", GTK_MOVEMENT_LOGICAL_POSITIONS, len, FALSE, NULL);
    } else if(in == KEY_NEGATIVE){
        double entry = atof(text) * (-1);
        float_to_string(entry, &result_str);
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer), result_str, (gint) strlen(result_str));
        g_signal_emit_by_name(input, "move-cursor", GTK_MOVEMENT_LOGICAL_POSITIONS, strlen(result_str), FALSE, NULL);
    } else if(in == GDK_KEY_BackSpace){
        if (cursor > 0) {
            double entry = atof(text);
            if (entry <= 0 && cursor == 2) {
                gtk_entry_buffer_delete_text(GTK_ENTRY_BUFFER(buffer), cursor - 2, 2);
            } else {
                gtk_entry_buffer_delete_text(GTK_ENTRY_BUFFER(buffer), cursor - 1, 1);
            }
        }
    } else if(in == KEY_CLEAR){
        clearCB(NULL, NULL);
    }

}


gboolean key_pressedCB(GtkWidget *widget, GdkEventKey *event){
    printf("%d\n", event->keyval);
    gint in = event->keyval;
    if ((event->keyval >= GDK_KEY_KP_0 && event->keyval <= GDK_KEY_KP_9)){
        in = in - GDK_KEY_KP_0 + GDK_KEY_0; //normalization to classic number key
    } else if(event->keyval == GDK_KEY_KP_Decimal){
        in = GDK_KEY_comma;
    }

    button_clickedCB(widget, (gpointer) &in);
    return FALSE;
}


void create_buttons(GtkTable *t, Key *keys, size_t num_keys){
    GtkWidget *b;
    for (int i = 0; i < num_keys; i++){
        b = gtk_button_new_with_mnemonic(keys[i].label);
        keys[i].button = b;
        gtk_table_attach_defaults(t, b,
                                  keys[i].x1,
                                  keys[i].x2,
                                  keys[i].y1,
                                  keys[i].y2);
        gtk_button_set_focus_on_click(GTK_BUTTON(b), FALSE);
        g_signal_connect(b, "clicked", G_CALLBACK(button_clickedCB), &keys[i].gdk_key);
    }
}


void initialize_app() {
    GdkGeometry size_hints;
    size_hints.min_width = MIN_WIDHT;
    size_hints.min_height = MIN_HEIGHT;
    size_hints.max_width = MAX_WIDHT;
    size_hints.max_height = MAX_HEIGHT;

    gint spacing = 5;

    // Window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calc");
    g_signal_connect(window, "destroy", G_CALLBACK(quitCB), NULL);
    g_signal_connect(window, "key_press_event", G_CALLBACK(key_pressedCB), NULL);
    gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &size_hints,
                                  GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE | GDK_HINT_MAX_SIZE);

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
    GtkWidget *key_tables = gtk_vbox_new(FALSE, spacing);

    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "Basic");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeBasicCB), key_tables);

    radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(check_menu_item));
    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "Scientific");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeScientificCB), key_tables);


    radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(check_menu_item));
    check_menu_item = gtk_radio_menu_item_new_with_mnemonic(radio_group, "Programmer");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_mode), check_menu_item);
    g_signal_connect(check_menu_item, "activate", G_CALLBACK(changeModeProgrammerCB), key_tables);


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
    output_operand1 = gtk_label_new("");
    output_operator = gtk_label_new("");
    output_operand2 = gtk_label_new("");

    // Input box
    input = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(input), 1);
    gtk_editable_set_editable(GTK_EDITABLE(input), false);

    // Buttons
    GtkWidget *table_basic_buttons = gtk_table_new(NUM_ROW_KEY_BASIC, NUM_COLUMN_KEY_BASIC, TRUE);
    create_buttons(GTK_TABLE(table_basic_buttons), basic_buttons, sizeof(basic_buttons) / sizeof(Key));
    gtk_widget_set_name(table_basic_buttons, "table-basic-buttons");

    GtkWidget *table_scientific_buttons = gtk_table_new(NUM_ROW_KEY_SCIENTIFIC, NUM_COLUMN_KEY_SCIENTIFIC, TRUE);
    create_buttons(GTK_TABLE(table_scientific_buttons), scientific_buttons, sizeof(scientific_buttons) / sizeof(Key));
    gtk_widget_set_name(table_scientific_buttons, "table-scientific-buttons");

    GtkWidget *table_programmer_buttons = gtk_table_new(NUM_ROW_KEY_PROGRAMMER, NUM_COLUMN_KEY_PROGRAMMER, TRUE);
    create_buttons(GTK_TABLE(table_programmer_buttons), programmer_buttons, sizeof(programmer_buttons) / sizeof(Key));
    gtk_widget_set_name(table_programmer_buttons, "table-programmer-buttons");


    // Container settings
    GtkWidget *main_window_box = gtk_vbox_new(FALSE, spacing);
    GtkWidget *outputs = gtk_hbox_new(FALSE, spacing);

    gtk_container_add(GTK_CONTAINER(outputs), output_operand1);
    gtk_container_add(GTK_CONTAINER(outputs), output_operator);
    gtk_container_add(GTK_CONTAINER(outputs), output_operand2);


    gtk_container_add(GTK_CONTAINER(main_window_box), menu_bar);
    gtk_container_add(GTK_CONTAINER(main_window_box), outputs);
    gtk_container_add(GTK_CONTAINER(main_window_box), input);
    gtk_container_add(GTK_CONTAINER(key_tables), table_scientific_buttons);
    gtk_container_add(GTK_CONTAINER(key_tables), table_programmer_buttons);
    gtk_container_add(GTK_CONTAINER(key_tables), table_basic_buttons);
    gtk_container_add(GTK_CONTAINER(main_window_box), key_tables);
    gtk_container_child_set(GTK_CONTAINER(main_window_box), GTK_WIDGET(menu_bar), "expand", FALSE, NULL);

    gtk_container_add(GTK_CONTAINER(window), main_window_box);

    // Show all
    gtk_widget_show_all(window);

    // Hide extended mode keys
    gtk_widget_hide(table_scientific_buttons);
    gtk_widget_hide(table_programmer_buttons);

}

int main( int argc, char *argv[] )
{

    gtk_init(&argc, &argv);
    initialize_app();

    gtk_main();

    return 0;
}
