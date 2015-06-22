#ifndef _GVIEW_H
#define _GVIEW_H
/*
 * Graphical colormap editor
 */

#include <stdlib.h>
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <string>

// ***** Constants Definitions

#define CMENTMAX  16384

extern "C" {
	extern void hsb2rgb(float h, float s, float b, float *rp, float *gp, float *bp);
    extern void rgb2hsb(float r, float g, float b,  float *hp, float *sp, float *bp);
} // To be rewritten as C++ functions

enum CMfield {
    HUE, SAT, BRIGHT,
    ALPHA,
    RED, GREEN, BLUE
};

// ***** Constants Definitions END


// ***** CMedit Class Definition

class CMedit : public Fl_Gl_Window {

public:
    
    // **** Constructors
    	//taking in (x,y) as left-most point(?), and h as height, w as width of the drawing area.
        CMedit(int x, int y, int h, int w, const char *label = 0);
    // **** Constructors END


    // **** Getters and Setters
        float get_data_x_min_for_display();
		float get_data_x_max_for_display();
		float get_hist_data_x_min();
		float get_hist_data_x_max();
		float get_data_x_min_for_cmap();
		float get_data_x_max_for_cmap();

		void set_data_x_min_for_display(float val);
		void set_data_x_max_for_display(float val);
		void set_hist_data_x_min(float val);
		void set_hist_data_x_max(float val);
		void set_data_x_min_for_cmap(float val);
		void set_data_x_max_for_cmap(float val);

		float get_data_y_min_for_display();
		float get_data_y_max_for_display();
		float get_hist_data_y_min();
		float get_hist_data_y_max();
		float get_data_y_min_for_cmap();
		float get_data_y_max_for_cmap();

		void set_data_y_min_for_display(float val);
		void set_data_y_max_for_display(float val);
		void set_hist_data_y_min(float val);
		void set_hist_data_y_max(float val);
		void set_data_y_min_for_cmap(float val);
		void set_data_y_max_for_cmap(float val);
    // **** Getters and Setters END


	// **** I/O Stuffs

		int cmap_fload( FILE *inf ); // previously fload
		int cmap_fsave( FILE *outf ); // previously fsave
		int hist_fload( FILE *inf );

		virtual void draw(); //must be called draw()
		virtual void handle(int ev); //have to be called handle???
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
	
	// **** Reporters (For updating cmed class objects)

};


// ***** CMedit Class Definition END










#endif /*_GVIEW_H*/