#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <ctype.h>
#undef isspace

#include "cmed.H"

struct _cshow  cshow;


static int cleanup(char *buf, int room, const char *str) {
  char *p;
  const char *q;
  p = buf;
  for(q = str; *q && isspace(*q); q++) ;
  for( ; *q && !isspace(*q) && p < &buf[room-1]; q++)
    *p++ = *q;
  *p = '\0';
  return(p > buf);
}

//TESTING AREA ============================================================

//*****This is for Histogram File field and the pop up file browser.

//***Globals
// Fl_Input    *filter;
Fl_File_Browser   *files;
Fl_File_Chooser   *fc;
// Fl_Shared_Image   *image = 0;

//***END Globals

//***Functions for file browsers

void
fc_callback(Fl_File_Chooser *fc,  // I - File chooser
            void            *data)  // I - Data
{
  const char    *filename;  // Current filename


  printf("fc_callback(fc = %p, data = %p)\n", fc, data);

  filename = fc->value();

  printf("    filename = \"%s\"\n", filename ? filename : "(null)");
}

void show_callback(void)
{
  int i;      // Looping var
  int count;      // Number of files selected
  char  relative[FL_PATH_MAX];  // Relative filename


  // if (filter->value()[0])
    // fc->filter(filter->value());

  fc->show();

  while (fc->visible()) {
    Fl::wait();
  }

  // histo_file->value(fc->value());

  // count = fc->count();
  // if (count > 0) {
  //   files->clear();

  //   for (i = 1; i <= count; i ++)
  //   {
  //     if (!fc->value(i))
  //       break;

  //     fl_filename_relative(relative, sizeof(relative), fc->value(i));

  //     files->add(relative,
  //                Fl_File_Icon::find(fc->value(i), Fl_File_Icon::PLAIN));
  //   }

    // files->redraw();
  // }
}

void B_callback(Fl_Button*, void*){
  fc = new Fl_File_Chooser::Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Fl_File_Chooser Test");
  fc->callback(fc_callback);
  show_callback();
}

//***END Functions for file browsers



//*****END for Histogram File field and the pop up file browser.


//*****This is for put buttons and options into a menu thingy.


//***Functions for colormap files
void menu_fload_cb( Fl_Menu_*, void * ) {
  FILE *inf;
  // char fname[256];
  const char* fname;

  fc = new Fl_File_Chooser::Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Fl_File_Chooser Test");
  fc->callback(fc_callback);
  show_callback();

  // while (fc->visible()) {
  //   Fl::wait();
  // }

  fname = fc->value();
  if (fname == NULL) {
    return;
  }

  if((inf = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "%s: can't open: ", fname);
    perror("");
    return;
  }

  cmedit->snapshot();

  if(!cmedit->fload( inf )) {
    fprintf(stderr, "%s: can't read colormap\n", fname);
    // fnamebox->insert(" [?]");
  }
  cmedit->redraw();
  fclose(inf);
}

void menu_fsave_cb( Fl_Menu_*, void * ) {
  FILE *outf;
  // char fname[256];

  const char* fname;

  fc = new Fl_File_Chooser::Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Fl_File_Chooser Test");
  fc->type(fc->type() ^ Fl_File_Chooser::CREATE);
  fc->callback(fc_callback);
  show_callback();

  // while (fc->visible()) {
  //   Fl::wait();
  // }

  fname = fc->value();
  if (fname == NULL) {
    return;
  }

  if((outf = fopen(fname, "r")) != NULL) {
    // fprintf(stderr, "%s: can't create: ", fname);
    // perror("");
    // return;
    int overwrite = fl_ask("File already exists. Wanna overwrite?");
    printf("%d\n", overwrite);
    if(overwrite==0) {
      fclose(outf);
      printf("%s\n", "QUIT!!");
      return;
    }
    else {
      outf = fopen(fname, "w");
    }
  }

  else {
    outf = fopen(fname, "w");
  }

  if( cmedit->fsave( outf ) ) {
    fprintf(stderr, "%s: saved\n", fname);
  } else {
    fprintf(stderr, "%s: write error: ", fname);
    perror("");
  }
  cmedit->redraw();
  fclose(outf);
}

//***END Functions for colormap files

//***Functions for histogram files

