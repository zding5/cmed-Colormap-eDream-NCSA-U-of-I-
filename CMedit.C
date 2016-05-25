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
#define  DTHIST_YH    (DTHIST_YMAX-DTHIST_YMIN)


#define  CMAP_XMIN  0
#define  CMAP_XMAX  cmapw()
#define  CMAP_XW    (CMAP_XMAX - CMAP_XMIN)

#define  CMAP_YMIN  0
#define  CMAP_YMAX  cmap_y_max_
#define  CMAP_YH    (CMAP_YMAX-CMAP_YMIN)

// The range in data coord for which the cmap is applied to
#define  CMAPAPP_XMIN  data_x_min_for_cmap_
#define  CMAPAPP_XMAX  data_x_max_for_cmap_
#define  CMAPAPP_XW    (CMAPAPP_XMAX-CMAPAPP_XMIN)


#define  DR_XMIN    (-1/16.f)
#define  DR_XMAX    (1.0)
// #define  DR_XMAX    w() + DR_XMIN
#define  DR_XW      (DR_XMAX-DR_XMIN)
#define  DR_X0   	(0.0)
#define  DR_XFROM0  (DR_XMAX-DR_X0)
//Y
#define  DR_YMIN    (-0.25)
#define  DR_YMAX    (1.0)
#define  DR_YH      (DR_YMAX-DR_YMIN)
#define  DR_Y0      (0.0)
#define  DR_YFROM0  (DR_YMAX-DR_Y0)
#define  YBAR0  	(-.005)
#define  YBAR1  	(-.015)
#define  YBAR2		(-.065)


#define  DISP_XMIN    data_x_min_for_display_
#define  DISP_XMAX    data_x_max_for_display_
#define  DISP_XW      (DISP_XMAX-DISP_XMIN)


#define  HDISP_YMIN  data_y_min_for_hist_display_
#define  HDISP_YMAX  data_y_max_for_hist_display_
#define  HDISP_YH    (HDISP_YMAX-HDISP_YMIN)

// Don't need
#define  CDISP_YMIN  data_y_min_for_cmap_display_
#define  CDISP_YMAX  data_y_max_for_cmap_display_
#define  CDISP_YH    (CDISP_YMAX-CDISP_YMIN)
// ***** Constants END


// ***** Coordinates Conversion
/* General Conversion:
	
	x2xx( x ) {
		return (xx_min + x *( xx_range ) / x_range)
	}

*/

int CMedit::cmapw() {
	return int((15/16.f)*w());
}

int CMedit::wx2cmapx( int wx ) {
	return wx - int((1/16.f)*w());
}

// windows to drawing
float CMedit::wx2drx( int wx ) {//pixel coordinate comes in as int
	// printf("%d %d\n", wx, w());
	return (DR_XMIN + wx * ( DR_XW ) / w());
	// return (wx + (-1/16.f)*w());
	// return wx;
}

float CMedit::wy2dry( int wy ) {//pixel coordinate comes in as int
  return (DR_YMAX - wy * ( DR_YH ) / h());
}

// data to drawing (Actually only converting the displaying part, between DISP_XMIN and DISP_XMAX, to drawing)
float CMedit::dtx2drx( float dtx ) {
  // return (( dtx - DTHIST_XMIN ) * ( DR_XFROM0 ) / DTHIST_XW);
	return ( 0 + ( dtx - DISP_XMIN ) * ( DR_XFROM0 ) / DISP_XW );
	// return ( 0 + ( dtx - DISP_XMIN ) * w()*15/16 / DISP_XW );

}// data to drawing (Actually only converting the displaying part, between HDISP_YMIN and HDISP_YMAX, to drawing)

float CMedit::hdty2dry( float hdty ) {
  return ( 0 + ( hdty - HDISP_YMIN ) * ( DR_YFROM0 ) / HDISP_YH );
}

float CMedit::cdty2dry( float cdty ) {
	return ( 0 + ( cdty - CDISP_YMIN ) * ( DR_YFROM0 ) / CDISP_YH );
}

// drawing to data ( Actually to data range for display, between DISP_XMIN and DISP_XMAX I think ... )
float CMedit::drx2dtx( float drx ) {
  return ( DISP_XMIN + (drx) * ( DISP_XW ) / DR_XFROM0 );
  // return ( DISP_XMIN + (drx) * ( DISP_XW ) / (w()*15/16) ); 
}

float CMedit::dry2hdty( float dry ) {
	return ( HDISP_YMIN + (dry) * ( HDISP_YH ) / DR_YFROM0 );
}

// colormap to data ( Actually the cmap entries to data range which the cmap apply to )
float CMedit::cmapx2dtx( int cmapx ) {
	return ( CMAPAPP_XMIN + (cmapx) * ( CMAPAPP_XW ) / cment_ );
}

// histogram to data ( hist entries to data range )
float CMedit::histx2dtx( int histx ) {
	return ( DTHIST_XMIN + ( histx ) * ( DTHIST_XW ) / hisent_ );
}

//data to colormap
int CMedit::dtx2cmapx( float dtx ) {
  if (dtx < CMAPAPP_XMIN) {
  // if (dtx < CMAP_XMIN) {
    return -1;
  }
  else if (dtx > CMAPAPP_XMAX) {
  // else if (dtx > CMAP_XMAX) {
    return cment_ + 1;
  }
  else {
    int cmap_ind = (int) ( 0 + (dtx - CMAP_XMIN) * ( CMAP_XMAX ) / CMAP_XW);
    return cmap_ind;
  }
}

