#ifndef EPSPLOT_H
#define EPSPLOT_H

#include <stdio.h>
#include	<vector>
#include	<cassert>

// by Olli Niemitalo, http://www.student.oulu.fi/~oniemita/DSP/INDEX.HTM
// Use freely. August 2001

// Modifications:
// LDS = Laurent de Soras - http://ldesoras.free.fr
// 2003.05.07 - LDS - Added const qualifier to *text in drawLegend().
// 2003.05.13 - LDS - Fixed gSave/gRestore bug by adding a context stack and tweaking the beginning of the drawText() function
// 2003.05.17 - LDS - Added flags for automatic area filling and path closing
// 2005.02.13 - LDS - Fixed printString() to print correctly percent characters
// 2016.10.09 - LDS - Fixed the headers
// 2019.12.18 - LDS - Fixed delete []

// A class for writing Encapsulated PostScript figures
class EPSPlot {
private:

	class Context
	{
	public:
		float				_fontsize;
	};

	std::vector <Context>
						_stack;
	bool				_closepath_flag;
	bool				_fill_flag;
	FILE *			stream;

	// Calculate squared distance from a point to a line in 2D space
	//
	// x0, y0 = Point coordinates
	// x1, y1 = Line startpoint coordinates
	// x2, y2 = Line endpoint coordinates
	//
	// Returns: squared distance of point (x0, y0) and line (x1, y1)..(x2,y2)
	float calcPointLineSqrDist(float x0, float y0, float x1, float y1, float x2, float y2) {
		if ((x1 == x2) && (y1 == y2)) return (x0-x1)*(x0-x1)+(y0-y1)*(y0-y1);
		float temp = x0*(y1-y2)-x1*(y0-y2)-x2*(y1-y0);
		return temp*temp/(x1*x1-2.0f*x1*x2+x2*x2+y1*y1-2.0f*y1*y2+y2*y2);
	}

	// Printing routines
	// -----------------

	// Print a float number into .EPS
	void printFloat(float number) {
		if (stream) {
			if (number == (int)number) fprintf(stream, "%d", (int)number);
			else fprintf(stream, "%.3f", number);
		}
	}

	// Print a float number and a space into .EPS
	void printFloatAndSpace(float number) {
		if (stream) {
			if (number == (int)number) fprintf(stream, "%d ", (int)number);
			else fprintf(stream, "%.3f ", number);
		}
	}

	// Print an integer number into .EPS
	void printInt(int number) {
		if (stream) {
			fprintf(stream, "%d", number);
		}
	}

	// Print an integer number and a space into .EPS
	void printIntAndSpace(int number) {
		if (stream) {
			fprintf(stream, "%d ", number);
		}
	}

	// Print a string into .EPS
	void printString(const char *string) {
		if (stream) {
			fprintf(stream, "%s", string);
		}
	}

	// PostScript commands for internal use
	// ------------------------------------

	// Create new path
	void newPath() {
		printString("newpath\n");
	}

	// Move position
	//
	// x, y = New position
	void moveTo(float x, float y) {
		printFloatAndSpace(x);
		printFloatAndSpace(y);
		printString("moveto\n");
	}

	// Add a line to the current shape, starting from current position,
	// and move to the new position.
	// 
	// x, y = Line endpoint and new position
	void lineTo(float x, float y) {
		printFloatAndSpace(x);
		printFloatAndSpace(y);
		printString("lineto\n");
	}

	// Connect last point of path to first point with a line
	void closePath() {
		printString("closepath\n");
	}

	// Stroke a path
	void stroke() {
		printString("stroke\n");
	}
	
	// Fill a path
	void fill() {
		printString("fill\n");
	}

	void	stroke_or_fill ()
	{
		if (_fill_flag)
		{
			fill ();
		}
		else
		{
			stroke ();
		}
	}

	// Find a font
	// 
	// font = Font name (for example "/Helvetica", "/Times-Roman")
	void findFont(const char *font) {
		printString(font);
		printString(" findfont\n");
	}
	
	// Scale font
	//
	// scale = Size of font
	void scaleFont(float scale) {
		printFloatAndSpace(scale);
		printString(" scalefont\n");
	}

	// Set font
	void setFont() {
		printString("setfont\n");
	}

	// Show (text)
	//
	// text = Text to show
	void show(const char *text) {
		printString("(");
		printString(text);
		printString(") show\n");
	}

	// Add text to path
	//
	// text = Text to add
	void charPath(const char *text) {
		printString("(");
		printString(text);
		printString(") true charpath\n");
	}

public:

	// User-callable PostScript commands
	// ---------------------------------	

