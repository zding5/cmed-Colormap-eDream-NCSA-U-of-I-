#ifndef _CMED_H
#define _CMED_H

#ifdef _WIN32
# include "../src/winjunk.h"
#endif

#include "cmedpanel.H"
#include "CMedit.H"
#include "colorpatch.H"

//FOR TESTING AREA
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_File_Icon.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNM_Image.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>



//END FOR TESTING AREA



extern struct _cshow {
    colorpatch* color;
    Fl_Output* hsba;
    Fl_Output* rgba;
    Fl_Value_Input* cindex;
    Fl_Box *hsblbls[3];
    Fl_Value_Input *postscalein;
    Fl_Value_Input *postexponin;
    Fl_Value_Input *cmentin;
    Fl_Output* scaleout;
} cshow;

#endif /*_CMED_H*/

