/*
 * xdraw.c - Base for 1. project
 */

/*
 * Standard XToolkit and OSF/Motif include files.
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h> 

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/MainW.h> 
#include <Xm/Form.h> 
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h> /* OSF/Motif message box widget */


/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Shared variables
 */

#define LINES_ALLOC_STEP	10	/* memory allocation stepping */
#define MIN_WIDTH 735   /* window width */
#define MIN_HEIGHT 400   /* window height */
#define NUM_COLORS 5

enum Shape {Dot, Line, Rectangle, Elipse} c_shape = Line; /* current shape */
String names[NUM_COLORS] = {"Black", "White", "Red", "Green", "Blue"};
Pixel colors[NUM_COLORS];

Pixel cbg_color; /* current color of line */
Pixel cfg_color;
Pixel cfill_color;
unsigned c_thickness = 0; /* current thickness of line */
int c_style = LineSolid; /* current style of line */
Boolean filled = False;

typedef struct PictureElement{
    enum Shape shape;
    unsigned int thickness;
    int style;
    Pixel fgColor;
    Pixel bgColor;
    Pixel fillColor;
    int x1;
    int y1;
    int x2;
    int y2;
} PictureElement;

PictureElement *objects; /* array of objects */
int maxObjects = 0;		/* space allocated for max objects */
int n_object = 0;			/* current number of objects */

GC drawGC = 0;			/* GC used for final drawing */
Widget drawArea;
Widget quitDialog;		    /* Display dialog on application close*/
Widget helpDialog;

int x1, y1, x2, y2;		/* input points */ 
int button_pressed = 0;		/* input state */


void swap(int *a, int *b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}


void DrawObject(Widget w, GC gc, int x1, int y1, int x2, int y2, enum Shape shape, unsigned int thickness, int style,
                Pixel fgColor, Pixel bgColor, Pixel fillColor) {

    Display *display = XtDisplay(w);
    Window window = XtWindow(w);
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    XSetForeground(display, gc, fgColor);
    XSetBackground(display, gc, bgColor);

    XSetLineAttributes(display, gc, thickness, style, 0, 0);
    switch(shape){
        case Dot:
            if (thickness == 0){
                XDrawPoint(display, window, gc, x1, y1);
            } else {
                XDrawArc(display, window, gc, x1, y1, thickness, thickness, 0, 360*64);
            }
            break;
		case Line:
			XDrawLine(display, window, gc, x1, y1, x2, y2);
			break;
		case Rectangle:
            if (x1 > x2) {
                swap(&x1, &x2);
            }
            if (y1 > y2) {
                swap(&y1, &y2);
            }
            unsigned int width = (unsigned int) x2 - x1; // always positive because of swap
            unsigned int height = (unsigned int) y2 - y1;
            if (filled) {
                XSetForeground(display, gc, fillColor);
                XFillRectangle(display, window, gc, x1, y1, width, height);
                XSetForeground(display, gc, fgColor);
            }
			XDrawRectangle(display, window, gc, x1, y1, width, height);
			break;
        case Elipse:
            if (filled) {
                XSetForeground(display, gc, fillColor);
                XFillArc(display, window, gc, x1-dx, y1-dy, 2*(unsigned)dx, 2*(unsigned)dy, 0, 360*64);
                XSetForeground(display, gc, fgColor);
            }
            XDrawArc(display, window, gc, x1-dx, y1-dy, 2*(unsigned)dx, 2*(unsigned)dy, 0, 360*64);
            break;
		default:
			break;
    }	
}


/*
 * "InputLine" event handler
 */