	// Save state
	void gSave() {
		printString("gsave\n");
		Context	nc;
		nc._fontsize = 0;
		_stack.push_back (nc);
	}

	// Restore state
	void gRestore() {
		assert (_stack.size () > 0);
		_stack.pop_back ();
		printString("grestore\n");
	}

	// Set gray scale
	//
	// scale = 0..1 = black..white
	void setGray(float scale) {
		printFloatAndSpace(scale);
		printString("setgray\n");
	}

	// Set RGB color
	//
	// red   = 0..1 = black .. bright red
	// green = 0..1 = black .. bright green
	// blue  = 0..1 = black .. bright blue
	void setRGBColor(float red, float green, float blue) {
		printFloatAndSpace(red);
		printFloatAndSpace(green);
		printFloatAndSpace(blue);
		printString("setrgbcolor\n");
	}

	// Set line width
	//
	// width = Line width
	void setLineWidth(float width) {
		printFloatAndSpace(width);
		printString("setlinewidth\n");
	}

	// Set dash
	//
	// ms[]   = Marks and spaces vector. For example {1, 2, 3} would set dash
	//          to 1 unit mark, 2 unit space, 3 unit mark.
	// count  = Number of elements in marks and spaces vector (3 for above example)
	// offset = Offset of the first dash (typically 0)
	void setDash(const float *ms, int count, float offset) {
		printString("[");
		for (int t = 0; (t < count); t++) {
			if (t == (count-1)) printFloat(ms[t]);
			else printFloatAndSpace(ms[t]);
		}			
		printString("] ");
		printFloatAndSpace(offset);
		printString("setdash\n");
	}

	// Set line cap
	//
	// cap = 0 = Butt caps
	//       1 = Round caps
	//       2 = Extended butt caps
	void setLineCap(int cap) {
		printIntAndSpace(cap);
		printString("setlinecap\n");
	}

	// Set line join
	//
	// join = 0 = Miter join
	//        1 = Round join
	//        2 = Bevel join
	void setLineJoin(int join) {
		printIntAndSpace(join);
		printString("setlinejoin\n");
	}

	// High level routines
	// -------------------

	void	set_fill_path (bool flag)
	{
		_fill_flag = flag;
	}

	bool	is_path_filled () const
	{
		return (_fill_flag);
	}

	void	set_close_path (bool flag)
	{
		_closepath_flag = flag;
	}

	bool	is_path_closed () const
	{
		return (_closepath_flag);
	}

	// Set font size
	//
	// size = font size in points
	void setFontSize(float size) {
		if (size != _stack.back ()._fontsize) {
			findFont("/Helvetica"); // Modify this is you want a different font
			_stack.back ()._fontsize = size;
			scaleFont(size);
			setFont();
		}
	}

	// Draw text, optionally aligned and rotated
	//
	// x, y   = Bottom left coordinates (before rotation around that point)
	// text   = Text to write
	// halign = Horizontal alignment, 0 = left, 0.5 = center, 1 = right (other values allowed too)
	// valign = Vertical alignment, 0 = bottom, 0.5 = middle, 1 = top (other values allowed too)
	// angle  = Angle to rotate counterclockwise (in degrees)
	void drawText(float x, float y, const char *text, float halign, float valign, float angle) {
		valign *= 0.7f; // Font-dependent adjustment
		gSave();
		float			cur_size = _stack.at (_stack.size () - 2)._fontsize;	// Automatically inherits from previous context
		if (cur_size == 0)
		{
			cur_size = 6;	// Default font size is 6 if not defined in current (actually previous) context
		}
		setFontSize (cur_size);
		printFloatAndSpace(x);
		printFloatAndSpace(y);
		printString("translate\n");
		if (angle != 0) {
			printFloatAndSpace(angle);
			printString("rotate\n");
		}
		newPath();
		printString("(");
		printString(text);
		if (halign != 0) {
			printString(") dup dup stringwidth pop\n");
			printFloatAndSpace(-halign);
			printString("mul dup ");
			printFloat(-valign * _stack.back ()._fontsize);
			printString(" moveto exch\n");
		} else {
			printString(") dup\n");
			moveTo(0, -valign * _stack.back ()._fontsize);
		}
		printString("true charpath\n");
		setGray(1);
		setNoDash();
		setLineWidth(1);
		setLineJoin(1);
		stroke();
		newPath();
		setGray(0);
		if (halign == 0)
		{
			printFloatAndSpace(0);
		}
		printFloatAndSpace(-valign * _stack.back ()._fontsize);
		printString("moveto show\n");		
		gRestore();
	}
	// Draw text with horizontal and vertical alignment
	void drawText(float x, float y, const char *text, float halign, float valign) {
		drawText(x, y, text, halign, valign, 0);
	}
	// Draw text with horizontal alignment
	void drawText(float x, float y, const char *text, float halign) {
		drawText(x, y, text, halign, 0);
	}
	// Draw text
	void drawText(float x, float y, const char *text) {
		drawText(x, y, text, 0);
	}