void menu_histfload_cb( Fl_Menu_*, void*) {

  FILE *inf;
  // char fname[256];
  const char* fname;

  fc = new Fl_File_Chooser::Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Fl_File_Chooser Test");
  fc->callback(fc_callback);
  show_callback();

  fname = fc->value();
  if (fname == NULL) {
    return;
  }

  if((inf = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "%s: can't open: ", fname);
    perror("");
    return;
  }    

//snapshot???

  if(!cmedit->histfload( inf )) {
    fprintf(stderr, "%s: can't read histogram\n", fname);
  }
  cmedit->redraw();
  fclose(inf);

}


//***Functions for histogram files

//*****END for put buttons and options into a menu thingy.


//*****This is for range changes




//*****END This is for range changes

void x_hist_range_min_box_cf(Fl_Output* op, void*) {
  float hist_min_val = cmedit->get_hist_data_x_min();
  char str[32];
  sprintf(str, "%g", hist_min_val);
  op->value(str);

}

void x_hist_range_max_box_cf(Fl_Output* op, void*) {
  float hist_max_val = cmedit->get_hist_data_x_max();
  char str[32];
  sprintf(str, "%g", hist_max_val);
  op->value(str);
}

void y_hist_range_max_box_cf(Fl_Output* op, void*) {
  float hist_max_val = cmedit->get_hist_data_y_max();
  char str[32];
  sprintf(str, "%g", hist_max_val);
  op->value(str);
}

void y_hist_range_min_box_cf(Fl_Output* op, void*) {
  float hist_min_val = cmedit->get_hist_data_y_min();
  char str[32];
  sprintf(str, "%g", hist_min_val);
  op->value(str);
}

void x_cmap_range_min_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_x_min_for_cmap(inp->value());
  cmedit->redraw();
}

void x_cmap_range_max_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_x_max_for_cmap(inp->value());
  cmedit->redraw();
}

void y_cmap_range_max_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_y_max_for_cmap(inp->value());
  cmedit->redraw();
}

void y_cmap_range_min_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_y_min_for_cmap(inp->value());
  cmedit->redraw();
}

void x_display_range_min_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_x_min_for_display(inp->value());
  cmedit->redraw();
}

void x_display_range_max_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_x_max_for_display(inp->value());
  cmedit->redraw();
}

void y_display_range_max_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_y_max_for_display(inp->value());
  cmedit->redraw();
}

void y_display_range_min_box_cb(Fl_Value_Input* inp, void*) {
  cmedit->set_data_y_min_for_display(inp->value());
  cmedit->redraw();
}

void x_display_range_min_slider_cb(Fl_Slider* fl, void*) {
  // fl->bounds(cmedit->get_hist_data_x_min(), (cmedit->get_hist_data_x_max() - cmedit->get_hist_data_x_min()) / 2);
  fl->bounds(cmedit->get_hist_data_x_min(), cmedit->get_hist_data_x_max() );
  if( fl->value() < x_display_range_max_slider->value() ) {
    cmedit->set_data_x_min_for_display(fl->value());
  }
  else {

  }

  x_display_range_min_box->value(fl->value());
  cmedit->redraw();
}

void x_display_range_max_slider_cb(Fl_Slider* fl, void*) {
  // fl->bounds( (cmedit->get_hist_data_x_max() - cmedit->get_hist_data_x_min()) / 2, cmedit->get_hist_data_x_max() );
  fl->bounds(cmedit->get_hist_data_x_min(), cmedit->get_hist_data_x_max() );
  if( fl->value() > x_display_range_min_slider->value() ){
    cmedit->set_data_x_max_for_display(fl->value());
  }
  else {

  }
  
  x_display_range_max_box->value(fl->value());
  cmedit->redraw();
}

void y_cmap_range_max_slider_cb(Fl_Slider* fl, void*) {
  fl->bounds( cmedit->get_hist_data_y_max(),(cmedit->get_hist_data_y_max() - cmedit->get_data_y_min_for_cmap() ) / 2 );
  cmedit-> set_data_y_max_for_cmap(fl->value());
  y_cmap_range_max_box->value(fl->value());
  cmedit->redraw();
}