/* ARGSUSED */
void InputObjectEH(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
    Pixel	fg, bg;
    static GC inputGC = 0;			/* GC used for drawing current position */

    if (button_pressed) {
		if (!inputGC) {
		    inputGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
		    XSetFunction(XtDisplay(w), inputGC, GXinvert);
		    XSetPlaneMask(XtDisplay(w), inputGC, ~0);
		    XtVaGetValues(w, XmNforeground, &fg, XmNbackground, &bg, NULL);
		    //XSetForeground(XtDisplay(w), inputGC, bg ^ fg);
        }

		if (button_pressed > 1) {
		    /* erase previous position */
            DrawObject(w, inputGC, x1, y1, x2, y2, c_shape, c_thickness, c_style, cfg_color, cbg_color, cfill_color);
		} else {
		    /* remember first MotionNotify */
		    button_pressed = 2;
		}

		x2 = event->xmotion.x;
		y2 = event->xmotion.y;

        //XSetFunction(XtDisplay(w), inputGC, GXcopy);
        DrawObject(w, inputGC, x1, y1, x2, y2, c_shape, c_thickness, c_style, cfg_color, cbg_color, cfill_color);
    }
}


/*
 * "DrawLine" callback function
 */
void DrawObjectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg al[4];
    int ac;
    XGCValues v;
    XmDrawingAreaCallbackStruct *d = (XmDrawingAreaCallbackStruct*) call_data;

    switch (d->event->type) {
        case ButtonPress:
            if (d->event->xbutton.button == Button1) {
            button_pressed = 1;
            x1 = d->event->xbutton.x;
            y1 = d->event->xbutton.y;
            }
            break;

        case ButtonRelease:
            if (d->event->xbutton.button == Button1) {
                if (++n_object > maxObjects) {
                    maxObjects += LINES_ALLOC_STEP;
                    objects = (PictureElement *) realloc(objects,
                      (sizeof(PictureElement) * maxObjects));
                    if(objects == NULL){
                        perror("Error: Picture elements realloc error!");
                        exit(1);
                    }
                }

                objects[n_object - 1].shape = c_shape;
                objects[n_object - 1].thickness = c_thickness;
                objects[n_object - 1].style = c_style;
                objects[n_object - 1].fgColor = cfg_color;
                objects[n_object - 1].bgColor = cbg_color;
                objects[n_object - 1].fillColor = cfill_color;

                objects[n_object - 1].x1 = x1;
                objects[n_object - 1].y1 = y1;
                objects[n_object - 1].x2 = d->event->xbutton.x;
                objects[n_object - 1].y2 = d->event->xbutton.y;

                button_pressed = 0;

                if (!drawGC) {
                    ac = 0;
                    XtSetArg(al[ac], XmNforeground, &v.foreground); ac++;
                    XtGetValues(w, al, ac);
                    drawGC = XCreateGC(XtDisplay(w), XtWindow(w),
                    GCForeground, &v);
                }

                DrawObject(w, drawGC, x1, y1, objects[n_object - 1].x2, objects[n_object - 1].y2,
                           c_shape, c_thickness, c_style, cfg_color, cbg_color, cfill_color);
            }

            break;
        }
}

/*
 * "Expose" callback function
 */
/* ARGSUSED */
void ExposeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (n_object <= 0){
        return;
    }

    if (!drawGC){
        drawGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
    }

    for (int i = 0; i < n_object; i++){
        DrawObject(w, drawGC, objects[i].x1, objects[i].y1, objects[i].x2, objects[i].y2,
                   objects[i].shape, objects[i].thickness, objects[i].style, objects[i].fgColor,
                   objects[i].bgColor, objects[i].fillColor);
    }
}

void AboutMenuCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtManageChild(helpDialog);
}

/*
 * "Clear" button callback function
 */
/* ARGSUSED */
void ClearCB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
    Widget wcd = (Widget) client_data;

    n_object = 0;
    XClearWindow(XtDisplay(wcd), XtWindow(wcd));
}


void QuitDialogCB(Widget w, XtPointer client_data, XtPointer call_data){
    XtManageChild(quitDialog);
}

/*
 * "Quit" button callback function
 */
/* ARGSUSED */
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data) {
    if (objects != NULL) {
        free(objects);
    }
    exit(0);
}

