/** \file CMedit.H

	Header File for CMedit
	
	This file contains some constants and mainly the class CMedit, the Graphical colormap editor.

*/

#ifndef _GVIEW_H
#define _GVIEW_H


#include <stdlib.h>
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <ctype.h>
#include <stack>
#include <deque>
#include <vector>

// ***** Constants Definitions

#define CMENTMAX  16384 ///< The maximum number of colormap entries

extern "C" {
	extern void hsb2rgb(float h, float s, float b, float *rp, float *gp, float *bp);
    extern void rgb2hsb(float r, float g, float b,  float *hp, float *sp, float *bp);
} // To be rewritten as C++ functions

/** Enumerator for color components */
enum CMfield {
    HUE, SAT, BRIGHT,
    ALPHA,
    RED, GREEN, BLUE
};

struct colorEnt {
	int this_cment;
	float ent[4][CMENTMAX];
};

struct contPoint
{
	float data_x;
	float data_y;
	// int field;
	int type;
};

// ***** Constants Definitions END


// ***** CMedit Class Definition

/** CMedit Class. The class for graphical colormap editor */
class CMedit : public Fl_Gl_Window {

public:
    
    // **** Constructors
    	//taking in (x,y) as left-most point(?), and h as height, w as width of the drawing area.
	CMedit(int x, int y, int h, int w, const char *label = 0); ///< Constructor taking in top-left point of drawing area.
    // **** Constructors END


    // **** Getters and Setters
	
	void getrgba( int index, float rgba[4] ) const;
	void gethsba( int index, float hsba[4] ) const;

	int cment() const; ///< get number of colormap entries cment_.
	float get_hist_data_x_min(); ///< getter for the minimum data value (minimum histogram x value).
	float get_hist_data_x_max(); ///< getter for the maximum data value (minimum histogram x value).
	float get_hist_data_y_min();
	float get_hist_data_y_max();

	float get_data_x_min_for_cmap(); ///< getter for the minimum data value we want cmap to apply to.
	float get_data_x_max_for_cmap(); ///< getter for the maximum data value we want cmap to apply to.
	float get_data_y_min_for_cmap();
	float get_data_y_max_for_cmap();

	float get_data_x_min_for_display(); ///< getter for the minimum data value for display area.
	float get_data_x_max_for_display(); ///< getter for the maximum data value for display area.
	float get_data_y_min_for_hist_display();
	float get_data_y_max_for_hist_display();
	float get_data_y_min_for_cmap_display();
	float get_data_y_max_for_cmap_display();

	void cment( int newcment ); ///< set number of colormap entries cment_.
	void set_hist_data_x_min(float val);
	void set_hist_data_x_max(float val);
	void set_hist_data_y_min(float val);
	void set_hist_data_y_max(float val);

	void set_data_x_min_for_cmap(float val);
	void set_data_x_max_for_cmap(float val);
	void set_data_y_min_for_cmap(float val);
	void set_data_y_max_for_cmap(float val);


	void set_data_x_min_for_display(float val); 
	void set_data_x_max_for_display(float val);
	void set_data_y_min_for_hist_display(float val);
	void set_data_y_max_for_hist_display(float val);
	void set_data_y_min_for_cmap_display(float val);
	void set_data_y_max_for_cmap_display(float val);

	float hist_y_scaler( float dty );


	void set_editing_mode( int mode );

	void display_reset();

    // **** Getters and Setters END


	// **** I/O Stuffs

	int cmap_fload( FILE *inf ); ///< Handles the loading cmap file option.
	// previously fload
	int cmap_fsave( FILE *outf ); ///< Handles the saving cmap file option.
	// previously fsave
	int hist_fload( FILE *inf ); ///< Handles the loading histogram file option.

	virtual void draw(); //must be called draw()
	virtual int handle(int ev); //have to be called handle???
	virtual void resize(int nx, int ny, int nw, int nh);

	int handle_drawing(int ev);
	int handle_zooming(int ev);
	int handle_interpolation(int ev);

	// **** I/O Stuffs END


	// **** Mouse/Keyboard Handling

	float drag_drawing( int index, enum CMfield field, float value, float lerp = 1.0 );
	void dragrange( int x0, int x1, enum CMfield field, float v0, float v1, float lerp = 1.0 ); //what for?

	void fluid_moving( float x0, float x1, float y0, float y1 );
	void fluid_zooming( float x0, float x1, float y0, float y1 );
	// **** Mouse/Keyboard Handling END


	// **** Reporters (For updating cmed class objects)

	void report( int index );
	void report( );
	void reportto( void (*func)( CMedit *cm, int index ) );
	void cmentto( void (*cb)(const CMedit *) ) { cmentcb_ = cb; }
	
	void range_update_to( void(*func)( CMedit *cm ) );
	void updaterange();
	
	// **** Reporters (For updating cmed class objects) END


	// **** Undo/Redo

	void snapshot();
	//Need to be changed
	int undo();
	int redo();

	// **** Undo/Redo END


	// **** Other Stuffs that need changes
/*	void postscale( float );
	void postexpon( float );
	float postscale() const { return postscale_; } 
	float postexpon() const { return postexpon_; }

	float Aout( float Ain ) const;
*/