void y_cmap_range_min_slider_cb(Fl_Slider* fl, void*) {
  fl->bounds( (cmedit->get_hist_data_y_max() - cmedit->get_hist_data_y_min() ) / 2, cmedit->get_hist_data_y_min() );
  cmedit-> set_data_y_min_for_cmap(fl->value());
  y_cmap_range_min_box->value(fl->value());
  cmedit->redraw();
}

void y_display_range_max_slider_cb(Fl_Slider* fl, void*) {
  // fl->bounds( cmedit->get_hist_data_y_max(),(cmedit->get_hist_data_y_max() - cmedit->get_data_y_min_for_cmap() ) / 2 );
  fl->bounds( cmedit->get_hist_data_y_max(), cmedit->get_hist_data_y_min() );

  cmedit->set_data_y_max_for_display(fl->value());
  y_display_range_max_box->value(fl->value());
  cmedit->redraw();
}

void y_display_range_min_slider_cb(Fl_Slider* fl, void*) {
  fl->bounds( (cmedit->get_hist_data_y_max() - cmedit->get_hist_data_y_min() ) / 2, cmedit->get_hist_data_y_min() );
  cmedit->set_data_y_min_for_display(fl->value());
  y_display_range_min_box->value(fl->value());
  cmedit->redraw();
}


void R_updater (CMedit* cm) {
  x_hist_range_min_box_cf(x_hist_range_min_box, NULL);
  x_hist_range_max_box_cf(x_hist_range_max_box, NULL);
  y_hist_range_min_box_cf(y_hist_range_min_box, NULL);
  y_hist_range_max_box_cf(y_hist_range_max_box, NULL);
  x_cmap_range_min_box->value(cm->get_data_x_min_for_cmap());
  x_cmap_range_max_box->value(cm->get_data_x_max_for_cmap());
  y_cmap_range_min_box->value(cm->get_data_y_min_for_cmap());
  y_cmap_range_max_box->value(cm->get_data_y_max_for_cmap());
  x_display_range_min_box->value(cm->get_data_x_min_for_display());
  x_display_range_max_box->value(cm->get_data_x_max_for_display());
  y_display_range_min_box->value(cm->get_data_y_min_for_display());
  y_display_range_max_box->value(cm->get_data_y_max_for_display());
  cm->redraw();
}

//******Zoom Panel Stuffs============//

void zoom_panel_cb(Fl_Menu_*, void*) {
  zoom_panel_window->show();
}




//******END Zoom Panel Stuffs============//


//END TESTING AREA ============================================================






void input_cb( Fl_Input *inp, void * ) {
  int nents = cmedit->cment();
  if(sscanf(inp->value(), "cments %d", &nents) > 0
     || sscanf(inp->value(), "cment %d", &nents) > 0) {

    cmedit->cment( nents );

  }
}

/**void fload_cb( Fl_Button *, void * ) {
  FILE *inf;
  char fname[256];
cshow
  fnamebox->position(strlen(fnamebox->value()), strlen(fnamebox->value()));
  Fl::focus(fnamebox);

  if(!cleanup(fname, sizeof(fname), fnamebox->value())) {
    fnamebox->value("Type input file name here");
    return;
  }

  if((inf = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "%s: can't open: ", fname);
    perror("");
    fnamebox->insert(" [?]");
    return;
  }

  cmedit->snapshot();

  if(!cmedit->fload( inf )) {
    fprintf(stderr, "%s: can't read colormap\n", fname);
    fnamebox->insert(" [?]");
  }
  cmedit->redraw();
  fclose(inf);
}

void fsave_cb( Fl_Button *savebutton, void * ) {
  FILE *outf;
  char fname[256];

  if(!cleanup(fname, sizeof(fname), fnamebox->value())) {
    fnamebox->value("Type output file name here");
    fnamebox->position(0, strlen(fnamebox->value()));
    Fl::focus(fnamebox);
    return;
  }

  if((outf = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "%s: can't create: ", fname);
    perror("");
    fnamebox->position( strlen(fnamebox->value()) );
    fnamebox->insert(" [?]");
    return;
  }

  if( cmedit->fsave( outf ) ) {
    fprintf(stderr, "%s: saved\n", fname);
  } else {
    fprintf(stderr, "%s: write error: ", fname);
    perror("");
  }
  fclose(outf);
}**/