static float sample( float *a, int ents, float at, int smooth ) {
// static float sample( colorEnt ce, int ents, float at, int smooth ) {
	if(at <= 0 || ents <= 1) return a[0];
	if(at >= 1) return a[ents-1];
	float eat = at * ents;
	int iat = (int)eat;
	return smooth ? a[iat]*(1 - (eat-iat)) + a[iat+1]*(eat-iat)
	: a[iat];
}


//Histogram y scaler. Will have more options later on I think
float CMedit::hist_y_scaler( float dty ) {
	return hdty2dry(dty);
} 

// ***** Coordinates Conversion END

CMedit::CMedit(int x, int y, int w, int h, const char *label)
:Fl_Gl_Window(x,y,w,h,label) {
	init();
	end();
}

// **** Getters and Setters
int CMedit::cment() const {
	return cment_;    
}

void CMedit::cment( int newcment ) { // Change cmap entry number
	if(newcment < 1) return;
	if(newcment > CMENTMAX) {
		fprintf(stderr, "Oops, can't ask for more than %d colormap entries -- using %d\n", CMENTMAX,CMENTMAX);
		newcment = CMENTMAX;
	}
	if(cment_ == newcment) return;
	/* Resample */
	// int temp_cment = cment_;
	snapshot();

	int smooth = 0; // (cment_ < newcment);
	// for(int o = 0; o < newcment; o++) {
	// 	float at = newcment > 1 ? (float)o / (newcment-1) : 0;
	// 	vh[o] = sample( &snap[0][0], cment_, at, smooth );
	// 	vs[o] = sample( &snap[1][0], cment_, at, smooth );
	// 	vb[o] = sample( &snap[2][0], cment_, at, smooth );
	// 	alpha[o] = sample( &snap[3][0], cment_, at, smooth );
	// }
	colorEnt last_snap = undo_stack.back();
	for(int o = 0; o < newcment; o++) {
		float at = newcment > 1 ? (float)o / (newcment-1) : 0;
		vh[o] = sample( last_snap.ent[0], cment_, at, smooth );
		vs[o] = sample( last_snap.ent[1], cment_, at, smooth );
		vb[o] = sample( last_snap.ent[2], cment_, at, smooth );
		alpha[o] = sample( last_snap.ent[3], cment_, at, smooth );
	}


	cment_ = newcment;
	remin = 0;
	remax = cment_ - 1;
	lockmin = 0;
	lockmax = cment_ - 1;
	if(cmentcb_ != 0) (*cmentcb_)( this );
	report( dragfrom );
	redraw();
}

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



void CMedit::set_editing_mode( int mode ) {
	editing_mode = mode;
}


void CMedit::display_reset() {
	data_x_min_for_display_ = data_x_min_for_display_ori_;
	data_x_max_for_display_ = data_x_max_for_display_ori_;
	data_y_max_for_hist_display_ = data_y_max_for_hist_display_ori_;
	data_y_min_for_hist_display_ = data_y_min_for_hist_display_ori_;
}

// **** Getters and Setters END


// WHAT ???


// **** I/O Stuffs

int CMedit::cmap_fload( FILE *inf ) {
	char* line = NULL;
	size_t line_size;

	float rgba[4], hsba[4], phsba[4];
	static enum CMfield flds[4] = { HUE, SAT, BRIGHT, ALPHA };

	int line_number = 0;
	char *buf1, *buf2;
	int nix;
	int ix; // ix is the current colormap entry index we are trying to write to.
	int ox = 0; // ox is the current colormap entry index we are using to output ???
	int prevox; // prevox is the previous ox ???
	char tc[2]; //for storing color temporarily for specially formatted cmap entry. "No. of entry : RGB"
	int count = -1;

	int f;

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
			printf("%s %d\n","ix: ", ix);
		}
	}

	if(count <= 0) {
		fprintf(stderr, "Empty colormap file?\n");
		return 0;
	}
	printf("%d\n", line_number);
	printf("%d\n", ix);
	if(ix < count) {
		fprintf(stderr, "Only got %d colormap entries, expected %d\n", ix, count);
	}
	
	for(f = 0; f < 4; f++) {
		dragrange( prevox, cment_, flds[f], phsba[f], phsba[f], 1.0 );
	}

	return ix > 0;

}

int CMedit::cmap_fsave( FILE *outf ) {
	float r,g,b;
	int i, ok = 1;

	fprintf(outf, "%s %d\n", "#cmAppXMin", CMAPAPP_XMIN);
	fprintf(outf, "%s %d\n", "#cmAppXMax", CMAPAPP_XMAX);
	fprintf(outf, "%d\n", cment_);
	for(i = 0; i < cment_; i++) {
		hsb2rgb( vh[i], vs[i], vb[i], &r, &g, &b );
		// if(fprintf(outf, "%f %f %f %f\n", r, g, b, Aout( alpha[i] )) <= 0)
		if(fprintf(outf, "%f %f %f %f\n", r, g, b, alpha[i] ) <= 0)

		ok = 0;
	}
	if(ncomments > 0) {
		fputs("\n", outf);
	}
	for(i = 0; i < ncomments; i++) {
		fputs(comments[i], outf);
	}
	return ok;
}