	int hsbmode;
	float lerpval;
	float vh[CMENTMAX], vs[CMENTMAX], vb[CMENTMAX], alpha[CMENTMAX];
	// **** Other Stuffs that need changes END


protected:

	int lastx_; //for updating colorpatch
	int locked, lockmin, lockmax; 
	//We are gonna use this for locking colormap for partial editing, later switching mode.
	int remin, remax;	/* repair region */ //Overlapped window part for some window system.
						// Only redraw certain area.
	enum CMfield dragfield; ///< Containing all possible color values for editing.
	int dragfrom;
	int dragamount;
	float dragval;
	//int draghue;
	

	float dragfrom_x;
	float dragfrom_y;

	int mouse_btn;

	float hueshift, huezoom;

	float hueshiffile;///usr/share/doc/HTML/index.htmlt, huezoom;

	char **comments;
	int ncomments, maxcomments;

	int editing_mode; ///< The flag for switching editing mode: 1 for drawing, 2 for zooming, 3 for interpolation


	// **** Coordinates Parameters
	int cment_; ///< Number of colormap entries
	int hisent_; ///< Number of Histogram entries
	// The above two are integers.

	int snapcment_;
	float snap[4][CMENTMAX];
	
	float hist_data_x_min_; ///< The minimum value for real data range which is also the range for whole histogram.
	float hist_data_x_max_; ///< The maximum value for real data range which is also the range for whole histogram
	float hist_data_y_min_; ///< The minimum height of histogram
	float hist_data_y_max_; ///< The maximum height of histogram

	float data_x_min_for_cmap_; ///< The minimum value in data range to which the colormap apply.
	float data_x_max_for_cmap_; ///< The maximum value in data range to which the colormap apply.
	float data_y_min_for_cmap_; 
	float data_y_max_for_cmap_;

	float data_x_min_for_display_; ///< The minimum value in data range which part of histgram & cmap we want to display.
	float data_x_max_for_display_; ///< The maximum value in data range which part of histgram & cmap we want to display.

	float data_y_min_for_hist_display_; ///< 
	float data_y_max_for_hist_display_;
	float data_y_min_for_cmap_display_;
	float data_y_max_for_cmap_display_; 


	float data_x_min_for_display_ori_;
	float data_x_max_for_display_ori_;
	float data_y_min_for_hist_display_ori_;
	float data_y_max_for_hist_display_ori_;

	// **** Coordinates Parameters END

	// float undo_stack[20][4][CMENTMAX];
	// float redo_stack[20][4][CMENTMAX];

	std::deque<colorEnt> undo_stack;
	std::deque<colorEnt> redo_stack;
	int undo_stack_count;
	int redo_stack_count;
	// **** Coordinates Transformations

	int cmapw();
	int wx2cmapx( int wx );
	//windows to drawing
	float wx2drx( int wx ); ///< pixel coordinate comes in as int

	float wy2dry( int wy ); ///< pixel coordinate comes in as int

	float dtx2drx( float dtx ); // data to drawing (Actually only converting the displaying part, between DISP_XMIN and DISP_XMAX, to drawing)

	float hdty2dry( float hdty ); // data to drawing (Actually only converting the displaying part, between HDISP_YMIN and HDISP_YMAX, to drawing)

	float cdty2dry( float cdty );

	float drx2dtx( float drx ); ///< drawing to data ( Actually to data range for display, between DISP_XMIN and DISP_XMAX I think ... )

	float dry2hdty( float dry );

	float cmapx2dtx( int cmapx ); ///< colormap to data ( Actually the cmap entries to data range which the cmap apply to )

	float histx2dtx( int histx ); ///< histogram to data ( hist entries to data range )

	int dtx2cmapx( float dtx ); ///< Data(Histogram) coord x to cmap coord x.

	float hist_display_y_scaler( float dty ); ///< Scaler for histogram display in y axis.



	// **** Coordinates Transformations END


	// **** Other Stuffs for now

	float *hist_ent_arr;



//	float postscale_, postexpon_;

	void (*cmentcb_)( const CMedit * );
	float huenear(float hue, float hueref);
	float hue2y(float hue);
	float y2hue(float y);

/*	int wx2x( int wx );
	float wy2y( int wy );
*/

	void (*reportfunc)( CMedit *, int index );

	void (*range_updater)(CMedit *);

// =====interpolation======

	// int start_flag;
	// int intStart;
	// float valStart;
	// int intEnd;
	// float valEnd;

	std::vector<contPoint> HUE_con_points; // x, y, type_of_interpolation; 0: linear
	std::vector<contPoint> SAT_con_points;
	std::vector<contPoint> BRI_con_points;
	std::vector<contPoint> ALP_con_points;

	int interp_linear(int x, float y, int field);
	int interp_linear_onePoint(int x, float y, std::vector<contPoint> v, float (&arr)[CMENTMAX]);
	void insert_into_controls(int x, float y, std::vector<contPoint> &v, int type);
	void init_contPoints();

// =====END interpolation======






	// **** Other Stuffs for now END

	void init(); ///< Initialization function for CMedit Object.

};


// ***** CMedit Class Definition END

#endif /*_GVIEW_H*/