void undo_cb( Fl_Button *, void * ) {
  cmedit->undo();
}

void menu_undo_cb( Fl_Menu_ *, void * ) {
  cmedit->undo();
}

void report_cb( Fl_Value_Input *cindex, void * ) {
  cmedit->report( (int) cindex->value() );
}

void lerp_cb( Fl_Slider *sl, void * ) {
  cmedit->lerpval = sl->value();
}

void rgbmode_cb( Fl_Button *btn, void * ) {
  static char *btnlbl[2] = { "RGB", "HSB" };
  static char *hsblbls[3][2] = {
		{ "Red(L)", "Hue(L)" },
		{ "Green(M)", "Sat(M)" },
		{ "Blue(R)", "Bright(R)" } };
  cmedit->hsbmode = !cmedit->hsbmode;
  cmedit->redraw();
  btn->label( btnlbl[ cmedit->hsbmode ] );
  btn->redraw();
  cshow.hsblbls[0]->labelcolor( cmedit->hsbmode ? 3/*yellow*/ : 1/*red*/ );

  for(int i = 0; i < 3; i++) {
    cshow.hsblbls[i]->label( hsblbls[i][cmedit->hsbmode] );
    cshow.hsblbls[i]->redraw();
  }
}

void reporter( CMedit *cm, int x )
{
  char msg[64];
  float hsba[4], rgba[4];

  if(cshow.cindex == NULL) return;
  cm->gethsba( x, hsba );
  cm->getrgba( x, rgba );
  cshow.cindex->value( x );
  sprintf(msg, "%.3f %.3f %.3f %.3f", hsba[0],hsba[1],hsba[2],hsba[3]);
  cshow.hsba->value( msg );
  sprintf(msg, "%.3f %.3f %.3f", rgba[0],rgba[1],rgba[2]);
  cshow.rgba->value( msg );
  cshow.color->rgba( rgba[0], rgba[1], rgba[2], rgba[3] );
  cshow.color->redraw();
  sprintf(msg, "%g", cm->Aout( rgba[3] ) );
  cshow.scaleout->value(msg);
  if(cshow.cmentin->value() != cm->cment())
    cshow.cmentin->value( cm->cment() );
  if(cshow.postscalein->value() != cm->postscale())
    cshow.postscalein->value( cm->postscale() );
  if(cshow.postexponin->value() != cm->postexpon())
    cshow.postexponin->value( cm->postexpon() );
}

void cmenter( const CMedit *cm )
{
  if(cshow.cmentin->value() != cm->cment())
    cshow.cmentin->value( cm->cment() );
}

void ascale_cb( Fl_Value_Input*, void* ) {

  if(cshow.postscalein->value() != cmedit->postscale() ||
     cshow.postexponin->value() != cmedit->postexpon()) {

    cmedit->postscale( cshow.postscalein->value() );
    cmedit->postexpon( cshow.postexponin->value() );
    cmedit->report();
  }
}

void ncment_cb( Fl_Value_Input *inp, void * )
{
  cmedit->cment( (int) (inp->value() + 0.5) );
}
  
void quietwarning( const char *fmt, ... ) {
  char msg[10240];
  static char avoid[] = "X_ChangeProperty: ";
  va_list args;
  va_start(args, fmt);
  vsprintf(msg, fmt, args);
  va_end(args);
  if(0!=strncmp(msg, avoid, sizeof(avoid)-1))
    fputs(msg, stderr);
}

int main(int argc, char *argv[]) {




  Fl::warning = quietwarning;

  Fl_Window *top = make_window();

  zoom_panel_window = make_zoom_panel();


  cmedit->reportto( reporter );
  cmedit->cmentto( cmenter );

  cmedit->range_update_to( R_updater );
  cmedit->updaterange();

  cshow.postscalein->value(1);
  cshow.postexponin->value(1);

  if(argc>2 && !strcmp(argv[1], "-e")) {
    cmedit->cment( atoi(argv[2]) );
    argc -= 2, argv += 2;
  }
  if(argc>1) {
    // fnamebox->value( argv[argc-1] );
    // fload_cb( loadbtn, NULL );
  }
  top->show(0, argv);
  cmedit->show();
  return Fl::run();
}
