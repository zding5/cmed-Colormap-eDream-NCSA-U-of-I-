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
}

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

};


// ***** CMedit Class Definition END










#endif /*_GVIEW_H*/