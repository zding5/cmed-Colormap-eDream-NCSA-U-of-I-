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
	int nix;
	int ix; // ix is the current colormap entry index we are trying to write to.
	int ox; // ox is the current colormap entry index we are using to output ???
	int prevox; // prevox is the previous ox ???
	char tc[2]; //for storing color temporarily for specially formatted cmap entry. "No. of entry : RGB"
	int count = -1;

	while(getline(&line, &line_size, inf) != -1) {
		line_number++;
		for(buf1 = line; *buf1 && isspace(*buf1); buf1++){
			//skip potential spaces
		}
		if(*buf1 == '\0' || *buf1 == '\n'){
			//skip potential empty lines
			continue;
		}

		//check for colormap range flags
		buf2 = strstr(buf1, "#cmAppXMin");
		if( buf2 != NULL ) {
			if(0==strncmp(buf2, "#cmAppXMin", 10)) {
				*buf2 = '\0';
				buf2 += 10;
				if(*buf2 == '=' || *buf2 == ':') {
					buf2++;
				}
				sscanf(buf2, "%f", &data_x_min_for_cmap_);
				buf1 = buf2; //how would this prevent recording this as a comment???
			}
		}

		buf2 = strstr(buf1, "#cmAppXMax");
		if( buf2 != NULL ) {
			if(0==strncmp(buf2, "#cmAppXMax", 10)) {
				*buf2 = '\0';
				buf2 += 10;
				if(*buf2 == '=' || *buf2 == ':') {
					buf2++;
				}
				sscanf(buf2, "%f", &data_x_max_for_cmap_);
				buf1 = buf2;
			}
		}

		//load in comments for future saving
		if(*buf1 == '#') {
			if(ncomments >= maxcomments) {
				maxcomments *= 2;
				comments = (char **)realloc( comments, maxcomments * sizeof(char *) );
			}
			comments[ncomments++] = strdup( line );
			continue;
		}

		/* ``nnn:'' entries set the colormap pointer ... */
		/* This is for special format, "Colormap entry : RGB". With this we can skip colormap entries
		without specifying color in cmap file (those will be left with default color setting). */
	 	if(sscanf(line, "%d%1[:]", &nix, tc) == 2) { //What is in the sscanf , "%1[:]" ???
	 		//Why using nix ???
			ix = nix;
			buf1 = strchr(line, ':') + 1;
			while(*buf1 && isspace(*buf1)) {
				buf1++;
			}
			if(*buf1 == '\0' || *buf1 == '\n' || *buf1 == '#')
		 	continue;
		}

		if(count == -1) {
			if(!sscanf(line, "%d", &count) || count < 1) { //Specified numbe of lines in cmap file
				// Don't we want a flag for this ???
				fprintf(stderr, "Not a .cmap file?  Doesn't begin with a number.\n");
				return 0;
			}
			cment( count );
			ix = 0, prevox = 0;
			continue;
		}
		else {
			if(ix >= count) {
				break;
			}

			rgba[3] = 1;
			if(sscanf(buf1, "%f%f%f%f", &rgba[0],&rgba[1],&rgba[2],&rgba[3]) < 3) {
				fprintf(stderr, "Couldn't read colormap line %d (cmap entry %d of 0..%d)\n",
				line_number, ix, count-1);
				return 0;
			}
			rgb2hsb( rgba[0],rgba[1],rgba[2], &hsba[0],&hsba[1],&hsba[2] );
			hsba[3] = rgba[3];//WHAT ???
			if(ox == 0) {
				memcpy(phsba, hsba, sizeof(phsba));
			}
			else {
				phsba[0] = huenear(phsba[0], hsba[0]);
			}
			ox = (cment_-1) * ix / count + 1;
			for(f = 0; f < 4; f++) {
				dragrange( prevox, ox, flds[f], phsba[f], hsba[f], 1.0 );
				phsba[f] = hsba[f];
			}
			prevox = ox;
			ix++;
		}
	}

	if(count <= 0) {
		fprintf(stderr, "Empty colormap file?\n");
		return 0;
	}
	
	if(ix < count) {
		fprintf(stderr, "Only got %d colormap entries, expected %d\n", ix, count);
	}
	
	for(f = 0; f < 4; f++) {
		dragrange( prevox, cment_, flds[f], phsba[f], phsba[f], 1.0 );
	}

	return ix > 0;

}

int cmap_fsave( FILE *outf ) {
	float r,g,b;
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