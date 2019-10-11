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
#define WIDTH 500   /* window width */
#define HEIGHT 400   /* window height */

enum Shape {Dot, Line, Rectangle, Elipse} c_shape = Line; /* current shape */
typedef struct PictureElement{
    enum Shape shape;
    int x1;
    int y1;
    int x2;
    int y2;
} PictureElement;

PictureElement *objects = NULL; /* array of objects */
int maxObjects = 0;		/* space allocated for max objects */
int n_object = 0;			/* current number of objects */

GC drawGC = 0;			/* GC used for final drawing */
Widget drawArea;
Widget quitDialog;		    /* Display dialog on application close*/


int x1, y1, x2, y2;		/* input points */ 
int button_pressed = 0;		/* input state */


void swap(int *a, int *b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}


void DrawObject(Widget w, GC gc, int x1, int y1, int x2, int y2){
    if (c_shape == Rectangle || c_shape == Elipse) {
        if (x1 > x2) {
            swap(&x1, &x2);
        }
        if (y1 > y2) {
            swap(&y1, &y2);
        }
    }

    int width = x2 - x1;
    int height = y2 - y1;

	switch(c_shape){
        case Dot:
            XDrawPoint(XtDisplay(w), XtWindow(w), gc, x1, y1);
            break;
		case Line:
			XDrawLine(XtDisplay(w), XtWindow(w), gc, x1, y1, x2, y2);
			break;
		case Rectangle:
			XDrawRectangle(XtDisplay(w), XtWindow(w), gc, x1, y1, width, height);
			break;
        case Elipse:
            XDrawArc(XtDisplay(w), XtWindow(w), gc, x1, y1, width, height, 0, 360*64);
            break;
		default:
			break;
    }	
}


/*
 * "InputLine" event handler
 */
/* ARGSUSED */
void InputLineEH(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
    Pixel	fg, bg;
    static GC inputGC = 0;			/* GC used for drawing current position */

    if (button_pressed) {
		if (!inputGC) {
		    inputGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
		    XSetFunction(XtDisplay(w), inputGC, GXxor);
		    XSetPlaneMask(XtDisplay(w), inputGC, ~0);
		    XtVaGetValues(w, XmNforeground, &fg, XmNbackground, &bg, NULL);
		    XSetForeground(XtDisplay(w), inputGC, fg ^ bg);
		}

		if (button_pressed > 1) {
		    /* erase previous position */
		    DrawObject(w, inputGC, x1, y1, x2, y2);
		} else {
		    /* remember first MotionNotify */
		    button_pressed = 2;
		}

		x2 = event->xmotion.x;
		y2 = event->xmotion.y;

		DrawObject(w, inputGC, x1, y1, x2, y2);
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

                DrawObject(w, drawGC, x1, y1, objects[n_object - 1].x2, objects[n_object - 1].y2);
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
        DrawObject(w, drawGC, objects[i].x1, objects[i].y1, objects[i].x2, objects[i].y2);
    }
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
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    exit(0);
}

void OptionMenuCB(Widget w, XtPointer client_data, XtPointer call_data){
    if (client_data == 0){
        ClearCB(w, drawArea, call_data);
    } else {
        QuitDialogCB(w, client_data, call_data);
    }
}

void AboutMenuCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("About App\n");
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


void initApp(XtAppContext *app_context, int argc, char* argv[]){
    Widget topLevel, mainWin, frame, rowColumn;

    topLevel = XtVaAppInitialize(
            app_context,		 	/* Application context */
            "Draw",				/* Application class */
            NULL, 0,				/* command line option list */
            &argc, argv,
            NULL,/* command line args */
            XmNdeleteResponse, XmDO_NOTHING,    /* for missing app-defaults file */
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
            XmNwidth, WIDTH,			/* set startup width */
            XmNheight, HEIGHT,			/* set startup height */
            NULL);				/* terminate varargs list */

/*
    XSelectInput(XtDisplay(drawArea), XtWindow(drawArea),
      KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
      | Button1MotionMask );
*/

    rowColumn = XtVaCreateManagedWidget(
            "rowColumn",			/* widget name */
            xmRowColumnWidgetClass,		/* widget class */
            mainWin,				/* parent widget */
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
            XmVaPUSHBUTTON, help, XK_H, "Ctrl<Key>A", helpShortCut,
            NULL);

    XmStringFree(file);
    XmStringFree(clear);
    XmStringFree(clearShortCut);
    XmStringFree(quit);
    XmStringFree(quitShortCut);
    XmStringFree(about);
    XmStringFree(help);
    XmStringFree(helpShortCut);


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

    XmStringFree(quitMessage);
    XmStringFree(quitYes);
    XmStringFree(quitNo);

    XtAddCallback(quitDialog, XmNokCallback, QuitCB, NULL);
    
    // Select Shapes
    XmString dot = XmStringCreateSimple("Dot");
    XmString line = XmStringCreateSimple("Line");
    XmString rectangle = XmStringCreateSimple("Rectangle");
    XmString elipse = XmStringCreateSimple("Elipse");
    XtVaCreateManagedWidget("Shape", xmLabelWidgetClass, rowColumn, NULL);

    Widget holderShape = XtVaCreateManagedWidget(
            "holderShape",
            xmFrameWidgetClass,
            rowColumn,
            NULL);
    Widget shapeRadio = XmVaCreateSimpleRadioBox(
            holderShape,
            "widthRadio",
            WIDTH,
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
    XtVaCreateManagedWidget("Width", xmLabelWidgetClass, rowColumn, NULL);

    Widget holderWidth = XtVaCreateManagedWidget(
            "holderWidth",
            xmFrameWidgetClass,
            rowColumn,
            NULL);
    Widget widthRadio = XmVaCreateSimpleRadioBox(
            holderWidth,
            "widthRadio",
            WIDTH,
            NULL,
            XmVaRADIOBUTTON, px_0, NULL, NULL, NULL,
            XmVaRADIOBUTTON, px_3, NULL, NULL, NULL,
            XmVaRADIOBUTTON, px_8, NULL, NULL, NULL,
            NULL);

    XmStringFree(px_0);
    XmStringFree(px_3);
    XmStringFree(px_8);

    // Set rest of callbacks and manage created Widgets

    XmMainWindowSetAreas(mainWin, NULL, rowColumn, NULL, NULL, frame);

    XtAddCallback(drawArea, XmNinputCallback, DrawObjectCB, drawArea);
    XtAddEventHandler(drawArea, ButtonMotionMask, False, InputLineEH, NULL);
    XtAddCallback(drawArea, XmNexposeCallback, ExposeCB, drawArea);


    XtRealizeWidget(topLevel);

    XtManageChild(menuBar);
    XtManageChild(widthRadio);
    XtManageChild(shapeRadio);
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

