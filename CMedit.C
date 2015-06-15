#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <string.h>


#include <ctype.h>
#undef isspace

#include "CMedit.H"
#include "colorpatch.H"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

CMedit :: CMedit(int x, int y, int w, int h, const char *label)
: Fl_Gl_Window(x,y,w,h,label) {
  init();
  end();
}

int CMedit::fload( FILE *inf ) {
  char line[256];
  char *cp, *sp;
  int count = -1;
  int i, ix, ox, nix, prevox;
  float rgba[4], hsba[4], phsba[4];
  int lno;
  static enum CMfield flds[4] = { HUE, SAT, BRIGHT, ALPHA };
  int f;
  char tc[2];

  lno = 0;
  this->postscale_ = 1;
  this->postexpon_ = 1;
  ox = 0;
  while(fgets(line, sizeof(line), inf) != NULL) {
    lno++;
    for(cp = line; *cp && isspace(*cp); cp++)
     ;
   if(*cp == '\0' || *cp == '\n')
     continue;

   sp = strstr(cp, "#Ascale");
   if(sp != NULL) {
     if(0==strncmp(sp, "#Ascale", 7)) {
       *sp = '\0';
       sp += 7;
       if(*sp == '=' || *sp == ':')
        sp++;
      sscanf(sp, "%f%f", &this->postscale_, &this->postexpon_);
  	    cp = sp;	/* don't record this one as a comment */
    }
  }

  if(*cp == '#') {
  	if(ncomments >= maxcomments) {
     maxcomments *= 2;
     comments = (char **)realloc( comments, maxcomments * sizeof(char *) );
   }
   comments[ncomments++] = strdup( line );
   continue;
 }

      /* ``nnn:'' entries set the colormap pointer ... */
  if(sscanf(line, "%d%1[:]", &nix, tc) == 2) {
   ix = nix;
   cp = strchr(line, ':') + 1;
   while(*cp && isspace(*cp)) cp++;
   if(*cp == '\0' || *cp == '\n' || *cp == '#')
     continue;
  }

  if(count == -1) {
    if(!sscanf(line, "%d", &count) || count < 1) {
     fprintf(stderr, "Not a .cmap file?  Doesn't begin with a number.\n");
     return 0;
    }
    cment( count );
    ix = 0, prevox = 0;
    continue;
  } 
    else {

      if(ix >= count)
       break;

      rgba[3] = 1;
      if(sscanf(cp, "%f%f%f%f", &rgba[0],&rgba[1],&rgba[2],&rgba[3]) < 3) {
       fprintf(stderr, "Couldn't read colormap line %d (cmap entry %d of 0..%d)\n",
        lno, ix, count-1);
       return 0;
      }
      rgb2hsb( rgba[0],rgba[1],rgba[2], &hsba[0],&hsba[1],&hsba[2] );
      hsba[3] = rgba[3];
      if(ox == 0)
       memcpy(phsba, hsba, sizeof(phsba));
      else
       phsba[0] = huenear(phsba[0], hsba[0]);
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
  if(ix < count)
    fprintf(stderr, "Only got %d colormap entries, expected %d\n", ix, count);

  for(f = 0; f < 4; f++)
    dragrange( prevox, cment_, flds[f], phsba[f], phsba[f], 1.0 );

  if(postscale_ != 1 || postexpon_ != 1) {
    for(i = 0; i < count; i++)
    	alpha[i] = (alpha[i] > 0) ? pow(alpha[i] / postscale_, 1 / postexpon_) : 0;
  }

  return ix > 0;
}

int CMedit::fsave( FILE *outf ) {
  float r,g,b;
  int i, ok = 1;

  fprintf(outf, "%d\n", cment());
  for(i = 0; i < cment_; i++) {
    hsb2rgb( vh[i], vs[i], vb[i], &r, &g, &b );
    if(fprintf(outf, "%f %f %f %f\n", r, g, b, Aout( alpha[i] )) <= 0)
     ok = 0;
  }
  if(postscale_ != 1 || postexpon_ != 1)
    fprintf(outf, "#Ascale %g %g\n", postscale_, postexpon_);
  if(ncomments > 0)
    fputs("\n", outf);
  for(i = 0; i < ncomments; i++)
    fputs(comments[i], outf);
  return ok;
}


//NEW CONSTRUCTION AREA===================================================

//*****Load function for Histograms

/* Possible situations of creating/changing/saving/loading files

  1. LOAD: nothing;     CREATE cmap;      SAVE cmap;
  2. LOAD: hist;        CREATE cmap;      SAVE cmap;
  3. LOAD: hist;        CREATE cmap;      SAVE cmap+hist;
  4. LOAD: cmap+hist;   CHANGE cmap;      SAVE 

*/


/* Attributes that a histogram/histogram&cmap file can contain:

  datamin
  datamax
  
  datamin_for_cmap
  datamax_for_cmap

  datamin_for_hist
  datamin_for_hist

*/

/* Options when saving cmap/histogram/cmap&histogram files
  
  Save the cmap only (like before)
  Save a hcmap file which contains histogram, cmap and all the constraints

*/


int CMedit::histfload( FILE *inf ) {
  char line[256];
  char *flag_pointer;
  int entry_counter;


  fgets(line, sizeof(line), inf);

  fgets(line, sizeof(line), inf);
  data_point_num_ = atoi(line);


  fgets(line, sizeof(line), inf);
  hist_data_x_max_ = atoi(line);
  data_x_max_for_cmap_ = hist_data_x_max_;
  data_x_max_for_display_ = hist_data_x_max_;



  hist_ent_arr = (float*) malloc(hist_data_x_max_ * sizeof(float));

  float maxmax = 0;

  for (int i=0; i<hist_data_x_max_; i++) {
    // inf.getline(line, 256);  
    fgets(line, sizeof(line), inf);
    strtok(line, " ");
    hist_ent_arr[i] = atof(strtok(NULL, " "));
    // printf("%f\n", hist_ent_arr[i]);
    maxmax = (hist_ent_arr[i]>maxmax)?hist_ent_arr[i]:maxmax;
  }

  hist_data_y_max_ = maxmax;
  data_y_max_for_cmap_ = hist_data_y_max_;
  data_y_max_for_display_ = hist_data_y_max_;

  updaterange();
  redraw();
}




//***** END Save & Load functions for Histograms
/*
  Xiao Pang doesn't agree

*/
//getters

float CMedit::get_data_x_min_for_display() {
  return data_x_min_for_display_;
}

float CMedit::get_data_x_max_for_display() {
  return data_x_max_for_display_;
}

float CMedit::get_hist_data_x_min() {
  return hist_data_x_min_;
}

float CMedit::get_hist_data_x_max() {
  return hist_data_x_max_;
}

float CMedit::get_data_x_min_for_cmap() {
  return data_x_min_for_cmap_;
}

float CMedit::get_data_x_max_for_cmap() {
  return data_x_max_for_cmap_;
}

float CMedit::get_data_y_min_for_display() {
  return data_y_min_for_display_;
}

float CMedit::get_data_y_max_for_display() {
  return data_y_max_for_display_;
}

float CMedit::get_hist_data_y_min() {
  return hist_data_y_min_;
}

float CMedit::get_hist_data_y_max() {
  return hist_data_y_max_;
}

float CMedit::get_data_y_min_for_cmap() {
  return data_y_min_for_cmap_;
}

float CMedit::get_data_y_max_for_cmap() {
  return data_y_max_for_cmap_;
}

//setters

void CMedit::set_data_x_min_for_display(float val) {
  data_x_min_for_display_ = val;
}

void CMedit::set_data_x_max_for_display(float val) {
  data_x_max_for_display_ = val;
}

void CMedit::set_hist_data_x_min(float val) {
  hist_data_x_min_ = val;
}

void CMedit::set_hist_data_x_max(float val) {
  hist_data_x_max_ = val;
}

void CMedit::set_data_x_min_for_cmap(float val) {
  data_x_min_for_cmap_ = val;
}

void CMedit::set_data_x_max_for_cmap(float val) {
  data_x_max_for_cmap_ = val;
}

void CMedit::set_data_y_min_for_display(float val) {
  data_y_min_for_display_ = val;
}

void CMedit::set_data_y_max_for_display(float val) {
  data_y_max_for_display_ = val;
}

void CMedit::set_hist_data_y_min(float val) {
  hist_data_y_min_ = val;
}

void CMedit::set_hist_data_y_max(float val) {
  hist_data_y_max_ = val;
}

void CMedit::set_data_y_min_for_cmap(float val) {
  data_y_min_for_cmap_ = val;
}

void CMedit::set_data_y_max_for_cmap(float val) {
  data_y_max_for_cmap_ = val;
}

#define  YMAX  (1.0)
#define  YMIN  (-.25)
#define  YBAR0  (-.01)
#define  YBAR1  (-.06)

#define  XMIN  (-cment_ / 16.f)
#define  XMAX  cment_

// int CMedit::wx2x( int wx ) { // window coord to cmap coord
//   return (int) (XMIN + wx * (XMAX-XMIN) / w());
// }

float CMedit::wy2y( int wy ) { // 
  return (YMAX - wy * (YMAX-YMIN) / h());
}

//*****New Coordinate Systems

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


#define  HIST_XMIN  0
#define  HIST_XMAX  hist_data_x_max_
#define  HIST_XW    (HIST_XMAX-HIST_XMIN)
#define  HIST_YMIN  0
#define  HIST_YMAX  hist_data_y_max_
#define  HIST_YH    (HIST_YMAX-HIST_YMIN)

// // #define  CMAP_XMIN  (-cment_ / 16.f)
#define  CMAP_XMIN  0
#define  CMAP_XMAX  cment_
#define  CMAP_XW     (CMAP_XMAX-CMAP_XMIN)
#define  CMAP_YMIN  0
#define  CMAP_YMAX  cmap_y_max_
#define  CMAP_YH    (CMAP_YMAX-CMAP_YMIN)

#define  DT_XMIN    data_x_min_for_display_
#define  DT_XMAX    data_x_max_for_display_
#define  DT_XW      (DT_XMAX-DT_XMIN)
//Y
#define  DT_YMIN    (0.0)
#define  DT_YMAX    data_y_max_for_display_
#define  DT_YH      (DT_YMAX-DT_YMIN)

#define  DTHIST_XMIN  hist_data_x_min_
#define  DTHIST_XMAX  hist_data_x_max_
#define  DTHIST_XW    (DTHIST_XMAX-DTHIST_XMIN)
#define  DTHIST_YMIN  hist_data_y_min_
#define  DTHIST_YMAX  hist_data_y_max_
#define  DTHIST_YH    (DTHIST_YMAX-DTHIST_YMAX)

#define  DTCMAP_XMIN  data_x_min_for_cmap_
#define  DTCMAP_XMAX  data_x_max_for_cmap_
#define  DTCMAP_XW    (DTCMAP_XMAX-DTCMAP_XMIN)
#define  DTCMAP_YMIN  data_y_min_for_cmap_
#define  DTCMAP_YMAX  data_y_max_for_cmap_
#define  DTCMAP_YH    (DTCMAP_YMAX-DTCMAP_YMIN)


//windows to drawing
float CMedit::wx2drx( int wx ) {//pixel coordinate comes in as int
  return (DR_XMIN + wx * ( DR_XW ) / w());
}
float CMedit::wy2dry( int wy ) {//pixel coordinate comes in as int
  return (DR_YMIN + wy * ( DR_YH ) / h());
}

//data to drawing
float CMedit::dtx2drx( float dtx ) {
  return (( dtx - DT_XMIN ) * ( DR_XFROM0 ) / DT_XW);
}

float CMedit::dty2dry( float dty ) {
  return (( dty - DT_YMIN ) * ( DR_YFROM0 ) / DT_YH);
}

//drawing to data
float CMedit::drx2dtx( float drx ) {
  return (DT_XMIN + (drx) * ( DT_XW ) / DR_XFROM0);
}//add a drawing zero constant


//histogram to data
float CMedit::histx2dtx( int histx ) {
  return (DTHIST_XMIN + ( histx - HIST_XMIN ) * ( DTHIST_XW ) / HIST_XW);
}
float CMedit::histy2dty( int histy ) {
  return (DTHIST_YMIN + ( histy - HIST_YMIN ) * ( DTHIST_YH ) / HIST_YH );
}

//colormap to data
float CMedit::cmapx2dtx( int cmapx ) {
  return (DTCMAP_XMIN + (cmapx - CMAP_XMIN) * ( DTCMAP_XW ) / CMAP_XW);
}
float CMedit::cmapy2dty( int cmapy ) {
  return (DTCMAP_YMIN + (cmapy - CMAP_YMIN) * ( DTCMAP_YH ) / CMAP_YH);
}

//data to colormap
int CMedit::dtx2cmapx( float dtx ) {
  if (dtx < DTCMAP_XMIN) {
    return CMAP_XMIN-1;
  }
  else if (dtx > DTCMAP_XMAX) {
    return CMAP_XMAX+1;
  }
  else {
    int cmap_ind = (int) ( CMAP_XMIN + (dtx - DTCMAP_XMIN) * ( CMAP_XW ) / DTCMAP_XW);
    return cmap_ind;
  }
}

//data to histogram this is wrong
int CMedit::dtx2histx( float dtx ) {
  int hist_ind = (int) ((dtx - DT_XW) * ( HIST_XW ) / DT_XW);
  if (hist_ind >= HIST_XMIN && hist_ind <= HIST_XMAX) {
    return hist_ind;
  }
  else {
    return -1;
  }
}

// 
float CMedit::hist_y_scaler( float dty ) {
  return dty2dry(dty);
}
//

//*****New Coordinate System Transformations END



//END NEW CONSTRUCTION AREA=================================================

void CMedit::draw() {
  int i; //?
  float v; //?
  //int coarse = (w() >= 2*DR_XMAX); //I guess??
  int coarse = 1; //what???

  if(!valid() || damage()) {
    valid(1);
    remin = 0; remax = DR_XMAX;

    glViewport( 0, 0, w(), h() );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( DR_XMIN, DR_XMAX, DR_YMIN, DR_YMAX, -1, 1 );//Changed!!!

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
  }

  // if()


  glClearColor( 0,0,0,0 );
  glClear( GL_COLOR_BUFFER_BIT );

//***cmap part=========================
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_LIGHTING );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_COLOR_MATERIAL ); 

//***histogram part=========================

  // float Y[] = {0.324340, 0.184513, 0.455935, 0.983492, 0.413960, 0.410116, 0.396811, 0.126532, 0.068274, 0.984338, 0.354699, 0.819587, 0.039018, 0.037167, 0.276222, 0.075273, 0.671394, 0.038015, 0.162898, 0.181561, 0.811186, 0.598697, 0.238338, 0.913472, 0.999503, 0.266164, 0.578180, 0.893520, 0.432901, 0.705469, 0.883511, 0.276978, 0.365474, 0.645072, 0.009056, 0.060010, 0.332326, 0.842911, 0.497601, 0.910704, 0.941651, 0.850012, 0.111624, 0.349739, 0.397841, 0.616044, 0.749573, 0.507757, 0.816912, 0.820676, 0.774584, 0.954912, 0.916837, 0.218831, 0.802885, 0.995048, 0.321064, 0.215885, 0.090240, 0.907219, 0.846479, 0.308137, 0.398779, 0.878454, 0.272959, 0.967407, 0.768639, 0.299569, 0.311693, 0.361166, 0.869193, 0.609403, 0.482768, 0.260645, 0.502629, 0.583083, 0.463928, 0.693696, 0.028416, 0.435589, 0.225842, 0.219674, 0.449149, 0.615937, 0.400304, 0.307836, 0.541936, 0.529348, 0.069277, 0.330198, 0.735907, 0.955596, 0.434552, 0.887078, 0.234281, 0.637010, 0.499682, 0.929375, 0.356213, 0.274177, 0.917317, 0.402061, 0.390121, 0.253212, 0.671374, 0.401175, 0.615069, 0.656687, 0.515884, 0.432584, 0.921714, 0.796624, 0.764711, 0.973687, 0.751637, 0.271167, 0.647408, 0.702861, 0.225550, 0.060758, 0.182582, 0.826465, 0.808791, 0.521218, 0.865758, 0.839763, 0.644469, 0.913731, 0.668367, 0.815981, 0.722289, 0.311850, 0.003498, 0.304198, 0.315159, 0.197128, 0.688034, 0.016105, 0.399702, 0.331068, 0.966212, 0.353680, 0.173368, 0.948103, 0.998288, 0.695262, 0.905166, 0.410266, 0.273170, 0.861891, 0.301884, 0.858582, 0.797684, 0.703369, 0.491786, 0.405445, 0.767023, 0.967312, 0.746526, 0.448356, 0.290321, 0.601216, 0.710448, 0.511332, 0.476508, 0.413848, 0.638533, 0.996599, 0.298670, 0.885475, 0.819359, 0.882651, 0.983368, 0.699751, 0.959827, 0.445325, 0.805068, 0.570580, 0.900812, 0.781105, 0.783551, 0.390493, 0.263095, 0.118839, 0.298464, 0.826759, 0.823877, 0.596384, 0.395522, 0.733147, 0.856996, 0.538822, 0.084982, 0.136633, 0.335369, 0.205704, 0.150507, 0.229318, 0.088577, 0.829534, 0.631044, 0.984948, 0.795217, 0.103131, 0.019430, 0.708380, 0.531245, 0.640397, 0.949489, 0.051924, 0.910782, 0.527707, 0.570317, 0.293839, 0.538865, 0.155049, 0.543141, 0.859710, 0.281036, 0.355882, 0.706533, 0.529030, 0.254825, 0.425659, 0.925936, 0.718374, 0.105305, 0.354478, 0.921387, 0.735522, 0.150311, 0.981769, 0.842967, 0.530813, 0.857653, 0.857738, 0.370701, 0.837924, 0.366502, 0.689893, 0.080003, 0.544109, 0.245078, 0.535467, 0.833646, 0.107362, 0.621948, 0.180310, 0.188742, 0.292664, 0.617638, 0.068078, 0.306712, 0.294926, 0.864395, 0.333884, 0.094721, 0.712724, 0.627852, 0.677477, 0.260288, 0.566306, 0.427265, 0.983572, 0.441762, 0.108103, 0.948935, 0.613641, 0.833825, 0.218330, 0.483575, 0.552763, 0.258698, 0.187450, 0.007914, 0.742375, 0.901258, 0.729467, 0.764222, 0.828674, 0.196342, 0.598295, 0.198156, 0.021937, 0.064222, 0.408313, 0.204567, 0.123050, 0.104243, 0.982664, 0.018824, 0.936292, 0.766781, 0.669919, 0.232596, 0.336120, 0.788338, 0.423820, 0.028507, 0.399960, 0.481375, 0.826793, 0.798626, 0.271559, 0.119713, 0.130514, 0.979387, 0.926317, 0.337397, 0.864719, 0.228182, 0.320474, 0.392166, 0.891916, 0.413281, 0.566079, 0.015975, 0.120827, 0.172858, 0.504725, 0.190588, 0.056226, 0.060194, 0.313992, 0.977405, 0.097882, 0.700912, 0.724535, 0.706321, 0.122228, 0.496163, 0.227533, 0.360576, 0.784946, 0.913734, 0.782870, 0.574299, 0.853648, 0.704803, 0.164956, 0.756370, 0.852192, 0.797511, 0.099105, 0.868004, 0.579656, 0.602121, 0.514178, 0.214095, 0.358822, 0.297329, 0.421306, 0.734928, 0.250070, 0.798767, 0.687225, 0.115859, 0.149449, 0.235462, 0.643683, 0.696363, 0.605679, 0.820723, 0.882940, 0.371322, 0.012141, 0.703397, 0.164213, 0.053896, 0.793056, 0.658591, 0.476501, 0.728082, 0.774775, 0.758148, 0.296712, 0.496700, 0.439004, 0.833675, 0.090948, 0.733169, 0.480992, 0.706112, 0.722258, 0.579144, 0.918438, 0.431005, 0.340234, 0.730391, 0.395507, 0.994912, 0.228368, 0.957469, 0.853040, 0.189834, 0.486795, 0.350047, 0.912033, 0.127237, 0.253313, 0.850527, 0.576220, 0.633750, 0.787692, 0.132504, 0.601032, 0.071839, 0.975415, 0.762926, 0.614730, 0.659655, 0.359902, 0.882575, 0.255398, 0.901348, 0.677072, 0.114687, 0.261755, 0.221042, 0.028545, 0.089946, 0.177448, 0.485637, 0.422131, 0.638698, 0.544417, 0.286050, 0.589673, 0.872667, 0.636038, 0.629612, 0.099657, 0.495017, 0.881578, 0.208942, 0.549463, 0.647471, 0.523135, 0.926912, 0.146284, 0.421895, 0.341903, 0.550930, 0.602199, 0.549693, 0.627555, 0.915890, 0.333335, 0.169927, 0.112595, 0.033641, 0.519735, 0.154783, 0.358927, 0.081778, 0.505594, 0.868292, 0.219507, 0.521624, 0.236685, 0.700018, 0.270854, 0.741829, 0.873976, 0.232923, 0.288228, 0.571083, 0.070951, 0.141124, 0.020555, 0.984059, 0.497543, 0.582678, 0.853861, 0.707556, 0.726008, 0.858279, 0.996423, 0.331560, 0.693522, 0.294582, 0.544664, 0.965506, 0.854877, 0.293371, 0.840771, 0.118663, 0.849235, 0.791215, 0.361121, 0.203496, 0.980463, 0.744066, 0.432756, 0.491495, 0.070823, 0.462377, 0.812111, 0.402651, 0.491188, 0.532523, 0.689629, 0.415382, 0.251998, 0.035234, 0.930012, 0.465195, 0.492849, 0.126577, 0.388188, 0.119555, 0.903703, 0.502053, 0.183991, 0.149191, 0.784881, 0.027264, 0.402273, 0.335512, 0.451047, 0.943970, 0.207149, 0.600300, 0.004492, 0.975442, 0.019828, 0.074582, 0.672506, 0.000336, 0.916605, 0.863349, 0.181554, 0.646458, 0.260655, 0.507696, 0.845527, 0.768707, 0.116686, 0.137460, 0.959830, 0.151091, 0.862733, 0.624439, 0.622114, 0.745460, 0.255996, 0.829975, 0.473913, 0.424458, 0.432050, 0.668529, 0.085136, 0.020271, 0.859301, 0.910917, 0.477000, 0.990828, 0.847465, 0.786093, 0.658959, 0.643796, 0.719987, 0.436413, 0.835016, 0.195942, 0.731156, 0.815856, 0.168692, 0.628518, 0.521449, 0.028130, 0.414224, 0.414112, 0.769811, 0.039458, 0.277053, 0.018886, 0.288442, 0.701223, 0.052094, 0.801913, 0.975413, 0.916681, 0.859118, 0.823566, 0.650403, 0.743499, 0.820742, 0.709260, 0.010089, 0.004968, 0.410859, 0.675266, 0.412842, 0.884757, 0.811345, 0.981811, 0.177996, 0.182118, 0.925879, 0.659061, 0.782158, 0.561215, 0.316390, 0.777362, 0.257369, 0.195277, 0.672903, 0.858331, 0.604685, 0.754939, 0.422723, 0.871246, 0.062603, 0.209774, 0.565197, 0.512166, 0.987078, 0.723900, 0.103918, 0.568393, 0.959288, 0.324835, 0.585509, 0.795587, 0.376519, 0.786502, 0.067223, 0.699031, 0.301247, 0.457174, 0.625705, 0.834840, 0.338580, 0.169829, 0.460486, 0.238634, 0.371362, 0.012128, 0.975352, 0.163563, 0.807528, 0.718115, 0.412453, 0.908672, 0.177781, 0.607659, 0.568285, 0.565427, 0.858814, 0.994871, 0.371322, 0.145328, 0.496767, 0.382265, 0.553772, 0.086972, 0.865027, 0.815291, 0.719205, 0.236118, 0.269186, 0.061843, 0.157239, 0.795409, 0.368178, 0.177888, 0.865541, 0.793280, 0.871088, 0.172921, 0.493942, 0.483744, 0.103775, 0.642235, 0.829606, 0.702945, 0.712127, 0.275753, 0.544045, 0.869023, 0.911102, 0.994324, 0.955075, 0.472027, 0.392338, 0.672534, 0.846920, 0.114118, 0.381715, 0.347514, 0.487497, 0.776125, 0.200974, 0.187496, 0.146295, 0.963055, 0.109057, 0.844089, 0.227710, 0.934141, 0.435979, 0.763924, 0.489376, 0.534251, 0.826515, 0.317120, 0.803212, 0.801809, 0.061433, 0.970203, 0.600413, 0.200562, 0.588574, 0.006869, 0.983692, 0.630821, 0.806008, 0.014559, 0.701009, 0.211452, 0.933590, 0.015338, 0.972503, 0.030833, 0.821960, 0.335165, 0.390535, 0.483124, 0.357295, 0.120294, 0.859435, 0.042987, 0.663472, 0.184612, 0.029931, 0.462729, 0.377456, 0.883579, 0.461858, 0.945597, 0.134790, 0.610005, 0.090489, 0.930895, 0.004468, 0.387962, 0.849480, 0.189504, 0.878115, 0.607525, 0.845872, 0.463862, 0.239165, 0.659338, 0.348809, 0.141093, 0.734886, 0.451600, 0.515313, 0.975661, 0.506859, 0.160903, 0.904128, 0.336918, 0.025095, 0.625130, 0.545572, 0.605102, 0.968888, 0.030818, 0.306993, 0.274030, 0.643820, 0.250671, 0.685633, 0.381111, 0.036529, 0.615953, 0.725699, 0.440367, 0.929761, 0.109596, 0.705550, 0.698471, 0.474800, 0.859222, 0.471355, 0.449627, 0.280751, 0.337257, 0.914433, 0.304186, 0.213310, 0.253975, 0.244660, 0.879560, 0.962070, 0.111129, 0.090245, 0.568678, 0.295906, 0.198953, 0.190775, 0.339134, 0.201724, 0.867610, 0.627830, 0.947398, 0.617354, 0.852776, 0.955303, 0.019021, 0.861284, 0.373268, 0.327499, 0.557493, 0.388187, 0.840084, 0.257113, 0.446754, 0.542182, 0.610651, 0.714603, 0.926840, 0.671755, 0.196757, 0.998951, 0.434196, 0.467693, 0.591420, 0.012183, 0.317911, 0.477846, 0.332778, 0.933401, 0.809325, 0.173753, 0.777247, 0.031957, 0.099598, 0.353686, 0.805340, 0.076247, 0.024642, 0.445832, 0.144999, 0.529704, 0.499378, 0.296305, 0.976878, 0.275930, 0.331468, 0.699991, 0.666836, 0.291655, 0.051042, 0.563098, 0.832711, 0.262030, 0.378921, 0.109168, 0.420546, 0.113161, 0.623929, 0.223573, 0.178533, 0.349395, 0.368097, 0.620127, 0.408209, 0.727669, 0.569755, 0.146448, 0.606558, 0.456906, 0.744521, 0.696687, 0.776784, 0.185666, 0.617013, 0.241555, 0.782198, 0.996523, 0.926703, 0.466032, 0.583299, 0.371753, 0.237724, 0.702554, 0.306678, 0.236313, 0.109788, 0.826337, 0.929184, 0.884789, 0.130293, 0.456969, 0.322544, 0.422997, 0.794600, 0.237739, 0.960679, 0.084091, 0.207594, 0.410532, 0.246294, 0.750983, 0.029190, 0.197880, 0.936231, 0.904405, 0.437743, 0.210913, 0.323741, 0.707117, 0.280943, 0.897763, 0.399707, 0.788278, 0.399091, 0.680550, 0.224964, 0.891968, 0.322663, 0.821977, 0.401791, 0.240356, 0.096391, 0.440024, 0.896993, 0.864796, 0.791188, 0.430817, 0.707125, 0.504607, 0.333433, 0.366844, 0.633408, 0.900742, 0.212522, 0.257094, 0.186004, 0.300649, 0.752506, 0.278200, 0.695617, 0.581968, 0.433548, 0.914297, 0.171506, 0.987497, 0.819415, 0.530124, 0.650028, 0.864860, 0.520726, 0.171731, 0.629368, 0.101222, 0.482214, 0.117483, 0.841115, 0.011996, 0.040469, 0.466249, 0.730013, 0.055250, 0.710098, 0.388513, 0.016748, 0.312400, 0.820931, 0.999122, 0.241786, 0.808268, 0.039390, 0.700097, 0.741989, 0.434754, 0.841184, 0.724426, 0.079174, 0.837447, 0.718582, 0.361956, 0.525342, 0.987318, 0.910450, 0.963847, 0.664472, 0.688177, 0.392231, 0.645863, 0.735178, 0.380988, 0.356146, 0.487138, 0.802550, 0.889097, 0.508873, 0.155823, 0.906952, 0.027929, 0.466114, 0.965556, 0.890214, 0.781477, 0.765742, 0.260005, 0.837690, 0.220790, 0.237295, 0.852765, 0.365509, 0.013401, 0.646768, 0.648673, 0.371324, 0.417865, 0.383330, 0.474750, 0.690883, 0.238639};

  // hist_data_x_max_ = 1024;
  if(hist_ent_arr!=NULL) {
    glBegin(GL_QUADS);
    for (i = HIST_XMIN; i<=HIST_XMAX-1; i++) {
      glColor3f( 0,0,0 );
      glVertex2f( dtx2drx(histx2dtx(i)), 0.0 );
      glVertex2f( dtx2drx(histx2dtx(i)), dty2dry(hist_ent_arr[i]) );
      glVertex2f( dtx2drx(histx2dtx(i+1)), dty2dry(hist_ent_arr[i]) );
      glVertex2f( dtx2drx(histx2dtx(i+1)), 0.0 );
    }
    glEnd();

    glBegin(GL_QUADS);
    for (i = HIST_XMIN; i<=HIST_XMAX-1; i++) {
      float rgb[3];
      float j = histx2dtx(i);
      float k = histx2dtx(i+1);

      if (j >= DTCMAP_XMIN && j <= DTCMAP_XMAX){
        hsb2rgb(vh[ dtx2cmapx(histx2dtx(i)) ], vs[ dtx2cmapx(histx2dtx(i)) ], vb[ dtx2cmapx(histx2dtx(i)) ], &rgb[0], &rgb[1], &rgb[2]);
      }

      else {
        rgb[0] = 125;
        rgb[1] = 125;
        rgb[2] = 125;
      }

      glColor3fv( rgb );
      glVertex2f( dtx2drx(histx2dtx(i)), 0.0 );
      glVertex2f( dtx2drx(histx2dtx(i)), dty2dry(hist_ent_arr[i]) );
      glVertex2f( dtx2drx(histx2dtx(i+1)), dty2dry(hist_ent_arr[i]) );
      glVertex2f( dtx2drx(histx2dtx(i+1)), 0.0 );
    }
    glEnd();
  }

//***END histogram part=========================

  glBegin( GL_QUADS );
  glColor3f( 0,0,0 );
  glVertex2f( remin, -.05 );
  glVertex2f( remax+1, -.05 );
  glColor3f( 1,1,1 );
  glVertex2f( remax+1, YMIN );
  glVertex2f( remin, YMIN );
  glEnd();

  glLineWidth( 1 );
  glDisable( GL_BLEND );

  if(hsbmode) {
    glColor3f( 1,1,0 ); /* Hue: yellow */
    float midhue = y2hue(.5);
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      v = hue2y( huenear( vh[i], midhue ) ); //Need to worry about vh stuffs???
      glVertex2f( dtx2drx(cmapx2dtx(i)),  v);
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), v );
    }
    glEnd();

    glColor3f( .3,1,.25 );  /* Saturation: teal */
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      glVertex2f( dtx2drx(cmapx2dtx(i)), vs[i] );
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), vs[i] );
    }
    glEnd();

    glColor3f( .5,.2,1 );  /* Brightness: purple */
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      glVertex2f( dtx2drx(cmapx2dtx(i)), vb[i] );
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), vb[i] );
    }
    glEnd();
  }

  else {
    float r[CMENTMAX], g[CMENTMAX], b[CMENTMAX];

    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      hsb2rgb( vh[i], vs[i], vb[i], &r[i], &g[i], &b[i] );    
    }

    glColor3f( 1,0,0 ); /* red */
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      glVertex2f( dtx2drx(cmapx2dtx(i)), r[i] );
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), r[i] );
    }
    glEnd();

    glColor3f( 0,1,0 );  /* green */
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      glVertex2f( dtx2drx(cmapx2dtx(i)), g[i] );
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), g[i] );
    }
    glEnd();

    glColor3f( 0,0,1 );  /* blue */
    glBegin( GL_LINE_STRIP );
    for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
      glVertex2f( dtx2drx(cmapx2dtx(i)), b[i] );
      if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), b[i] );
    }
    glEnd();
  }   

  glColor3f( .7,.7,.7 );  /* alpha: gray */
  glBegin( GL_LINE_STRIP );
  for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
    glVertex2f( dtx2drx(cmapx2dtx(i)), alpha[i] );
    if(coarse) glVertex2f( dtx2drx(cmapx2dtx(i+1)), alpha[i] );
  }
  glEnd();

  glDisable( GL_BLEND );
  glShadeModel( GL_SMOOTH );
  glBegin( GL_QUAD_STRIP );  //upper bottom strips
  for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
    float rgb[3];
    hsb2rgb( vh[i], vs[i], vb[i], &rgb[0],&rgb[1],&rgb[2] );
    glColor3fv( rgb );
    glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR0 );
    glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR1 );
    if(coarse) {
      glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR0 );
      glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR1 );
    }
  }
  glEnd();

  glEnable( GL_BLEND ); //This is how you can draw layers on layers
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glShadeModel( GL_SMOOTH );
  glBegin( GL_QUAD_STRIP );  //lower bottom strips
  for(i = CMAP_XMIN; i <= CMAP_XMAX-1; i++) {
    float rgba[4];
    hsb2rgb( vh[i], vs[i], vb[i], &rgba[0],&rgba[1],&rgba[2] );
    rgba[3] = alpha[i];
    glColor4fv( rgba );
    glVertex2f( dtx2drx(cmapx2dtx(i)), YBAR1 );
    glVertex2f( dtx2drx(cmapx2dtx(i)), YMIN );
    if(coarse) {
      glVertex2f( dtx2drx(cmapx2dtx(i+1)), YBAR1 );
      glVertex2f( dtx2drx(cmapx2dtx(i+1)), YMIN );
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

  /* draw (I hope) any children lying on top of us */
  // if(children()) Fl_Gl_Window::draw();

//***END cmap part=========================



}










 float CMedit::drag( int x, enum CMfield field, float y, float lerp ) {
  float ny, oy, *vp;
  float rgb[3];
  int usergb = 0;
  //printf("%d\n", x);/////////////////////////
  if(x < 0 || x >= cment())
    return 0;
  if(locked && (x < lockmin || x > lockmax))
    return 0;
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
  } else {
    oy = *vp;
    ny = (1-lerp) * oy + lerp * y;
    if(field != ALPHA) {
     if(ny < 0) ny = 0;
     else if(ny > 1) ny = 1;
   }
   *vp = ny;
   if(usergb)
     rgb2hsb( rgb[0],rgb[1],rgb[2], &vh[x],&vs[x],&vb[x] );
 }
 return ny;
}

