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

/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * Shared variables
 */

#define LINES_ALLOC_STEP	10	/* memory allocation stepping */
XSegment *objects = NULL;		/* array of line descriptors */
int maxObjects = 0;		/* space allocated for max objects */
int n_object = 0;			/* current number of objects */

GC drawGC = 0;			/* GC used for final drawing */
GC inputGC = 0;			/* GC used for drawing current position */

int x1, y1, x2, y2;		/* input points */
int button_pressed = 0;		/* input state */
enum Shape {Line, Rectangle} c_shape = Rectangle; /* current shape */


void swap(int *a, int *b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}


void DrawObject(Widget w, GC gc, int x1, int y1, int x2, int y2){
	int height, width;
	switch(c_shape){
		case Line:
			XDrawLine(XtDisplay(w), XtWindow(w), gc, x1, y1, x2, y2);
			break;
		case Rectangle:
			if (x1 > x2){
				swap(&x1, &x2);
			}
			if (y1 > y2){
				swap(&y1, &y2);
			}

			width = x2 - x1;
			height = y2 - y1;
			XDrawRectangle(XtDisplay(w), XtWindow(w), gc, x1, y1, width, height);
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
					objects = (XSegment*) XtRealloc((char*)objects,
													(Cardinal)(sizeof(XSegment) * maxObjects));
				}

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

	if (n_object <= 0)
		return;
	if (!drawGC)
		drawGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
	XDrawSegments(XtDisplay(w), XtWindow(w), drawGC, objects, n_object);
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

/*
 * "Quit" button callback function
 */
/* ARGSUSED */
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{

	exit(0);
}


int main(int argc, char **argv)
{
	XtAppContext app_context;
	Widget topLevel, mainWin, frame, drawArea, rowColumn, quitBtn, clearBtn;

	/*
     * Register the default language procedure
     */
	XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

	topLevel = XtVaAppInitialize(
			&app_context,		 	/* Application context */
			"Draw",				/* Application class */
			NULL, 0,				/* command line option list */
			&argc, argv,			/* command line args */
			NULL,				/* for missing app-defaults file */
			NULL);				/* terminate varargs list */

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
			XmNwidth, 500,			/* set startup width */
			XmNheight, 400,			/* set startup height */
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

	clearBtn = XtVaCreateManagedWidget(
			"Clear",				/* widget name */
			xmPushButtonWidgetClass,		/* widget class */
			rowColumn,			/* parent widget*/
			NULL);				/* terminate varargs list */

	quitBtn = XtVaCreateManagedWidget(
			"Quit",				/* widget name */
			xmPushButtonWidgetClass,		/* widget class */
			rowColumn,			/* parent widget*/
			NULL);				/* terminate varargs list */

	XmMainWindowSetAreas(mainWin, NULL, rowColumn, NULL, NULL, frame);

	XtAddCallback(drawArea, XmNinputCallback, DrawObjectCB, drawArea);
	XtAddEventHandler(drawArea, ButtonMotionMask, False, InputLineEH, NULL);
	XtAddCallback(drawArea, XmNexposeCallback, ExposeCB, drawArea);

	XtAddCallback(clearBtn, XmNactivateCallback, ClearCB, drawArea);
	XtAddCallback(quitBtn, XmNactivateCallback, QuitCB, 0);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);

	return 0;
}