void OptionMenuCB(Widget w, XtPointer client_data, XtPointer call_data){
    if (client_data == 0){
        ClearCB(w, drawArea, call_data);
    } else {
        QuitDialogCB(w, client_data, call_data);
    }
}

void thicknessRadioCB(Widget w, XtPointer client_data, XtPointer call_data){
    intptr_t thickness = (intptr_t)  client_data;

    switch(thickness){
        case 0:
            c_thickness = 0;
            break;
        case 1:
            c_thickness = 3;
            break;
        case 2:
            c_thickness = 8;
        default:
            break;
    }
}


void shapeRadioCB(Widget w, XtPointer client_data, XtPointer call_data){
    intptr_t select = (intptr_t) client_data;

    switch(select){
        case 0:
            c_shape = Dot;
            break;
        case 1:
            c_shape = Line;
            break;
        case 2:
            c_shape = Rectangle;
            break;
        case 3:
            c_shape = Elipse;
            break;
        default:
            break;
    }
}


void initColors(Widget w, Pixel *color, int nColors){
    char *cName;
    XColor xColor, spare;

    Colormap cmap = DefaultColormapOfScreen(XtScreen(w));

    for (int i = 0; i < nColors; i++) {
        cName = names[i];
        if (XAllocNamedColor(XtDisplay(w), cmap, cName, &xColor, &spare) == 0) {
            fprintf(stderr, "Error: Cannot allocate colormap entry"
                    " for \"%s\"\n", cName);
            exit(1);
        }
        colors[i] = xColor.pixel;
    }

    cbg_color = colors[1];
    cfg_color = colors[0];
}

void fillRadioCB (Widget w, XtPointer client_data, XtPointer call_data){
    intptr_t select = (intptr_t) client_data;

    if (select == 0){
        filled = False;
    } else {
        filled = True;
    }
}

void lineStyleRadioCB (Widget w, XtPointer client_data, XtPointer call_data){
    intptr_t select = (intptr_t) client_data;

    if (select == 0){
        c_style = LineSolid;
    } else {
        c_style = LineDoubleDash;
    }
}

void fgColorRadioCB (Widget w, XtPointer client_data, XtPointer call_data) {
    intptr_t select = (intptr_t) client_data;

    cfg_color = colors[select];
}

void bgColorRadioCB (Widget w, XtPointer client_data, XtPointer call_data) {
    intptr_t select = (intptr_t) client_data;

    cbg_color = colors[select];
}


void fillColorRadioCB (Widget w, XtPointer client_data, XtPointer call_data) {
    intptr_t select = (intptr_t) client_data;

    cfill_color = colors[select];
}


