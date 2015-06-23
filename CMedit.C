#undef isspace //????

#include "CMedit.H"
#include "colorpatch.H"

#ifdef __APPLE__ //????
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


CMedit::CMedit(int x, int y, int w, int h, const char *label)
:Fl_Gl_Window(x,y,w,h,label) {
	init();
	end();
}