int CMedit::hist_fload( FILE *inf ) {
	char* line = NULL;
	size_t line_size;
	char *flag_pointer = NULL;
	int entry_counter = 0;
	float datamax = -1;

	getline(&line, &line_size, inf);

	getline(&line, &line_size, inf);
	hist_data_x_min_ = atoi(strtok(line, " "));
	hist_data_x_max_ = atoi(strtok(NULL, " "));

	data_x_max_for_cmap_ = hist_data_x_max_;
	data_x_max_for_display_ = hist_data_x_max_;
	data_x_max_for_display_ori_ = data_x_max_for_display_;

	getline(&line, &line_size, inf);
	hisent_ = atoi(line);
	printf("%d\n", hisent_);

	hist_ent_arr = (float*) malloc(hisent_ * sizeof(float));

	for (int i=0; i<hisent_; i++) {
    	getline(&line, &line_size, inf);
		strtok(line, " ");
		hist_ent_arr[i] = atof(strtok(NULL, " "));
		//printf("%f", hist_ent_arr[i]);
		datamax = (hist_ent_arr[i]>datamax)?hist_ent_arr[i]:datamax;
	}

	hist_data_y_max_ = datamax*1.1;
	data_y_max_for_cmap_ = hist_data_y_max_;
	data_y_max_for_hist_display_ = hist_data_y_max_;
	data_y_max_for_hist_display_ori_ = data_y_max_for_hist_display_;
	printf("datamax: %d\n", datamax);

	updaterange();
	redraw();
}

// **** I/O Stuffs END

