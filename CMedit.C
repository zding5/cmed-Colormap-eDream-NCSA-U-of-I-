#undef isspace //????

#include "CMedit.H"
#include "colorpatch.H"

#ifdef __APPLE__ //????
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


// ***** Constants

#define  DTHIST_XMIN  hist_data_x_min_
#define  DTHIST_XMAX  hist_data_x_max_
#define  DTHIST_XW    (DTHIST_XMAX-DTHIST_XMIN)
#define  DTHIST_YMIN  hist_data_y_min_
#define  DTHIST_YMAX  hist_data_y_max_
#define  DTHIST_YH    (DTHIST_YMAX-DTHIST_YMAX)


#define  CMAP_XMIN  0
#define  CMAP_XMAX  cment_
#define  CMAP_XW     (CMAP_XMAX-CMAP_XMIN)
#define  CMAP_YMIN  0
#define  CMAP_YMAX  cmap_y_max_
#define  CMAP_YH    (CMAP_YMAX-CMAP_YMIN)


#define  CMAPAPP_XMIN  data_x_min_for_cmap_
#define  CMAPAPP_XMAX  data_x_max_for_cmap_
#define  CMAPAPP_XW    (CMAPAPP_XMAX-CMAPAPP_XMIN)


#define  DR_XMIN    (-1/16.f)
#define  DR_XMAX    (1.0)
#define  DR_XW      (DR_XMAX-DR_XMIN)
#define  DR_XZERO   (0.0)
#define  DR_XFROM0  (DR_XMAX-DR_XZERO)  
//Y
#define  DR_YMIN    (-0.25)
#define  DR_YMAX    (1.0)
#define  DR_YH      (DR_YMAX-DR_YMIN)
#define  DR_Y0      (0.0)
#define  DR_YFROM0  (DR_YMAX-DR_Y0)
#define  YBAR0  	(-.01)
#define  YBAR1  	(-.06)


#define  DISP_XMIN    data_x_min_for_display_
#define  DISP_XMAX    data_x_max_for_display_
#define  DISP_XW      (DISP_XMAX-DISP_XMIN)


#define  HDISP_YMIN  data_y_min_for_hist_display_
#define  HDISP_YMAX  data_y_max_for_hist_display_
#define  HDISP_YH    (HDISP_YMAX-HDISP_YMIN)

#define  CDISP_YMIN  data_y_min_for_cmap_display_
#define  CDISP_YMAX  data_y_max_for_cmap_display_
#define  CDISP_YH    (CDISP_YMAX-CDISP_YMIN)
// ***** Constants END


// ***** Coordinates Conversion

//windows to drawing
float CMedit::wx2drx( int wx ) {//pixel coordinate comes in as int
  return (DR_XMIN + wx * ( DR_XW ) / w());
}
float CMedit::wy2dry( int wy ) {//pixel coordinate comes in as int
  return (DR_YMIN + wy * ( DR_YH ) / h());
}

//data to drawing
float CMedit::dtx2drx( float dtx ) {
  return (( dtx - DTHIST_XMIN ) * ( DR_XFROM0 ) / DTHIST_XW);
}
float CMedit::dty2dry( float dty ) {
  return (( dty - DTHIST_YMIN ) * ( DR_YFROM0 ) / DTHIST_YH);
}

//drawing to data
float CMedit::drx2dtx( float drx ) {
  return (DTHIST_XMIN + (drx) * ( DTHIST_XW ) / DR_XFROM0);
}

//colormap apply to data
float CMedit::cmapx2dtx( int cmapx ) {
	return (CMAPAPP_XMIN + (cmapx - CMAPAPP_XMIN) * ( CMAPAPP_XW ) / DTHIST_XW)
}

float CMedit::dispx2dtx( int dispx ) {
  return (DTHIST_XMIN + ( dispx - DISP_XMIN ) * ( DTHIST_XW ) / DISP_XMIN);
}
float CMedit::h_dispy2dty( int h_dispy ) {
	return (HDISP_YMIN + ( h_dispy - HDISP_YMIN ) * ( DTHIST_YH ) / HDISP_YH);
}
float CMedit::c_dispy2dty( int c_dispy ) {
	return (CDISP_YMIN + ( c_dispy - CDISP_YMIN ) * ( DTHIST_YH ) / CDISP_YH );
}