void initApp(XtAppContext *app_context, int argc, char* argv[]){
    Widget topLevel, mainWin, frame, rowColumn;

    topLevel = XtVaAppInitialize(
            app_context,		 	/* Application context */
            "Draw",				/* Application class */
            NULL, 0,				/* command line option list */
            &argc, argv,
            NULL,/* command line args */
            XmNdeleteResponse, XmDO_NOTHING,    /* for missing app-defaults file */
            XmNminWidth, MIN_WIDTH,
            XmNminHeight, MIN_HEIGHT,
            NULL);				/* terminate varargs list */

    Atom windowExit = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(topLevel, windowExit, QuitDialogCB, NULL);
    XmActivateWMProtocol(topLevel, windowExit);

    mainWin = XtVaCreateManagedWidget(
            "mainWin",			/* widget name */
            xmMainWindowWidgetClass,		/* widget class */
            topLevel,				/* parent widget*/
            XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
            NULL);				/* terminate varargs list */

    frame = XtVaCreateManagedWidget(
            "frame",				/* widget name */
            xmFrameWidgetClass,		/* widget class */
            mainWin,				/* parent widget */
            NULL);				/* terminate varargs list */

    drawArea = XtVaCreateManagedWidget(
            "drawingArea",			/* widget name */
            xmDrawingAreaWidgetClass,		/* widget class */
            frame,				/* parent widget*/
            NULL);				/* terminate varargs list */

/*
    XSelectInput(XtDisplay(drawArea), XtWindow(drawArea),
      KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
      | Button1MotionMask );
*/
    Widget tools = XtVaCreateManagedWidget(
            "Command window",			/* widget name */
            xmRowColumnWidgetClass,			/* widget class */
            mainWin,				/* parent widget */
            NULL);

    rowColumn = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            tools,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmHORIZONTAL,	/* orientation */
            XmNpacking, XmPACK_COLUMN,	/* packing mode */
            NULL);				/* terminate varargs list */


    // Menu bar
    XmString file  = XmStringCreateSimple("File");
    XmString clear = XmStringCreateSimple("Clear");
    XmString clearShortCut = XmStringCreateSimple("Ctrl+C");
    XmString quit = XmStringCreateSimple("Quit");
    XmString quitShortCut = XmStringCreateSimple("Ctrl+Q");
    XmString about  = XmStringCreateSimple("About");
    XmString help = XmStringCreateSimple("Help");
    XmString helpShortCut = XmStringCreateSimple("Ctrl+H");
    Widget menuBar = XmVaCreateSimpleMenuBar(
            mainWin,
            "menuBar",
            XmVaCASCADEBUTTON, file, NULL,
            XmVaCASCADEBUTTON, about, NULL,
            NULL);

    XmVaCreateSimplePulldownMenu(
            menuBar,
            "fileOptionMenu",
            0,
            OptionMenuCB,
            XmVaPUSHBUTTON, clear, XK_C, "Ctrl<Key>C", clearShortCut,
            XmVaPUSHBUTTON, quit,  XK_Q, "Ctrl<Key>Q", quitShortCut,
            NULL);

    XmVaCreateSimplePulldownMenu(
            menuBar,
            "aboutOptionMenu",
            1,
            AboutMenuCB,
            XmVaPUSHBUTTON, help, XK_H, "Ctrl<Key>H", helpShortCut,
            NULL);

    XmStringFree(file);
    XmStringFree(clear);
    XmStringFree(clearShortCut);
    XmStringFree(quit);
    XmStringFree(quitShortCut);
    XmStringFree(about);
    XmStringFree(help);
    XmStringFree(helpShortCut);

    // Select Shapes
    XmString dot = XmStringCreateSimple("Dot");
    XmString line = XmStringCreateSimple("Line");
    XmString rectangle = XmStringCreateSimple("Rectangle");
    XmString elipse = XmStringCreateSimple("Elipse");

    Widget holderShape = XtVaCreateManagedWidget(
            "holderShape",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnShape = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderShape,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("Shape", xmLabelWidgetClass, rowColumnShape, NULL);

    Widget shapeRadio = XmVaCreateSimpleRadioBox(
            rowColumnShape,
            "widthRadio",
            1,
            shapeRadioCB,
            XmVaRADIOBUTTON, dot, NULL, NULL, NULL,
            XmVaRADIOBUTTON, line, NULL, NULL, NULL,
            XmVaRADIOBUTTON, rectangle, NULL, NULL, NULL,
            XmVaRADIOBUTTON, elipse, NULL, NULL, NULL,
            NULL);

    XmStringFree(dot);
    XmStringFree(line);
    XmStringFree(rectangle);
    XmStringFree(elipse);

    // Select Widths
    XmString px_0 = XmStringCreateSimple("0 px");
    XmString px_3 = XmStringCreateSimple("3 px");
    XmString px_8 = XmStringCreateSimple("8 px");

    Widget holderThickness = XtVaCreateManagedWidget(
            "holderThickness",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnThickness = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderThickness,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("Thickness", xmLabelWidgetClass, rowColumnThickness, NULL);

    Widget thicknessRadio = XmVaCreateSimpleRadioBox(
            rowColumnThickness,
            "thicknessRadio",
            0,
            thicknessRadioCB,
            XmVaRADIOBUTTON, px_0, NULL, NULL, NULL,
            XmVaRADIOBUTTON, px_3, NULL, NULL, NULL,
            XmVaRADIOBUTTON, px_8, NULL, NULL, NULL,
            NULL);

    XmStringFree(px_0);
    XmStringFree(px_3);
    XmStringFree(px_8);

    // Select Color
    initColors(topLevel, colors, NUM_COLORS);

    XmString black = XmStringCreateSimple(names[0]);
    XmString white = XmStringCreateSimple(names[1]);
    XmString red = XmStringCreateSimple(names[2]);
    XmString green = XmStringCreateSimple(names[3]);
    XmString blue = XmStringCreateSimple(names[4]);

    Widget holderFGColor = XtVaCreateManagedWidget(
            "holderFGColor",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnFGColor = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderFGColor,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("FG Color", xmLabelWidgetClass, rowColumnFGColor, NULL);

    Widget fgColorRadio = XmVaCreateSimpleRadioBox(
            rowColumnFGColor,
            "colorFGRadio",
            0,
            fgColorRadioCB,
            XmVaRADIOBUTTON, black, 'k', NULL, NULL,
            XmVaRADIOBUTTON, white, 'W', NULL, NULL,
            XmVaRADIOBUTTON, red, 'R', NULL, NULL,
            XmVaRADIOBUTTON, green, 'G', NULL, NULL,
            XmVaRADIOBUTTON, blue, 'B', NULL, NULL,
            NULL);

    Widget holderBGColor = XtVaCreateManagedWidget(
            "holderBGColor",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnBGColor = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderBGColor,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("BG Color", xmLabelWidgetClass, rowColumnBGColor, NULL);

    Widget bgColorRadio = XmVaCreateSimpleRadioBox(
            rowColumnBGColor,
            "colorBGRadio",
            1,
            bgColorRadioCB,
            XmVaRADIOBUTTON, black, 'k', NULL, NULL,
            XmVaRADIOBUTTON, white, 'W', NULL, NULL,
            XmVaRADIOBUTTON, red, 'R', NULL, NULL,
            XmVaRADIOBUTTON, green, 'G', NULL, NULL,
            XmVaRADIOBUTTON, blue, 'B', NULL, NULL,
            NULL);


    // Fill setting
    XmString empty = XmStringCreateSimple("Empty");
    XmString fill = XmStringCreateSimple("Fill");
    Widget holderFill = XtVaCreateManagedWidget(
            "holderFill",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnFill = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderFill,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("Filling", xmLabelWidgetClass, rowColumnFill, NULL);

    Widget fillRadio = XmVaCreateSimpleRadioBox(
            rowColumnFill,
            "fillRadio",
            0,
            fillRadioCB,
            XmVaRADIOBUTTON, empty, 'k', NULL, NULL,
            XmVaRADIOBUTTON, fill, 'W', NULL, NULL,
            NULL);


    XmStringFree(empty);
    XmStringFree(fill);


    Widget holderFillColor = XtVaCreateManagedWidget(
            "holderFillColor",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnFillColor = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderFillColor,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("Filling Color", xmLabelWidgetClass, rowColumnFillColor, NULL);

    Widget fillColorRadio = XmVaCreateSimpleRadioBox(
            rowColumnFillColor,
            "colorFillRadio",
            0,
            fillColorRadioCB,
            XmVaRADIOBUTTON, black, 'k', NULL, NULL,
            XmVaRADIOBUTTON, white, 'W', NULL, NULL,
            XmVaRADIOBUTTON, red, 'R', NULL, NULL,
            XmVaRADIOBUTTON, green, 'G', NULL, NULL,
            XmVaRADIOBUTTON, blue, 'B', NULL, NULL,
            NULL);

    XmStringFree(black);
    XmStringFree(white);
    XmStringFree(red);
    XmStringFree(green);
    XmStringFree(blue);


    // Dash style
    // Fill setting

    XmString solid = XmStringCreateSimple("Solid");
    XmString doubleDash = XmStringCreateSimple("DoubleDash");
    Widget holderLineStyle = XtVaCreateManagedWidget(
            "holderLineStyle",
            xmFrameWidgetClass,
            rowColumn,
            NULL);

    Widget rowColumnLineStyle = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            holderLineStyle,				/* parent widget */
            XmNentryAlignment, XmALIGNMENT_CENTER,	/* alignment */
            XmNorientation, XmVERTICAL,	/* orientation */
            XmNpacking, XmPACK_TIGHT,	/* packing mode */
            NULL);

    XtVaCreateManagedWidget("Line style", xmLabelWidgetClass, rowColumnLineStyle, NULL);

    Widget lineStyleRadio = XmVaCreateSimpleRadioBox(
            rowColumnLineStyle,
            "lineStyleRadio",
            0,
            lineStyleRadioCB,
            XmVaRADIOBUTTON, solid, 'k', NULL, NULL,
            XmVaRADIOBUTTON, doubleDash, 'W', NULL, NULL,
            NULL);


    XmStringFree(solid);
    XmStringFree(doubleDash);


    // Dialog window
    XmString quitMessage = XmStringCreateSimple("Quit application?");
    XmString quitYes = XmStringCreateSimple("Yes");
    XmString quitNo = XmStringCreateSimple("No");

    quitDialog = XmCreateQuestionDialog(topLevel, "close", NULL, 0);
    XtVaSetValues(quitDialog,
                  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                  XmNmessageString, quitMessage,
                  XmNokLabelString, quitYes,
                  XmNcancelLabelString, quitNo,
                  NULL);

    XtAddCallback(quitDialog, XmNokCallback, QuitCB, NULL);

    XmStringFree(quitMessage);
    XmStringFree(quitYes);
    XmStringFree(quitNo);


    Widget shell = XtVaCreateWidget("dialog_shell",
                             xmDialogShellWidgetClass,
                             topLevel,
                             NULL);
    helpDialog = XtVaCreateWidget("dialog_box",
                              xmMessageBoxWidgetClass,
                              shell,
                              NULL);
    XtUnmanageChild(XmMessageBoxGetChild(helpDialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(helpDialog, XmDIALOG_HELP_BUTTON));


    XtVaSetValues(helpDialog, XmNdialogStyle, XmDIALOG_MODELESS, NULL);

    String helpText = "Help of application Draw:\n\n"
            "In application Draw you can draw different shapes of object\n"
            "and colors. By selecting a choice in toolbox on the bottom\n"
            "of application you can pick what will you draw. To draw an\n"
            "object click on drawing area.\n\n"
            "Created by Tomas Blazek (xblaze31)\n";

    XtVaCreateManagedWidget(helpText, xmLabelWidgetClass, helpDialog, NULL);


    // Set rest of callbacks and manage created Widgets

    XmMainWindowSetAreas(mainWin, NULL, tools, NULL, NULL, frame);

    XtAddCallback(drawArea, XmNinputCallback, DrawObjectCB, drawArea);
    XtAddEventHandler(drawArea, ButtonMotionMask, False, InputObjectEH, NULL);
    XtAddCallback(drawArea, XmNexposeCallback, ExposeCB, drawArea);


    XtRealizeWidget(topLevel);

    XtManageChild(menuBar);
    XtManageChild(thicknessRadio);
    XtManageChild(shapeRadio);
    XtManageChild(fgColorRadio);
    XtManageChild(bgColorRadio);
    XtManageChild(fillColorRadio);
    XtManageChild(fillRadio);
    XtManageChild(lineStyleRadio);
}


int main(int argc, char **argv)
{
    XtAppContext app_context;

    /*
     * Register the default language procedure
     */
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

    initApp(&app_context, argc, argv);

    XtAppMainLoop(app_context);

    return 0;
}