// **** Virtual Funcs: draw(), resize() and handle()
void CMedit::draw() {

	int i;
	float v;
	int coarse = 1;

	if(!valid() || damage()) {
	// not valid??? or damage (means the window is damaged for some reason, say overlapped by another window)
		valid(1);
		remin = DR_XMIN; // ???
		remax = DR_XMAX;

		glViewport( 0, 0, w(), h() );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		glOrtho( DR_XMIN, DR_XMAX, DR_YMIN, DR_YMAX, -1, 1 );
		// Might have to change for y axis

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

	}

	glClearColor( 0,0,0,0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_BLEND );

	glScissor( (w()/17.0), (h()*0.25/1.25), (16*w()/17.0), (h()/1.25) );


	//***histogram part=========================
	if(hist_ent_arr!=NULL) {

		glEnable( GL_SCISSOR_TEST );
		glBegin(GL_QUADS);
		for (i = 0; i<=hisent_; i++) {
			//printf("%d, %f, %f\n",i, histx2dtx(i), dtx2drx(i) );

// 
			glColor3f( 0,0,0 );
			glVertex2f( dtx2drx(histx2dtx(i)), 0.0 );
			glVertex2f( dtx2drx(histx2dtx(i)), hdty2dry(hist_ent_arr[i]) );
			glVertex2f( dtx2drx(histx2dtx(i+1)), hdty2dry(hist_ent_arr[i]) );
			glVertex2f( dtx2drx(histx2dtx(i+1)), 0.0 );

		}
		glEnd();

		glBegin(GL_QUADS);
		for (i = 0; i <= hisent_; i++) {
			float rgb[3];
			float j = histx2dtx(i);
			float k = histx2dtx(i+1);

			if (j >= CMAPAPP_XMIN && j <= CMAPAPP_XMAX){

				hsb2rgb(vh[ dtx2cmapx(histx2dtx(i)) ], vs[ dtx2cmapx(histx2dtx(i)) ], vb[ dtx2cmapx(histx2dtx(i)) ], &rgb[0], &rgb[1], &rgb[2]);
			}

			else {
				rgb[0] = 125;
				rgb[1] = 125;
				rgb[2] = 125;
			}

			glColor3fv( rgb );
			glVertex2f( dtx2drx(histx2dtx(i)), 0.0 );
			glVertex2f( dtx2drx(histx2dtx(i)), hdty2dry(hist_ent_arr[i]) );
			glVertex2f( dtx2drx(histx2dtx(i+1)), hdty2dry(hist_ent_arr[i]) );
			glVertex2f( dtx2drx(histx2dtx(i+1)), 0.0 );
		}
		glEnd();

		glDisable( GL_SCISSOR_TEST );


		glBegin(GL_QUADS);
		for (i = 0; i <= hisent_; i++) {
			glColor3f( 1,1,1 );
			glVertex2f( dtx2drx(histx2dtx(i)), YBAR0 );
			glVertex2f( dtx2drx(histx2dtx(i)), 0.0 );
			glVertex2f( dtx2drx(histx2dtx(i+1)), 0.0 );
			glVertex2f( dtx2drx(histx2dtx(i+1)), YBAR0 );
			// This is supposed to draw thin strips indicating where there are data there.
		}


		// glBegin(GL_QUADS);
		// glColor3f( 0,0,0 );
		// glVertex2f( DR_XMIN, YBAR0 );
		// glVertex2f( DR_XMIN, DR_YMIN );
		// glVertex2f( DR_XMAX, DR_YMIN );
		// glVertex2f( DR_XMAX, YBAR0 );
		// glEnd();

	}

	//*** histogram part END =========================

	//*** Cmap part===============================

	glBegin( GL_QUADS );
	glColor3f( 0,0,0 );
	glVertex2f( remin, -.05 );
	glVertex2f( remax+1, -.05 );
	glColor3f( 1,1,1 );

	glVertex2f( remax+1, DR_YMIN );
	glVertex2f( remin, DR_YMIN );

	glEnd();

	glLineWidth( 1 );
	glDisable( GL_BLEND );
	
	glLineWidth( 1 );
	glDisable( GL_BLEND );

	if(hsbmode) {
		glColor3f( 1,1,0 ); /* Hue: yellow */
		float midhue = y2hue(.5);
		glBegin( GL_LINE_STRIP );
		// for(i = 0; i <= cment_-1; i++) {
		for(i = 0; i <= cmapw()-1; i++) {
			v = hue2y( huenear( vh[i], midhue ) ); //Need to worry about vh stuffs???
			glVertex2f( dtx2drx(cmapx2dtx(i)),  v);
	 		if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), v );
		}
		glEnd();

		glColor3f( .3,1,.25 );  /* Saturation: teal */
		glBegin( GL_LINE_STRIP );
		// for(i = 0; i <= cment_-1; i++) {
		for(i = 0; i <= cmapw()-1; i++) {
			// printf("%d\n", i);
			glVertex2f( dtx2drx(cmapx2dtx(i)), vs[i] );
			if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), vs[i] );
		}
		glEnd();

		glColor3f( .5,.2,1 );  /* Brightness: purple */
		glBegin( GL_LINE_STRIP );
		// for(i = 0; i <= cment_-1; i++) {
		for(i = 0; i <= cmapw()-1; i++) {
			glVertex2f( dtx2drx(cmapx2dtx(i)), vb[i] );
			if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), vb[i] );
		}
		glEnd();
	}	
  
	else {
		float r[CMENTMAX], g[CMENTMAX], b[CMENTMAX];

		// for(i = 0; i <= cment_-1; i++) {
		for(i = 0; i <= cmapw()-1; i++) {
			hsb2rgb( vh[i], vs[i], vb[i], &r[i], &g[i], &b[i] );    
		}

		glColor3f( 1,0,0 ); /* red */
		glBegin( GL_LINE_STRIP );
		for(i = 0; i <= cment_-1; i++) {
			glVertex2f( dtx2drx(cmapx2dtx(i)), r[i] );
			if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), r[i] );
			printf("%d | %f | %f\n", i, cmapx2dtx(i+1), dtx2drx(cmapx2dtx(i+1)));
		}
		glEnd();

		glColor3f( 0,1,0 );  /* green */
		glBegin( GL_LINE_STRIP );
		// for(i = 0; i <= cment_-1; i++) {
		for(i = 0; i <= cmapw()-1; i++) {
			glVertex2f( dtx2drx(cmapx2dtx(i)), g[i] );
			if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), g[i] );
		}
		glEnd();

		glColor3f( 0,0,1 );  /* blue */
		glBegin( GL_LINE_STRIP );
		for(i = 0; i <= cment_-1; i++) {
			glVertex2f( dtx2drx(cmapx2dtx(i)), b[i] );
			if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), b[i] );
		}
		glEnd();
	}

	glColor3f( .7,.7,.7 );  /* alpha: gray */
	glBegin( GL_LINE_STRIP );
	for(i = 0; i <= cment_-1; i++) {
		glVertex2f( dtx2drx(cmapx2dtx(i)), alpha[i] );
		if(coarse) {
			glVertex2f( dtx2drx(cmapx2dtx(i+1)), alpha[i] );
		}
	}
	glEnd();

	glDisable( GL_BLEND );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_QUAD_STRIP );  //upper bottom strips
	for(i = 0; i <= cment_-1; i++) {
		float rgb[3];
		hsb2rgb( vh[i], vs[i], vb[i], &rgb[0],&rgb[1],&rgb[2] );
		glColor3fv( rgb );
		glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR1 );
		glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR2 );
		if(coarse) {
			glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR1 );
			glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR2 );
		}
	}
	glEnd();

	glEnable( GL_BLEND ); //This is how you can draw layers on layers
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glShadeModel( GL_SMOOTH );
	glBegin( GL_QUAD_STRIP );  //lower bottom strips
	for(i = 0; i <= cment_-1; i++) {
		float rgba[4];
		hsb2rgb( vh[i], vs[i], vb[i], &rgba[0],&rgba[1],&rgba[2] );
		rgba[3] = alpha[i];
		glColor4fv( rgba );
		glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR2 );
		glVertex2f( dtx2drx(cmapx2dtx(i)), DR_YMIN );
		if(coarse) {
			glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR2 );
			glVertex2f( dtx2drx(cmapx2dtx(i+1)), DR_YMIN );
		}
	}
	glEnd();
	glDisable( GL_BLEND );


	//This is the left side strip
	glBegin( GL_QUAD_STRIP );
	for(i = 0; i < 128; i++) {
		float rgb[3];
		float y = i / 127.;
		hsb2rgb( y2hue(y), 1, 1, &rgb[0],&rgb[1],&rgb[2] );
		glColor3fv( rgb );
		glVertex2f( DR_XMIN , y );
		glVertex2f( DR_XMIN/4, y );
	}
	glEnd();
	glFinish();

	//*** Cmap part END ===============================

}

void CMedit::resize(int nx, int ny, int nw, int nh) {
	w(nw);
	h(nh);
	// cment_ = snapcment_ = cmapw();
	// Need to resample....????

	cment(cmapw());
	hide();
	show();
}