//data to colormap
int CMedit::dtx2cmapx( float dtx ) {
  if (dtx < CMAPAPP_XMIN) {
    return CMAP_XMIN-1;
  }
  else if (dtx > CMAPAPP_XMAX) {
    return CMAP_XMAX+1;
  }
  else {
    int cmap_ind = (int) ( CMAP_XMIN + (dtx - CMAPAPP_XMIN) * ( CMAP_XW ) / CMAPAPP_XW);
    return cmap_ind;
  }
}


//Histogram y scaler. Will have more options later on I think
float CMedit::hist_y_scaler( float dty ) {
	return dty2dry(dty);
} 

// ***** Coordinates Conversion END

CMedit::CMedit(int x, int y, int w, int h, const char *label)
:Fl_Gl_Window(x,y,w,h,label) {
	init();
	end();
}

// **** Getters and Setters
float CMedit::get_hist_data_x_min() {
	return hist_data_x_min_;
}
float CMedit::get_hist_data_x_max() {
	return hist_data_x_max_;
}
float CMedit::get_hist_data_y_min() {
	return hist_data_y_min_;
}
float CMedit::get_hist_data_y_max() {
	return hist_data_y_max_;
}

float CMedit::get_data_x_min_for_cmap() {
	return data_x_min_for_cmap_;
}
float CMedit::get_data_x_max_for_cmap() {
	return data_x_max_for_cmap_;
}
float CMedit::get_data_y_min_for_cmap() {
	return data_y_min_for_cmap_;
}
float CMedit::get_data_y_max_for_cmap() {
	return data_y_max_for_cmap_;
}

float CMedit::get_data_x_min_for_display() {
	return data_x_min_for_display_;
}
float CMedit::get_data_x_max_for_display() {
	return data_x_max_for_display_;
}
float CMedit::get_data_y_min_for_hist_display() {
	return data_y_min_for_hist_display_;
}
float CMedit::get_data_y_max_for_hist_display() {
	return data_y_max_for_hist_display_;
}
float CMedit::get_data_y_min_for_cmap_display() {
	return data_y_min_for_cmap_display_;
}
float CMedit::get_data_y_max_for_cmap_display() {
	return data_y_max_for_cmap_display_;
}

void CMedit::set_hist_data_x_min(float val) {
	hist_data_x_min_ = val;
}
void CMedit::set_hist_data_x_max(float val) {
	hist_data_x_max_ = val;
}
void CMedit::set_hist_data_y_min(float val) {
	hist_data_y_min_ = val;
}
void CMedit::set_hist_data_y_max(float val) {
	hist_data_y_max_ = val;
}

void CMedit::set_data_x_min_for_cmap(float val) {
	data_x_min_for_cmap_ = val;
}
void CMedit::set_data_x_max_for_cmap(float val) {
	data_x_max_for_cmap_ = val;
}
void CMedit::set_data_y_min_for_cmap(float val) {
	data_y_min_for_cmap_ = val;
}
void CMedit::set_data_y_max_for_cmap(float val) {
	data_y_max_for_cmap_ = val;
}


void CMedit::set_data_x_min_for_display(float val) {
	data_x_min_for_display_ = val;
}
void CMedit::set_data_x_max_for_display(float val) {
	data_x_max_for_display_ = val;
}
void CMedit::set_data_y_min_for_hist_display(float val) {
	data_y_min_for_hist_display_ = val;
}
void CMedit::set_data_y_max_for_hist_display(float val) {
	data_y_max_for_hist_display_ = val;
}
void CMedit::set_data_y_min_for_cmap_display(float val) {
	data_y_min_for_cmap_display_ = val;
}
void CMedit::set_data_y_max_for_cmap_display(float val) {
	data_y_max_for_cmap_display_ = val;
}

// **** Getters and Setters END


// **** I/O Stuffs

int cmap_fload( FILE *inf ) {
	char* line;
	size_t line_size;

	float rgba[4], hsba[4], phsba[4];
	static enum CMfield flds[4] = { HUE, SAT, BRIGHT, ALPHA };

	int line_number;
	char* buf1, buf2;

	while(getline(&line, &line_size, inf) != -1) {
		line_number++;
		for(buf1 = line; *buf1 && isspace(*buf1); buf1++){
			//skip potential spaces
		}
		if(*buf1 == '\0' || *buf1 == '\n'){
			//skip potential empty lines
			continue;
		}
	}
}

int cmap_fsave( FILE *outf ) {
	return 0;
}

int hist_fload( FILE *inf ) {
	return 0;
}

virtual void draw() {

}
virtual int handle(int ev) {

}
virtual void resize(int nx, int ny, int nw, int nh) {

}



// **** I/O Stuffs END