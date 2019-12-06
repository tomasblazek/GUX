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
    gint gdk_key;
    guint x1;
    guint x2;
    guint y1;
    guint y2;
} Key;

// my keys
#define KEY_CLEAR (-1)
#define KEY_NEGATIVE (-2)


#define NUM_ROW_KEY_BASIC 5
#define NUM_COLUMN_KEY_BASIC 4
Key basic_buttons[] = {
        {NULL, "↩",     GDK_KEY_BackSpace,  0, 1, 0, 1},
        {NULL, "CE",    KEY_CLEAR,          1, 2, 0, 1},
        {NULL, "(-1)",  KEY_NEGATIVE,       3, 4, 0, 1},
        {NULL, "1",     GDK_KEY_1,          0, 1, 1, 2},
        {NULL, "2",     GDK_KEY_2,          1, 2, 1, 2},
        {NULL, "3",     GDK_KEY_3,          2, 3, 1, 2},
        {NULL, "+",     GDK_KEY_plus,       3, 4, 1, 2},
        {NULL, "4",     GDK_KEY_4,          0, 1, 2, 3},
        {NULL, "5",     GDK_KEY_5,          1, 2, 2, 3},
        {NULL, "6",     GDK_KEY_6,          2, 3, 2, 3},
        {NULL, "-",     GDK_KEY_minus,      3, 4, 2, 3},
        {NULL, "7",     GDK_KEY_7,          0, 1, 3, 4},
        {NULL, "8",     GDK_KEY_8,          1, 2, 3, 4},
        {NULL, "9",     GDK_KEY_9,          2, 3, 3, 4},
        {NULL, "×",     GDK_KEY_asterisk,   3, 4, 3, 4},
        {NULL, ",",     GDK_KEY_comma,      0, 1, 4, 5},
        {NULL, "0",     GDK_KEY_0,          1, 2, 4, 5},
        {NULL, "=",     GDK_KEY_equal,      2, 3, 4, 5},
        {NULL, "÷",     GDK_KEY_slash,      3, 4, 4, 5},
};

#define NUM_ROW_KEY_SCIENTIFIC 1
#define NUM_COLUMN_KEY_SCIENTIFIC 4
Key scientific_buttons[] = {
        {NULL, "!", GDK_KEY_BackSpace, 0, 1, 0, 1}, //TODO GDK key
        {NULL, "x²", GDK_KEY_BackSpace, 1, 2, 0, 1},
        {NULL, "√", GDK_KEY_BackSpace, 2, 3, 0, 1},
        {NULL, "log", GDK_KEY_BackSpace, 3, 4, 0, 1},
};


#define NUM_ROW_KEY_PROGRAMMER 1
#define NUM_COLUMN_KEY_PROGRAMMER 4
Key programmer_buttons[] = {
        {NULL, "AND", GDK_KEY_BackSpace, 0, 1, 0, 1}, //TODO GDK key
        {NULL, "OR", GDK_KEY_BackSpace, 1, 2, 0, 1},
        {NULL, "NOT", GDK_KEY_BackSpace, 2, 3, 0, 1},
        {NULL, "XOR", GDK_KEY_BackSpace, 3, 4, 0, 1},
};

int operators[] = {'/', '*','+','-', '\0'};


enum Mode {BASIC, SCIENTIFIC, PROGRAMMER};
enum Calculation {OPERAND1, OPERATOR, OPERAND2};

enum Calculation calculation_state = OPERAND1;
GtkWidget *output_operand1;
GtkWidget *output_operator;
GtkWidget *output_operand2;
GtkWidget *input;
char *result_str = NULL;


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

bool is_number(const char* str){
    char *end;
    strtod(str, &end);
    if(strcmp(end, "") != 0){
        return false;
    }
    return true;
}