int CMedit::handle(int ev) {

	if(editing_mode == 1) {
		return handle_drawing(ev);
	}

	else if(editing_mode == 2) {
		return handle_zooming(ev);
	}

	else {
		return handle_interpolation(ev);
	}

}

int CMedit::handle_drawing(int ev) {

	// int x = dtx2cmapx(drx2dtx(wx2drx( Fl::event_x() )));
	int x = wx2cmapx( Fl::event_x() );
	// x value in data coord
	float y = wy2dry( Fl::event_y() );
	// y value in drawing coord
	int xmin, xmax;

	switch(ev) {
		case FL_SHORTCUT: // undo
		if(Fl::event_key() == 'u') {
			undo();
			return 1;
		}
		return 0;

		case FL_PUSH: //
		dragfrom = x;
		dragval = y;
		dragamount = 0;

		if(Fl::event_state(FL_SHIFT)) {
			dragfield = ALPHA;
		}
		else {
		// Which button?  Take account of alt/meta modifiers too.
			int btn = 1;
			if(Fl::event_state(FL_BUTTON2 | FL_ALT)) {
				btn = 2;
			}
			else if(Fl::event_state(FL_BUTTON3 | FL_META)) {
				btn = 3;
			}
			static CMfield btn2field[2][3] = {
				{ RED, GREEN, BLUE },
				{ HUE, SAT, BRIGHT }
			};
			dragfield = btn2field[ hsbmode ][ btn-1 ];
		}

		if(Fl::event_key('l')) {
		/*...*/
		}
		else if(Fl::event_key('r')) {
		/*...*/
		}
		else {
			snapshot();
		}
		// What is going on here ???
		return 1;



		case FL_DRAG:
		case FL_RELEASE:



		#ifdef NOTYET
		if(draghue) {
			float h0 = y2hue( y );
			hueshift += (y - dragval) / huezoom;
			huezoom *= (x - dragfrom)
		}
		else {
			//
		}
		#endif	
		// What is this ???

		dragrange( dragfrom, x, dragfield, dragval, y,
		Fl::event_state(FL_CTRL) ? 1.0 : lerpval );
		xmin = (dragfrom<x) ? dragfrom : x;
		xmax = dragfrom+x-xmin;
		if(damage() == 0) {
			remin = xmin, remax = xmax;
		} else {
			if(remin > xmin) remin = xmin;
			if(remax < xmax) remax = xmax;
		}
		damage(1);
		if(dragfrom != x)
			dragamount = 1;
		if(ev == FL_RELEASE && dragamount == 0)
			drag_drawing( x, dragfield, y, Fl::event_state(FL_CTRL) ? 1.0 : lerpval );

		dragfrom = x, dragval = y;
		report( x );
		return 1;

		case FL_ENTER:
		case FL_LEAVE:
		dragfrom = x;
		report( x );
		return 1;

		case FL_MOVE:
		dragfrom = x;
		report( x );
		return 1;
	}
	return 0;
}

int CMedit::handle_zooming(int ev) {
	
	float x = wx2drx( Fl::event_x() );
	float y = wy2dry( Fl::event_y() );
	int btn = 1;

	switch(ev) {
		case FL_PUSH:
		dragfrom_x = x;
		dragfrom_y = y;

		// dragamount = 0;

		btn = 1; // Left click
		mouse_btn = btn;
		if(Fl::event_state(FL_BUTTON3 | FL_META)) {
			btn = 3; // Right click
			mouse_btn = btn;
		}

		case FL_DRAG:
		case FL_RELEASE:
		// printf("%d \n", btn);
		if (mouse_btn == 1) {
			fluid_moving(dragfrom_x, x, dragfrom_y, y);
			dragfrom_x = x;
			dragfrom_y = y;
			return 1;
		}

		else if (mouse_btn == 3) {
			fluid_zooming(dragfrom_x, x, dragfrom_y, y);
			dragfrom_x = x;
			dragfrom_y = y;
			return 1;
		}
	}
	return 0;

}


void CMedit::fluid_moving( float x0, float x1, float y0, float y1 ) {
	// printf("%s\n", "zooming !!");
	float change_x = (drx2dtx(x0) - drx2dtx(x1));
	float change_y = (dry2hdty(y0) - dry2hdty(y1));

	// printf("%f, %f, %f\n", drx2dtx(x0), drx2dtx(x1), change_x);
	data_x_min_for_display_ += change_x;
	data_x_max_for_display_ += change_x;

	// Guarantee that the bottom of the histograms does not go higher than needed (>0)
	if (hist_data_y_min_ < data_y_min_for_hist_display_ + change_y) { 
		data_y_min_for_hist_display_ += change_y;
		data_y_max_for_hist_display_ += change_y;
	}
	else {
		change_y = (hist_data_y_min_ - data_y_min_for_hist_display_);
		data_y_min_for_hist_display_ += change_y;
		data_y_max_for_hist_display_ += change_y;
	}

	printf("data x for display: %f, %f\n", data_x_min_for_display_, data_x_max_for_display_);
	
	updaterange();
	redraw();
}


void CMedit::fluid_zooming( float x0, float x1, float y0, float y1 ) {
	
	float change_x = (drx2dtx(x0) - drx2dtx(x1));
	float change_y = (dry2hdty(y0) - dry2hdty(y1));

	// printf("%f, %f, %f\n", drx2dtx(x0), drx2dtx(x1), change_x);
	data_x_min_for_display_ -= change_x;
	data_x_max_for_display_ += change_x;

	// data_y_min_for_hist_display_ -= change_y;
	data_y_max_for_hist_display_ += change_y;

	updaterange();
	redraw();
}

