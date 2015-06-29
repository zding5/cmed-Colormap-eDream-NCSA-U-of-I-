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



	// **** I/O Stuffs END


	// **** Mouse/Keyboard Handling

	float drag( int index, enum CMfield field, float value, float lerp = 1.0 );
	void dragrange( int x0, int x1, enum CMfield field, float v0, float v1, float lerp = 1.0 ); //what for?


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

	int cment() const { return cment_; }
	void cment( int newcment );

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
	float hueshift, huezoom;

	char **comments;
	int ncomments, maxcomments;


	// **** Coordinates Parameters
	int cment_; ///< Number of colormap entries
	float hist_data_x_min_;
	float hist_data_x_max_;
	float hist_data_y_min_;
	float hist_data_y_max_;

	float data_x_min_for_cmap_;
	float data_x_max_for_cmap_;
	float data_y_min_for_cmap_;
	float data_y_max_for_cmap_;


	float data_x_min_for_display_;
	float data_x_max_for_display_;

	float data_y_min_for_hist_display_;
	float data_y_max_for_hist_display_;
	float data_y_min_for_cmap_display_;
	float data_y_max_for_cmap_display_;

	// **** Coordinates Parameters END


	// **** Coordinates Parameters END

	// **** Coordinates Transformations


	// **** Coordinates Transformations

	//windows to drawing
	float wx2drx( int wx ); ///< Window coord x to drawing coord x.
	float wy2dry( int wy ); ///< Window coord y to drawing coord y.

	//data to drawing
	float dtx2drx( float dtx ); ///< Data(Histogram) coord x to drawing coord x.
	float dty2dry( float dty ); ///< Data(Histogram) coord y to drawing coord y.

	//drawing to data
	float drx2dtx( float drx ); ///< Drawing coord x to data(histogram) coord x.


	//histogram to data
	float dispx2dtx( int dispx ); ///< Histogram coord(Histogram Display) x to data coord x.
	float h_dispy2dty( int h_dispy ); ///< Histogram coord(Histogram Display) y to data coord y.
	float c_dispy2dty( int c_dispy );


	//colormap to data
	float cmapx2dtx( int cmapx ); ///< Cmap coord x to data coord x.
	// float cmapy2dty( int cmapy ); ///< Cmap coord y to data coord y.

	//data to colormap
	int dtx2cmapx( float dtx ); ///< Data(Histogram) coord x to cmap coord x.

	//data to histogram
	// int dtx2dispx( float dtx );	///< Data(Histogram) coord x to histogram(Histogram Display) coord x.

	float hist_display_y_scaler( float dty ); ///< Scaler for histogram display in y axis.


	// **** Coordinates Transformations END


	// **** Other Stuffs for now

	int data_point_num_;
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


	// **** Other Stuffs for now END

	void init(); ///< Initialization function for CMedit Object.
};


// ***** CMedit Class Definition END

#endif /*_GVIEW_H*/