	// Set dash in a simple way
	//
	// mark   = Mark length
	// space  = Space length
	void setDash(float mark, float space) {
		const float markspace[2] = {mark, space};
		setDash(markspace, 2, mark/2.0f);
	}
	// Set a more complex dash
	void setDash(float mark1, float space1, float mark2, float space2) {
		const float markspace[4] = {mark1, space1, mark2, space2};
		setDash(markspace, 4, mark1/2.0f);
	}
	// Set no dash
	void setNoDash() {
		setDash(nullptr, 0, 0);
	}

	// Draw a line
	//
	// x1, y1 = Start position
	// x2, y2 = End position
	void drawLine(float x1, float y1, float x2, float y2) {
		newPath();
		moveTo(x1, y1);
		lineTo(x2, y2);
		stroke();
	}

	// Draw legend (explain what a curve of a color means in a graph)
	//
	// x, y = right middle coordinates of stuff to be drawn
	//
	// This is just a small routine to assist in graph drawing
	void drawLegend(float x, float y, const char *text) {
		drawLine(x, y, x-15, y);
		drawText(x-17, y, text, 1, 0.5);
	}

	// Draw several connected lines
	//
	// x[], y[] = Vertex x and y coordinates
	// count    = Number of vertices = x and y vector length
	void drawLines(const float *x, const float *y, int count) {
		newPath();
		for (int t = 0; (t < count); t++) {
			if (t == 0) moveTo(x[t], y[t]);
			else lineTo(x[t], y[t]);
		}
		if (_closepath_flag)
		{
			closePath ();
		}
		stroke_or_fill ();
	}

	// Draw several connected lines, with optimization
	//
	// x[], y[] = Vertex x and y coordinates
	// count    = Number of vertices = x and y vector length
	// maxerror = Maximum visual error (same unit as in other places)
	void drawLines(const float *x, const float *y, int count, float maxerror) {
		float maxerrorsqr = maxerror*maxerror;
		float *newx = new float[count];
		float *newy = new float[count];
		int newcount = 0;
		for (int t1 = 0; (t1 < count); t1++) {
			int tryskip = 1;
			for (; ((t1+tryskip) < count); tryskip++) {
				int t2 = t1+tryskip+1;
				int t0 = t1+1;
				for (; (t0 < t2); t0++) {
					if (calcPointLineSqrDist(x[t0], y[t0], x[t1], y[t1], x[t2], y[t2]) > maxerrorsqr) {
						break;
					}
				}
				if (t0 != t2) break;
			}
			int skip = tryskip-1;
			newx[newcount] = x[t1];
			newy[newcount] = y[t1];
			newcount++;
			t1 += skip;
		}
		if (newcount < count) {
			newx[newcount] = x[count-1];
			newy[newcount] = y[count-1];
			newcount++;
		}
		drawLines(newx, newy, newcount);
		delete [] newx;
		delete [] newy;
	}

	// Draw several connected lines, with top and bottom clipped and optimization
	//
	// x[], y[] = Vertex x and y coordinates
	// count    = Number of vertices = x and y vector length
	// maxerror = Maximum visual error (same unit as in other places)
	// y1       = Bottom y coordinate of clipping area
	// y2       = Top y coordinate of clipping area
	void drawLines(const float *x, const float *y, int count, float maxerror, float y1, float y2) {
		float *newx = new float[count];
		float *newy = new float[count];
		int t1, t2; // Startpoint and endpoint of lines sequence
		for(t1 = 0; (t1 < (count-1));) {
			if ((y[t1] < y1) && (y[t1+1] >= y1)) {
				// Must clip bottom near t1. Find t2
				newx[t1] = x[t1] + (y1-y[t1])*(x[t1+1]-x[t1])/(y[t1+1]-y[t1]);
				newy[t1] = y1;
			} else if ((y[t1] > y2) && (y[t1+1] <= y2)) {
				// Must clip top near t1. Find t2
				newx[t1] = x[t1] + (y2-y[t1])*(x[t1+1]-x[t1])/(y[t1+1]-y[t1]);
				newy[t1] = y2;
			} else {
				newx[t1] = x[t1];
				newy[t1] = y[t1];
			}
			if ((newy[t1] >= y1) && (newy[t1] <= y2)) {
				for (t2 = t1+1;; t2++) {
					if (y[t2] < y1) {
						// Must clip bottom near t2
						newx[t2] = x[t2-1] + (y1-y[t2-1])*(x[t2]-x[t2-1])/(y[t2]-y[t2-1]);
						newy[t2] = y1;
						break;
					} 
					else if (y[t2] > y2) {
						// Must clip top near t2
						newx[t2] = x[t2-1] + (y2-y[t2-1])*(x[t2]-x[t2-1])/(y[t2]-y[t2-1]);
						newy[t2] = y2;
						break;
					}
					newx[t2] = x[t2];
					newy[t2] = y[t2];
					if (t2 == (count-1)) break;
				}
				drawLines(&newx[t1], &newy[t1], t2-t1+1, maxerror);					
				t1 = t2;
			} else {
				t1++;
			}
		} 
		delete[] newx;
		delete[] newy;
	}