int CMedit::handle_interpolation(int ev) {

	int x = wx2cmapx( Fl::event_x() );
	float y = wy2dry( Fl::event_y() );
	int field = 0;
	switch(ev) {
		case FL_SHORTCUT: // undo
		return 0;

		case FL_PUSH: //

		if(Fl::event_state(FL_SHIFT)) {
			field = ALPHA;
		}
		else {
		// Which button?  Take account of alt/meta modifiers too.
			int btn = 1;
			if(Fl::event_state(FL_BUTTON2 | FL_ALT)) {
				btn = 2;
			}
			else if(Fl::event_state(FL_BUTTON3 | FL_META)) {
				btn = 3;
			}
			static CMfield btn2field[2][3] = {
				{ RED, GREEN, BLUE },
				{ HUE, SAT, BRIGHT }
			};
			field = btn2field[ hsbmode ][ btn-1 ];
		}
	
		interp_linear(x, y, field);

	}
	return 0;
}

int CMedit::interp_linear(int x, float y, int field) {

	switch(field) {
		case HUE:
			interp_linear_onePoint(x, y, HUE_con_points, vh);
			insert_into_controls(x, y, HUE_con_points, 0);
			for(int i=0; i<HUE_con_points.size();i++) {
				printf("%f ", HUE_con_points[i].data_x);
			}
			printf("\n");
		break;
		case SAT:
			interp_linear_onePoint(x, y, SAT_con_points, vs);
			insert_into_controls(x, y, SAT_con_points, 0);
		break;
		case BRIGHT:
			interp_linear_onePoint(x, y, BRI_con_points, vb);
			insert_into_controls(x, y, BRI_con_points, 0);
		break;
		case ALPHA:
			interp_linear_onePoint(x, y, ALP_con_points, alpha);
			insert_into_controls(x, y, ALP_con_points, 0);
		break;
		default: printf("%s\n", "AH!!"); break;
	}
	redraw();
	return 0;

}

void CMedit::insert_into_controls(int x, float y, std::vector<contPoint> &v, int type) {
	// std::vector<contPoint>::iterator it = v.begin();
	if(cmapx2dtx(x) < v[0].data_x){
		contPoint cp;
		cp.data_x = cmapx2dtx(x);
		cp.data_y = y;
		cp.type = type;
		v.insert(v.begin(), cp);
	}
	else if(cmapx2dtx(x) > v[v.size()-1].data_x) {
		contPoint cp;
		cp.data_x = cmapx2dtx(x);
		cp.data_y = y;
		cp.type = type;
		v.push_back(cp);
	}
	else{
printf("insertttttt\n");
		for (int i=1; i<v.size()-1; i++) {
			if(cmapx2dtx(x) >= v[i-1].data_x && cmapx2dtx(x) <= v[i].data_x) {
				contPoint cp;
				cp.data_x = cmapx2dtx(x);
				cp.data_y = y;
				cp.type = type;
				v.insert(v.begin()+i, cp); // Doesn't insert....
				for(int k=0; k<v.size();k++) {
				printf("%f ", v[k].data_x);
			}
			printf("\n");
				break;
			}
		}
	}

}

int CMedit::interp_linear_onePoint(int x, float y, std::vector<contPoint> v, float (&arr)[CMENTMAX]) {
	if(cmapx2dtx(x) < v[0].data_x){
	printf("Smallest\n");
		float dx = dtx2cmapx(v[0].data_x);
		for (int j=x;j<=dx;j++){ // edge cases...cmap not applied to certain part of data range??
			arr[j] = y + j*(v[0].data_y - y)/float(dx - x);
		printf("the arr[j] is %f, and vh[j] %f\n", arr[j], vh[j]);
		}
	}
	else if(cmapx2dtx(x) > v[v.size()-1].data_x) {
	printf("Biggest\n");
		float dx = dtx2cmapx(v[v.size()-1].data_x);
		for (int j=dx;j<=x;j++){ // edge cases...cmap not applied to certain part of data range??
			arr[j] = v[v.size()-1].data_y + j*(y - v[v.size()].data_y)/float(x - dx);
		printf("the arr[j] is %f, and vh[j] %f\n", arr[j], vh[j]);
		}
	}
	else {
		for (int i=1; i<=v.size()-1; i++) {
			if(cmapx2dtx(x) >= v[i-1].data_x && cmapx2dtx(x) <= v[i].data_x) {
				float dx_pre = dtx2cmapx(v[i-1].data_x);
				float dx_aft = dtx2cmapx(v[i].data_x);
				for (int j=dx_pre;j<x;j++){ // edge cases...cmap not applied to certain part of data range??
					arr[j] = v[i-1].data_y + j*(y - v[i-1].data_y)/float(x - dx_pre);
				}
				for (int j=x;j<=dx_aft;j++){ // edge cases...cmap not applied to certain part of data range??
					arr[j] = y + j*(v[i].data_y - y)/float(dx_aft - x);
				}
				break;
			}
		}
	}
	return 0;
}

// **** Virtual Funcs: draw(), resize() and handle() END