bool is_operator(int c){
    for (int i = 0; operators[i] > 0 ; i++){
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

    if (!fail){
        *result = &res;
    }

    return fail;
}


void calc_mashine(Key *key){
    static double operand1;
    static int operator;
    static double operand2;
    double *result;

    gint in = key->gdk_key;
    gchar* label = key->label;
    const gchar *input_str = gtk_entry_get_text(GTK_ENTRY(input));

    printf("State: %d\n", calculation_state);
    printf("Entry input: %s\n", input_str);
    printf("Data input: %c\n", in);

    switch(calculation_state){
        case OPERAND1:
            if (is_operator(in) && strcmp(input_str, "") != 0){
                operator = in;
                operand1 = atof(input_str);
                calculation_state = OPERATOR;
                gtk_label_set_text(GTK_LABEL(output_operand1), input_str);
                gtk_label_set_text(GTK_LABEL(output_operator), key->label);
                gtk_label_set_text(GTK_LABEL(output_operand2), "");
                gtk_entry_set_text(GTK_ENTRY(input), "");
            }
            break;
        case OPERATOR:
            if (is_operator(in)){
                operator = in;
                gtk_label_set_text(GTK_LABEL(output_operator), label);
                gtk_entry_set_text(GTK_ENTRY(input), "");
                break;
            }

            if (is_number(input_str)){
                calculation_state = OPERAND2;
            }
            break;
        case OPERAND2:
            if (key->gdk_key == '='){
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
            quitCB(NULL, NULL);
    }
}

Key* get_button(gint in) {
    size_t num_keys = sizeof(basic_buttons) / sizeof(Key);
    for(int i = 0; i < num_keys; i++){
        if (basic_buttons[i].gdk_key == in){
            return &basic_buttons[i];
        }
    }
    return NULL;
}



void button_clickedCB(GtkWidget *widget, gpointer data){
    gint in = *(gint *) data;

    Key *key = get_button(in);
    if (key == NULL){
        return;
    }
    calc_mashine(key);

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(input));
    const gchar *text = gtk_entry_buffer_get_text(buffer);
    guint cursor;
    g_object_get(input, "cursor-position", &cursor, NULL);

    bool a = is_number("2-2");

    if(in >= GDK_KEY_0 && in <= GDK_KEY_9 || in == GDK_KEY_comma) {
        // check if new entry text will be number
        char append_str[] = {(char) in, '\0'};
        size_t new_text_len = strlen(text) + sizeof(append_str);
        char new_text[new_text_len];
        strncpy(new_text, text, cursor);
        new_text[cursor] = '\0';
        strcat(new_text, append_str);
        strcat(new_text, text + cursor);
        if(!is_number(new_text)){
          return;
        }
        // add symbol to entry
        guint len = (guint) strlen((*key).label);
        gtk_entry_buffer_insert_text(GTK_ENTRY_BUFFER(buffer), cursor, (*key).label, len);
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
        gtk_entry_buffer_delete_text(GTK_ENTRY_BUFFER(buffer), 0, (guint) strlen(text));
        gtk_label_set_text(GTK_LABEL(output_operand1), "");
        gtk_label_set_text(GTK_LABEL(output_operator), "");
        gtk_label_set_text(GTK_LABEL(output_operand2), "");
        calculation_state = OPERAND1;
    }

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
        g_signal_connect(b, "clicked", G_CALLBACK(button_clickedCB), &keys[i].gdk_key);
    }


};


void allocate_resources(){
    result_str = calloc(0, SIZE_OF_BUFFER * sizeof(char));
    if (result_str == NULL){
        perror("Error: Calloc\n");
        exit(EXIT_FAILURE);
    }
}

void initialize_app(){
    GdkGeometry size_hints;
    size_hints.min_width = WIDHT;
    size_hints.min_height = HEIGHT;
    gint spacing = 5;

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
    gtk_entry_set_editable(GTK_ENTRY(input), false);
    gtk_entry_set_alignment (GTK_ENTRY(input), 1);
    //g_signal_connect(input, "key_release_event", G_CALLBACK(key_pressedCB), NULL);

    // Buttons
    GtkWidget *table_basic_buttons = gtk_table_new(NUM_ROW_KEY_BASIC, NUM_COLUMN_KEY_BASIC, TRUE);
    create_buttons(GTK_TABLE(table_basic_buttons), basic_buttons, sizeof(basic_buttons) / sizeof(Key));

    GtkWidget *table_scientific_buttons = gtk_table_new(NUM_ROW_KEY_SCIENTIFIC, NUM_COLUMN_KEY_SCIENTIFIC, TRUE);
    create_buttons(GTK_TABLE(table_scientific_buttons), scientific_buttons, sizeof(scientific_buttons) / sizeof(Key));

    GtkWidget *table_programmer_buttons = gtk_table_new(NUM_ROW_KEY_PROGRAMMER, NUM_COLUMN_KEY_PROGRAMMER, TRUE);
    create_buttons(GTK_TABLE(table_programmer_buttons), programmer_buttons, sizeof(programmer_buttons) / sizeof(Key));

    // Container settings
    GtkWidget *main_window_box = gtk_vbox_new(FALSE, spacing);
    GtkWidget *outputs = gtk_hbox_new(FALSE, spacing);

    gtk_container_add(GTK_CONTAINER(outputs), output_operand1);
    gtk_container_add(GTK_CONTAINER(outputs), output_operator);
    gtk_container_add(GTK_CONTAINER(outputs), output_operand2);


    gtk_container_add(GTK_CONTAINER(main_window_box), menu_bar);
    gtk_container_add(GTK_CONTAINER(main_window_box), outputs);
    gtk_container_add(GTK_CONTAINER(main_window_box), input);
    gtk_container_add(GTK_CONTAINER(main_window_box), table_scientific_buttons);
    gtk_container_add(GTK_CONTAINER(main_window_box), table_programmer_buttons);
    gtk_container_add(GTK_CONTAINER(main_window_box), table_basic_buttons);
    gtk_container_child_set(GTK_CONTAINER(main_window_box), GTK_WIDGET(menu_bar), "expand", FALSE, NULL);

    gtk_container_add(GTK_CONTAINER(window), main_window_box);

    // Show all
    gtk_widget_show_all(window);

    allocate_resources();
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