	// Draw several connected lines, with rectangular clipping area and optimization
	//
	// x[], y[] = Vertex x and y coordinates
	// count    = Number of vertices = x and y vector length
	// maxerror = Maximum visual error (same unit as in other places)
	// x1, y1   = Bottom-left coordinates of clipping area
	// x2, y2   = Top-right coordinates of clipping area
	void drawLines(const float *x, const float *y, int count, float maxerror, float x1, float y1, float x2, float y2) {
		float *newx = new float[count];
		float *newy = new float[count];
		int t1, t2; // Startpoint and endpoint of lines sequence
		for(t1 = 0; (t1 < (count-1));) {
			if ((x[t1] < x1) && (x[t1+1] >= x1)) {
				// Must clip left near t1. Find t2
				newx[t1] = x1;
				newy[t1] = y[t1] + (x1-x[t1])*(y[t1+1]-y[t1])/(x[t1+1]-x[t1]);
			} else if ((x[t1] > x2) && (x[t1+1] <= x2)) {
				// Must clip right near t1. Find t2
				newx[t1] = x2;
				newy[t1] = y[t1] + (x2-x[t1])*(y[t1+1]-y[t1])/(x[t1+1]-x[t1]);
			} else {
				newx[t1] = x[t1];
				newy[t1] = y[t1];
			}
			if ((newx[t1] >= x1) && (newx[t1] <= x2)) {
				for (t2 = t1+1;; t2++) {
					if (x[t2] < x1) {
						// Must clip left near t2
						newx[t2] = x1;
						newy[t2] = y[t2-1] + (x1-x[t2-1])*(y[t2]-y[t2-1])/(x[t2]-x[t2-1]);
						break;
					} 
					else if (x[t2] > x2) {
						// Must clip right near t2
						newx[t2] = x2;
						newy[t2] = y[t2-1] + (x2-x[t2-1])*(y[t2]-y[t2-1])/(x[t2]-x[t2-1]);
						break;
					}
					newx[t2] = x[t2];
					newy[t2] = y[t2];
					if (t2 == (count-1)) break;
				}
				drawLines(&newx[t1], &newy[t1], t2-t1+1, maxerror, y1, y2);
				t1 = t2;
			} else {
				t1++;
			}
		} 
		delete[] newx;
		delete[] newy;
	}

	// Draw a box made of lines
	//
	// x1, y1 = Position of one corner of the box
	// x2, y2 = Position of the opposite corner of the box
	void drawBox(float x1, float y1, float x2, float y2) {
		moveTo(x1, y1);
		lineTo(x2, y1);
		lineTo(x2, y2);
		lineTo(x1, y2);
		closePath();
		stroke();
	}

	// Constructor
	//
	// filename = Path and name of output file
	// x1, y1   = Bounding box bottom left corner (for example 0, 0)
	// x2, y2   = Bounding box top right corner (for example 100, 100)
	//            1/72 inch units. This defines the size of the figure you are creating. 
	//            (The same units is used in drawing routines)
	//
	// Note that if you draw objects as large as the bounding box, half of the line
	// width may get clipped away by a program that imports the EPS. So add about a line width
	// to the bounding box size. Most programs understand negative bounding box coordinates,
	// some don't.
	EPSPlot(const char *filename, float x1, float y1, float x2, float y2)
	:	_stack (1)
	,	_closepath_flag (false)
	,	_fill_flag (false)
	{
		_stack.back ()._fontsize = 0;
		stream = fopen(filename, "wt");
		printString("%!PS-Adobe-3.0 EPSF-3.0\n%%BoundingBox: ");
		printFloatAndSpace(x1);
		printFloatAndSpace(y1);
		printFloatAndSpace(x2);
		printFloat(y2);
		printString("\n");
	}

	// Destructor. Closes file
	~EPSPlot() {
		if (stream)
		{
			printString("%%Trailer\n");
			fclose(stream);
		}
	}
};

#endif