float CMedit::drag_drawing( int x, enum CMfield field, float y, float lerp ) {
	float ny, oy, *vp; //???
	float rgb[3];
	int usergb = 0;
	if(x < 0 || x >= cment()) {
		return 0;
	}
	if(locked && (x < lockmin || x > lockmax)) {
		return 0;
	}
	switch(field) {
		case HUE: vp = &vh[x]; break;
		case SAT: vp = &vs[x]; break;
		case BRIGHT: vp = &vb[x]; break;
		case ALPHA: vp = &alpha[x]; break;
		default:
		hsb2rgb( vh[x],vs[x],vb[x], &rgb[0],&rgb[1],&rgb[2] );
		vp = &rgb[ field - RED ];
		usergb = 1;
	}
	if(field == HUE) {
		oy = hue2y(*vp);
		y = hue2y( huenear( y2hue(y), *vp ) );
		ny = (1-lerp) * oy + lerp * y;
		*vp = y2hue(ny);
	}
	else {
		oy = *vp;
		ny = (1-lerp) * oy + lerp * y;
		if(field != ALPHA) {
			if(ny < 0) ny = 0;
			else if(ny > 1) ny = 1;
	}
		*vp = ny;
		if(usergb) rgb2hsb( rgb[0],rgb[1],rgb[2], &vh[x],&vs[x],&vb[x] );
	}
	return ny;
}

void CMedit::dragrange( int x0, int x1, enum CMfield field, float y0, float y1, float lerp ) {
	int x;
	float dy;
	if(x0 == x1  || x0 < 0 && x1 < 0  ||  x0 >= cment() && x1 >= cment()) {
		return;
	}
	dy = (y1 - y0) / (x1 - x0);
	if(x0 < x1) {
    	for(x = x0; x < x1; x++) {
    		drag_drawing( x, field, y0 + dy*(x-x0), lerp );
		}
	}
	else {
		for(x = x0; x > x1; x--) {
			drag_drawing( x, field, y0 + dy*(x-x0), lerp );
		}
	}
}


float CMedit::huenear( float hue, float hueref ) {
	float h = hue;
	if(h - hueref > .5f) {
		do { 
			h -= 1.0f;
		}
		while (h - hueref > .5);
	}
	else {
		while(h - hueref < -.5f) {
			h += 1.0f;
		}
	}
	return h;
}

float CMedit::hue2y( float hue ) {
    /* Find closest multiple of given hue to reference y-position y0 */
	return (hue - hueshift) * huezoom;
}

float CMedit::y2hue( float y ) {
	return y / huezoom + hueshift;
}



void CMedit::getrgba( int index, float rgba[4] ) const {
	if(index < 0 || index >= cment()-1) {
		rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
	}
	else {
		hsb2rgb( vh[index], vs[index], vb[index], &rgba[0],&rgba[1],&rgba[2] );
		rgba[3] = alpha[index];
	}
}

void CMedit::gethsba( int index, float hsba[4] ) const {
	if(index < 0 || index >= cment()-1) {
	hsba[0] = hsba[1] = hsba[2] = hsba[3] = 0;
	}
	else {
		hsba[0] = vh[index];
		hsba[1] = vs[index];
		hsba[2] = vb[index];
		hsba[3] = alpha[index];
	}
}

//**** Updaters

void CMedit::range_update_to( void(*func)( CMedit *cm ) ) {
	range_updater = func;
}

void CMedit::updaterange() {
	(*range_updater)( this );
}

void CMedit::reportto( void (*func)( CMedit *cm, int index ) ) {
	reportfunc = func;
}

void CMedit::report( int index ) {
	if(index < 0 || index >= cment() || reportfunc == NULL) return;
	lastx_ = index;
	(*reportfunc)( this, index );
}

void CMedit::report() {
	CMedit::report( lastx_ );
}

//**** Updaters END

void colorpatch::draw() {
	glClearColor( vr,vg,vb,va );
	glClear( GL_COLOR_BUFFER_BIT );
}

void CMedit::snapshot() {
	printf("%s\n", "SHOT");
	colorEnt snapper;

	for(int k = 0; k < CMENTMAX; k++) {
		snapper.ent[0][k] = vh[k];
		snapper.ent[1][k] = vs[k];
		snapper.ent[2][k] = vb[k];
		snapper.ent[3][k] = alpha[k];
	}
	// snapcment_ = cment_;
	snapper.this_cment = cment_;

	if(undo_stack_count>=10){
		// maybe use a deque...?
		undo_stack.pop_front();
		undo_stack.push_back(snapper);
		return;
	}
	else{
		undo_stack.push_back(snapper);
		undo_stack_count++;
	}
}

int CMedit::undo() {
	float t;
	// float** snap = (float**)(4*CMENTMAX);
	// printf("%d\n", undo_stack.size());
	if (undo_stack.empty()) return 0;
	// printf("%s\n", "GOT HERE?");
	colorEnt snapper;
	snapper = undo_stack.back();
	for(int k = 0; k < CMENTMAX; k++) {
		t = vh[k];       vh[k] = snapper.ent[0][k];  snapper.ent[0][k] = t;
		t = vs[k];       vs[k] = snapper.ent[1][k];  snapper.ent[1][k] = t;
		t = vb[k];       vb[k] = snapper.ent[2][k];  snapper.ent[2][k] = t;
		t = alpha[k]; alpha[k] = snapper.ent[3][k];  snapper.ent[3][k] = t;
	}
	// int i = cment_; cment_ = snapcment_; snapcment_ = i;
	cment_ = snapper.this_cment;
	redraw();
	cment(snapper.this_cment);
	if(redo_stack_count>=10){
		redo_stack.pop_back();
		redo_stack.push_front(snapper);
	}
	else{
		redo_stack.push_front(snapper);
		redo_stack_count++;
	}
	undo_stack.pop_back();
	undo_stack_count--;
	
	// printf("%d\n", undo_stack.size());
	return 1;
}