void CMedit::dragrange( int x0, int x1, enum CMfield field, float y0, float y1, float lerp ) {
  int x;
  float dy;
  if(x0 == x1  || x0 < 0 && x1 < 0  ||  x0 >= cment() && x1 >= cment())
    return;

  dy = (y1 - y0) / (x1 - x0);
  if(x0 < x1)
    for(x = x0; x < x1; x++)
     drag( x, field, y0 + dy*(x-x0), lerp );
   else
    for(x = x0; x > x1; x--)
     drag( x, field, y0 + dy*(x-x0), lerp );
 }


 int CMedit::handle(int ev) {
  // printf("drx: %f\n", wx2drx( Fl::event_x() ));
  // printf("dtx: %f\n", drx2dtx(wx2drx( Fl::event_x() )));
  int x = dtx2cmapx(drx2dtx(wx2drx( Fl::event_x() )));
  // printf("cmapx: %d\n", x);
  float y = wy2y( Fl::event_y() );
  int xmin, xmax;

  switch(ev) {

    case FL_SHORTCUT:
    if(Fl::event_key() == 'u') {
     undo();
     return 1;
   }
   return 0;

   case FL_PUSH:
   dragfrom = x, dragval = y, dragamount = 0;
      // draghue = ( (x - XMIN) * (x - (XMIN/4)) < 0 );/*this was for zooming*/  /* If dragging on hue strip */

   if(Fl::event_state(FL_SHIFT)) {
     dragfield = ALPHA;
   } else {
    // Which button?  Take account of alt/meta modifiers too.

     int btn = 1;

     if(Fl::event_state(FL_BUTTON2 | FL_ALT))
       btn = 2;
     else if(Fl::event_state(FL_BUTTON3 | FL_META))
       btn = 3;

     static CMfield btn2field[2][3] = {
       { RED, GREEN, BLUE },
       { HUE, SAT, BRIGHT }
     };
     dragfield = btn2field[ hsbmode ][ btn-1 ];
   }

   if(Fl::event_key('l')) {
    /*...*/
   } else if(Fl::event_key('r')) {
    /*...*/
   } else {
     snapshot();
   }
      /* Fall into ... */

   case FL_DRAG:
   case FL_RELEASE:

  #ifdef NOTYET
   if(draghue) {
     float h0 = y2hue( y );
     hueshift += (y - dragval) / huezoom;
     huezoom *= (x - dragfrom)
   } else { ... }
  #endif
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
     drag( x, dragfield, y, Fl::event_state(FL_CTRL) ? 1.0 : lerpval );

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




//  int CMedit::handle(int ev) {
//   int x = wx2x( Fl::event_x() );
//   float y = wy2y( Fl::event_y() );
//   int xmin, xmax;

//   switch(ev) {

//     case FL_SHORTCUT:
//     if(Fl::event_key() == 'u') {
//      undo();
//      return 1;
//    }
//    return 0;

//    case FL_PUSH:
//    dragfrom = x, dragval = y, dragamount = 0;
//       draghue = ( (x - XMIN) * (x - (XMIN/4)) < 0 );/*this was for zooming*/	/* If dragging on hue strip */

//    if(Fl::event_state(FL_SHIFT)) {
//      dragfield = ALPHA;
//    } else {
//   	// Which button?  Take account of alt/meta modifiers too.

//      int btn = 1;

//      if(Fl::event_state(FL_BUTTON2 | FL_ALT))
//        btn = 2;
//      else if(Fl::event_state(FL_BUTTON3 | FL_META))
//        btn = 3;

//      static CMfield btn2field[2][3] = {
//        { RED, GREEN, BLUE },
//        { HUE, SAT, BRIGHT }
//      };
//      dragfield = btn2field[ hsbmode ][ btn-1 ];
//    }

//    if(Fl::event_key('l')) {
//   	/*...*/
//    } else if(Fl::event_key('r')) {
//   	/*...*/
//    } else {
//      snapshot();
//    }
//       /* Fall into ... */

//    case FL_DRAG:
//    case FL_RELEASE:

//   #ifdef NOTYET
//    if(draghue) {
//      float h0 = y2hue( y );
//      hueshift += (y - dragval) / huezoom;
//      huezoom *= (x - dragfrom)
//    } else { ... }
//   #endif
//    dragrange( dragfrom, x, dragfield, dragval, y,
//     Fl::event_state(FL_CTRL) ? 1.0 : lerpval );
//    xmin = (dragfrom<x) ? dragfrom : x;
//    xmax = dragfrom+x-xmin;
//    if(damage() == 0) {
//      remin = xmin, remax = xmax;
//    } else {
//      if(remin > xmin) remin = xmin;
//      if(remax < xmax) remax = xmax;
//    }
//    damage(1);
//    if(dragfrom != x)
//      dragamount = 1;
//    if(ev == FL_RELEASE && dragamount == 0)
//      drag( x, dragfield, y, Fl::event_state(FL_CTRL) ? 1.0 : lerpval );

//    dragfrom = x, dragval = y;
//    report( x );
//    return 1;

//    case FL_ENTER:
//    case FL_LEAVE:
//    dragfrom = x;
//    report( x );
//    return 1;

//    case FL_MOVE:
//    dragfrom = x;
//    report( x );
//    return 1;

//  }
//  return 0;
// }

float CMedit::huenear( float hue, float hueref ) {
  float h = hue;
  if(h - hueref > .5f)
    do { h -= 1.0f; } while (h - hueref > .5);
  else
    while(h - hueref < -.5f) h += 1.0f;
  return h;
}

float CMedit::hue2y( float hue ) {
    /* Find closest multiple of given hue to reference y-position y0 */
  return (hue - hueshift) * huezoom;
}

float CMedit::y2hue( float y ) {
  return y / huezoom + hueshift;
}

static float sample( float *a, int ents, float at, int smooth ) {
  if(at <= 0 || ents <= 1) return a[0];
  if(at >= 1) return a[ents-1];
  float eat = at * ents;
  int iat = (int)eat;
  return smooth ? a[iat]*(1 - (eat-iat)) + a[iat+1]*(eat-iat)
  : a[iat];
}

void CMedit::postscale( float s ) {
  postscale_ = s;
}
void CMedit::postexpon( float e ) {
  postexpon_ = e;
}

void CMedit::cment( int newcment ) {
  if(newcment < 1) return;
  if(newcment > CMENTMAX) {
    fprintf(stderr, "Oops, can't ask for more than %d colormap entries -- using %d\n",
     CMENTMAX,CMENTMAX);
    newcment = CMENTMAX;
  }
  if(cment_ == newcment)
    return;

    /* Resample */

  snapshot();

    int smooth = 0; // (cment_ < newcment);
    for(int o = 0; o < newcment; o++) {
      float at = newcment > 1 ? (float)o / (newcment-1) : 0;
      vh[o] = sample( &snap[0][0], cment_, at, smooth );
      vs[o] = sample( &snap[1][0], cment_, at, smooth );
      vb[o] = sample( &snap[2][0], cment_, at, smooth );
      alpha[o] = sample( &snap[3][0], cment_, at, smooth );
    }
    cment_ = newcment;
    remin = 0;
    remax = cment_ - 1;
    lockmin = 0;
    lockmax = cment_ - 1;

    if(cmentcb_ != 0)
      (*cmentcb_)( this );
    report( dragfrom );

    redraw();
  }

  void CMedit::getrgba( int index, float rgba[4] ) const
  {
    if(index < 0 || index >= cment()-1) {
      rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
    } else {
      hsb2rgb( vh[index], vs[index], vb[index], &rgba[0],&rgba[1],&rgba[2] );
      rgba[3] = alpha[index];
    }
  }

  void CMedit::gethsba( int index, float hsba[4] ) const
  {
    if(index < 0 || index >= cment()-1) {
      hsba[0] = hsba[1] = hsba[2] = hsba[3] = 0;
    } else {
      hsba[0] = vh[index];
      hsba[1] = vs[index];
      hsba[2] = vb[index];
      hsba[3] = alpha[index];
    }
  }
//***Range change update functions

  void CMedit::range_update_to( void(*func)( CMedit *cm ) ) {
    range_updater = func;
  }

  void CMedit::updaterange() {
    (*range_updater)( this );
  }

//***END Range change update functions

  void CMedit::reportto( void (*func)( CMedit *cm, int index ) ) {
    reportfunc = func;
  }

  void CMedit::report( int index )
  {
    if(index < 0 || index >= cment() || reportfunc == NULL) 
      return;
    lastx_ = index;

    (*reportfunc)( this, index );
  }

  void CMedit::report()
  {
    CMedit::report( lastx_ );
  }

  float CMedit::Aout( float Ain ) const
  {
    return Ain > 0 ? postscale_ * pow( Ain, postexpon_ ) : 0;
  }

  void colorpatch::draw() {
    glClearColor( vr,vg,vb,va );
    glClear( GL_COLOR_BUFFER_BIT );
  }