int CMedit::redo() {
	float t;
	if (redo_stack.empty()) return 0;
// 	printf("%s\n", "GOT HERE?");
	colorEnt snapper;
	snapper = redo_stack.front();
	for(int k = 0; k < CMENTMAX; k++) {
		t = vh[k];       vh[k] = snapper.ent[0][k];  snapper.ent[0][k] = t;
		t = vs[k];       vs[k] = snapper.ent[1][k];  snapper.ent[1][k] = t;
		t = vb[k];       vb[k] = snapper.ent[2][k];  snapper.ent[2][k] = t;
		t = alpha[k]; alpha[k] = snapper.ent[3][k];  snapper.ent[3][k] = t;
	}
// 	// int i = cment_; cment_ = snapcment_; snapcment_ = i;
	snapper.this_cment = cment_;
	redraw();
	cment(snapper.this_cment);
	if(undo_stack_count>=10){
// 		// maybe use a deque...?
		undo_stack.pop_front();
		undo_stack.push_back(snapper);
// 		return;
	}
	else{
		undo_stack.push_back(snapper);
		undo_stack_count++;
	}

	redo_stack.pop_front();
	// undo_stack.pop_back();
	redo_stack_count--;
// 	printf("%d\n", undo_stack.size());
	return 1;
}

void CMedit::init() {

	hist_data_x_min_ = 0.0;
	hist_data_x_max_ = 1.0;
	hist_data_y_min_ = 0.0;
	hist_data_y_max_ = 1.0; 

	data_x_min_for_cmap_ = 0.0;
	data_x_max_for_cmap_ = 1.0;
	data_y_min_for_cmap_ = 0.0;
	data_y_max_for_cmap_ = 1.0;


	data_x_min_for_display_ = 0.0;
	data_x_max_for_display_ = 1.0;

	data_y_min_for_hist_display_ = 0.0;
	data_y_max_for_hist_display_ = 1.0;
	data_y_min_for_cmap_display_ = 0.0;
	data_y_max_for_cmap_display_ = 1.0;


	data_x_min_for_display_ori_ = data_x_min_for_display_;
	data_x_max_for_display_ori_ = data_x_max_for_display_;
	data_y_max_for_hist_display_ori_ = data_y_max_for_hist_display_;
	data_y_min_for_hist_display_ori_ = data_y_min_for_hist_display_;


	editing_mode = 3;
	hist_ent_arr = NULL;

	// cment_ = snapcment_ = 256;
	cment_ = snapcment_ = cmapw();

	remin = 0;
	remax = cment()-1;

	lerpval = 0.5;
	hsbmode = 1;
	dragfrom = -1;  dragval = 0;  dragamount = 0;
	locked = 0;
	lockmin = 0, lockmax = cment()-1;

	hueshift = 0;
	huezoom = 1;
	// draghue = 0;

	undo_stack_count = 0;
	redo_stack_count = 0;

	contPoint cp0; cp0.data_x = 0.0; cp0.data_y = 0.2; cp0.type = 0;
	contPoint cp1; cp1.data_x = 1.0; cp1.data_y = 0.8; cp1.type = 0;
	contPoint cp2; cp2.data_x = 0.0; cp2.data_y = 0.1; cp2.type = 0;
	contPoint cp3; cp3.data_x = 1.0; cp3.data_y = 0.7; cp3.type = 0;
	contPoint cp4; cp4.data_x = 0.0; cp4.data_y = 0.2; cp4.type = 0;
	contPoint cp5; cp5.data_x = 1.0; cp5.data_y = 0.6; cp5.type = 0;
	contPoint cp6; cp6.data_x = 0.0; cp6.data_y = 0.3; cp6.type = 0;
	contPoint cp7; cp7.data_x = 1.0; cp7.data_y = 0.9; cp7.type = 0;

	HUE_con_points.push_back(cp0);HUE_con_points.push_back(cp1);
	SAT_con_points.push_back(cp2);SAT_con_points.push_back(cp3);
	BRI_con_points.push_back(cp4);BRI_con_points.push_back(cp5);
	ALP_con_points.push_back(cp6);ALP_con_points.push_back(cp7);



	ncomments = 0;
	maxcomments = 8;
	comments = (char **)malloc( maxcomments * sizeof(char *) );

	// for(int k = 0; k < cment_; k++) {
	// 	vh[k] = 1 - .5*k / cment_;
	// 	vs[k] = .5;
	// 	vb[k] = .25 + .75*k / cment_;
	// 	alpha[k] = .33 + .67 * k*k / (cment_*cment_);
	// }

	for(int k=0; k<cmapw(); k++) {
		vh[k] = 0.2 + k*0.6/cmapw();
		vs[k] = 0.1 + k*0.6/cmapw();
		vb[k] = 0.2 + k*0.4/cmapw();
		alpha[k] = 0.3 + k*0.6/cmapw();
	}

	snapshot();
}