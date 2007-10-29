/* src/main.cc
 * 
 * Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007 by The University of York
 * Author: Paul Emsley
 * Copyright 2007 by The University of Oxford
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */


/* 
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#include <iostream>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


// We are not using NLS yet.
// #ifndef WINDOWS_MINGW
// #define ENABLE_NLS
// #endif
// #ifdef DATADIR
// #endif // DATADIR

#include <gtk/gtk.h>

#undef USE_LIBGLADE // for now

#ifdef USE_LIBGLADE
#include <glade/glade.h>
#endif // USE_LIBGLADE

#include <GL/glut.h> // for glutInit()


#include "interface.h"
#ifndef HAVE_SUPPORT_H
#define HAVE_SUPPORT_H
#include "support.h"
#endif /* HAVE_SUPPORT_H */


#include <sys/types.h> // for stating
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#define PKGDATADIR "C:/coot/share"
#endif

#include "globjects.h"

#include <vector>
#include <string>

#include "mmdb_manager.h"
#include "mmdb-extras.h"
#include "mmdb.h"
#include "mmdb-crystal.h"

#include "Cartesian.h"
#include "Bond_lines.h"

#include "command-line.h"

#include "c-interface.h"
#include "cc-interface.hh"
#include "graphics-info.h"

#include "rgbreps.h"

#include "coot-database.hh"

#ifdef USE_GUILE
#include <guile/gh.h>
#endif 

#if defined (USE_PYTHON)
#include "Python.h" // for Py_Initialize(); 
#define SWIG_init    init_coot
#if defined(_WIN32) || defined(__WIN32__)
#       if defined(_MSC_VER)
#               if defined(STATIC_LINKED)
#                       define SWIGEXPORT(a) a
#                       define SWIGIMPORT(a) extern a
#               else
#                       define SWIGEXPORT(a) __declspec(dllexport) a
#                       define SWIGIMPORT(a) extern a
#               endif
#       else
#               if defined(__BORLANDC__)
#                       define SWIGEXPORT(a) a _export
#                       define SWIGIMPORT(a) a _export
#               else
#                       define SWIGEXPORT(a) a
#                       define SWIGIMPORT(a) a
#               endif
#       endif
#else
#       define SWIGEXPORT(a) a
#       define SWIGIMPORT(a) a
#endif
extern "C" {
SWIGEXPORT(void) SWIG_init(void);
}
#endif

#include "c-inner-main.h"

void show_citation_request();
void load_gtk_resources();
void setup_splash_screen();
void setup_application_icon(GtkWindow *window);
void setup_symm_lib();
void setup_rgb_reps();
void check_reference_structures_dir();
#ifdef USE_MYSQL_DATABASE
#include "mysql/mysql.h"
int setup_database();
#endif

// This happens on the command line now (20070504) -DENABLE_NLS=1
// #if (GTK_MAJOR_VERSION > 1) 
// #define ENABLE_NLS
// #endif

// This main is used for both python/guile useage and unscripted. 
int
main (int argc, char *argv[]) {
   
  GtkWidget *window1;
  GtkWidget *glarea;

  graphics_info_t graphics_info;
  
#ifdef ENABLE_NLS // not used currently in Gtk1. Gkt2, yes.
  // 
  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  // command line
  command_line_data cld = parse_command_line(argc, argv);

  cld.handle_immediate_settings();
  
  if (graphics_info_t::show_citation_notice == 1) { 
     show_citation_request();
  }

  if (graphics_info_t::use_graphics_interface_flag) {
     gtk_set_locale ();    // gtk stuff
     load_gtk_resources();
     gtk_init (&argc, &argv);
     glutInit(&argc, argv);
  }
  
  // popup widget is only filled with graphics at the end of startup
  // which is not what we want.
  // 
  setup_splash_screen();

  
  GtkWidget *splash = NULL;

  if (graphics_info_t::use_graphics_interface_flag) { 
     splash = create_splash_screen_window();
     gtk_widget_show(splash);
  
     // usleep(100000);
     //   std::cout << "There are " << gdk_events_pending()
     // 	    << " pending gdk events\n";
     //   std::cout << "There are " << gtk_events_pending()
     // 	    << " pending gtk events\n";
     while(gtk_main_iteration() == FALSE);
     while (gtk_events_pending()) {
#if !defined(WINDOWS_MINGW) && !defined(_MSC_VER)
	usleep(10000);
#else
	_sleep(10);
#endif
	gtk_main_iteration();
     }
  }
  
  setup_symm_lib();
  setup_rgb_reps();
  check_reference_structures_dir();
#ifdef USE_MYSQL_DATABASE
  setup_database();
#endif  

  // static vector usage
  // and reading in refmac geometry restratints info:
  // 
  graphics_info.init();

  if (graphics_info_t::use_graphics_interface_flag) {

#ifdef USE_LIBGLADE
	
    /* load the interface */
    GladeXML *xml = glade_xml_new("../../coot/coot-gtk2-try2.glade", NULL, NULL);
    /* connect the signals in the interface */
    glade_xml_signal_autoconnect(xml);
    window1 = glade_xml_get_widget(xml, "window1");
    std::cout << "DEBUG:: ..... window1: " << window1 << std::endl;

#else
     window1 = create_window1 ();
#endif // USE_LIBGLADE
     

     // Trying to put a pixmap into the menu bar...
     GtkWidget *reset_view1 = lookup_widget(window1, "reset_view1");
     if (! reset_view1) {
	std::cout << "ERROR:: failed to find reset_view1" << std::endl;
     } else {
	//      GtkWidget *box = gtk_hbox_new(FALSE, 0);
	//      GtkWidget *pixmap = create_pixmap (box, "recentre.xpm");
	//      gtk_box_pack_start (GTK_BOX (box),
	// 			 pixmap, FALSE, FALSE, 3);
	//      gtk_container_add(GTK_CONTAINER(reset_view1), box);
	//      gtk_widget_show(box);
     }

     glarea = gl_extras(lookup_widget(window1, "vbox1"),
			cld.hardware_stereo_flag);
     if (glarea) {
	// application icon:
	setup_application_icon(GTK_WINDOW(window1));
	graphics_info.glarea = glarea; // save it in the static
	
	gtk_widget_show(glarea);
	if (graphics_info_t::glarea_2) 
	   gtk_widget_show(graphics_info_t::glarea_2);
	// and setup (store) the status bar
	GtkWidget *sb = lookup_widget(window1, "main_window_statusbar");
	graphics_info_t::statusbar = sb;
	graphics_info_t::statusbar_context_id =
	   gtk_statusbar_get_context_id(GTK_STATUSBAR(sb), "picked atom info");
	
	gtk_widget_show (window1);
	
	// We need to somehow connect the submenu to the menu's (which are
	// accessible via window1)
	// 
	create_initial_map_color_submenu(window1);
	create_initial_map_scroll_wheel_submenu(window1);
	create_initial_ramachandran_mol_submenu(window1);
	create_initial_sequence_view_mol_submenu(window1);
	
	// old style non-generic functions     
	//      create_initial_validation_graph_b_factor_submenu(window1);
	//      create_initial_validation_graph_geometry_submenu(window1);
	//      create_initial_validation_graph_omega_submenu(window1);

	// OK, these things work thusly:
	//
	// probe_clashes1 is the name of the menu_item set/created in
	// by glade and is in mapview.glade.
	// 
	// probe_submenu is something I make up. It must be the same
	// here and in c-interface-validate.cc's
	// add_on_validation_graph_mol_options()
	//
	// attach a function to the menu item activate function
	// created by glade in callbacks.c
	// (e.g. on_probe_clashes1_activate).  The name that is used
	// there to look up the menu is as above (e.g. probe_clashes1).
	//
	// The type defined there is that checked in
	// c-interface-validate.cc's
	// add_on_validation_graph_mol_options() 
	
	
	create_initial_validation_graph_submenu_generic(window1 , "peptide_omega_analysis1", "omega_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "geometry_analysis1", "geometry_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "temp_fact_variance_analysis1",
							"temp_factor_variance_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "rotamer_analysis1", "rotamer_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "density_fit_analysis1", "density_fit_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "probe_clashes1", "probe_submenu");
	create_initial_validation_graph_submenu_generic(window1 , "gln_and_asn_b_factor_outliers1",
							"gln_and_asn_b_factor_outliers_submenu");
     } else {
	std::cout << "CATASTROPHIC ERROR:: failed to create Gtk GL widget"
		  << "  (Check that your X11 server is working and has (at least)"
		  << "  \"Thousands of Colors\" and supports GLX.)" << std::endl;
     }
  }

  // allocate some memory for the molecules
  //
  std::cout << "initalize graphics molecules...";
  std::cout.flush();
  initialize_graphics_molecules();
  std::cout << "done." << std::endl;
	
  handle_command_line_data(cld);  // and add a flag if listener
	   		          // should be started.
     // which gets looked at later in c_inner_main's make_port_listener_maybe()

     // For the graphics to be able to see the data set in the .coot
     // file, we must put the gl_extras() call after
     // c_wrapper_scm_boot_guile().  So all the graphics
     // e.g. create_window1, gl_extras, and the submenu stuff should go
     // into the c_inner_main (I think)
     //

  if (splash) 
     gtk_widget_destroy(splash);

     // This should not be here, I think.  You can never turn it off -
     // because scripting/guile is not booted until after here - which
     // means its controlling parameter
     // (graphics_info_t::run_state_file_status) can never be changed
     // from the default.  It should be in c-inner-main, after the
     // run_command_line_scripts() call.  
     // 
     // run_state_file_maybe();


#ifdef USE_PYTHON
     //  (on Mac OS, call PyMac_Initialize() instead)
     //http://www.python.org/doc/current/ext/embedding.html
   
     Py_Initialize(); // otherwise it core dumps saying python
     // interpreter not initialized (or something).

     // Now we want to import coot.py/_coot.so (coot load _coot)
     // std::cout << "::::::::::::::::: Python path init_coot" << std::endl;
     init_coot(); // i.e. SWIG_init for python, best we do this before
                  // running .coot.py, eh?


     /* And now read the users own initialization code */

#if defined(WINDOWS_MINGW) || defined(_MSC_VER)
     char *directory = getenv("COOT_HOME");
#else      
     char *directory = getenv("HOME");
#endif

     // First load coot.py, then load the standard startup files, 
     // then load 0-coot.state.py
     
     std::string pydirectory = PKGDATADIR;
     pydirectory += "/python";
     char *pydirectory_cs = getenv("COOT_PYTHON_DIR");
     
     if (pydirectory_cs) { 
	pydirectory = pydirectory_cs;
     }

     // std::cout << "DEBUG:: stating pydirectory " << pydirectory << std::endl;
     
     struct stat buf;
     int status = stat(pydirectory.c_str(), &buf);
     if (status != 0) { 
	std::cout << "WARNING python directory " << pydirectory 
                  << " does not exist" << std::endl;;
     } else { 
         char *coot_dot_py = "coot.py";
         char *coot_dot_py_checked = does_file_exist(pydirectory.c_str(), coot_dot_py);
         if (coot_dot_py_checked) {
	    std::cout << "INFO:: loading coot.py from " << coot_dot_py_checked
		      << std::endl;
	    run_python_script(coot_dot_py_checked);
	    std::cout << "INFO:: coot.py loaded" << std::endl;

             char *coot_load_modules_dot_py = "coot_load_modules.py";
             char *coot_load_modules_dot_py_checked = 
                  does_file_exist(pydirectory.c_str(), coot_load_modules_dot_py);
             if (coot_load_modules_dot_py_checked) { 
                 std::cout << "INFO loading coot python modules" << std::endl;
                 run_python_script(coot_load_modules_dot_py_checked);
             } else { 
               std::cout << "WARNING:: No coot modules found! Python scripting crippled. " 
                         << std::endl;
             } 
         } else { 
            std::cout << "WARNING:: No coot.py file found! Python scripting unavailable. " 
                      << std::endl;
         }
     }
     char *filename = ".coot.py";
     if (directory) { 
        char *check_file = does_file_exist(directory, filename);
        if (check_file) {
	   std::cout << "Loading ~/.coot.py..." << std::endl;
	   run_python_script(check_file);
        }
     }

     // we only want to run one state file if using both scripting
     // languages.  Let that be the guile one.
#ifndef USE_GUILE     
     run_state_file_maybe(); // run local 0-coot.state.py?
#endif // USE_GUILE - not both start-up scripts
     

#endif // USE_PYTHON  
     
#ifdef USE_GUILE
     
     // Must be the last thing in this function, code after it does not get 
     // executed (if we are using guile)
     //
     c_wrapper_scm_boot_guile(argc, argv); 
     //  
#endif

     // to start the graphics, we need to init glut and gtk with the
     // command line args.

#if ! defined (USE_GUILE)
#ifdef USE_PYTHON
     run_command_line_scripts();
     if (graphics_info_t::use_graphics_interface_flag)
	gtk_main ();

#if PY_MAJOR_VERSION > 2 
     else
	Py_Main(0, argv);
#else
#if PY_MINOR_VERSION > 2 
     else
	Py_Main(0, argv);
#endif     // PY_MINOR_VERSION
#endif     // PY_MAJOR_VERSION

#else
     // not python or guile
     if (graphics_info_t::use_graphics_interface_flag)
	gtk_main();
#endif // USE_PYTHON
     
#endif // ! USE_GUILE

     return 0;
}


void load_gtk_resources() {

   std::string gtkrcfile = PKGDATADIR;
   gtkrcfile += "/cootrc";

   // over-ridden by user?
   char *s = getenv("COOT_RESOURCES_FILE");
   if (s) {
      gtkrcfile = s;
   }
  
  std::cout << "Acquiring application resources from " << gtkrcfile << std::endl;
  gtk_rc_add_default_file(gtkrcfile.c_str());

}


/*  ----------------------------------------------------------------------- */
/*            Amusing (possibly) little splash screen                       */
/*  ----------------------------------------------------------------------- */
void
setup_splash_screen() { 

   // default location:
   std::string splash_screen_pixmap_dir = PKGDATADIR;  
   splash_screen_pixmap_dir += "/";
   splash_screen_pixmap_dir += "pixmaps";

   // over-ridden by user?
   char *s = getenv("COOT_PIXMAPS_DIR");
   if (s) {
      splash_screen_pixmap_dir = s;
   }
   
   std::cout << "INFO:: splash_screen_pixmap_dir " 
	     << splash_screen_pixmap_dir << std::endl;
   // now add splash_screen_pixmap_dir to the pixmaps_directories CList
   //
   add_pixmap_directory(splash_screen_pixmap_dir.c_str());

}

void setup_application_icon(GtkWindow *window) { 
      
   std::string splash_screen_pixmap_dir = PKGDATADIR;  
   splash_screen_pixmap_dir += "/";
   splash_screen_pixmap_dir += "pixmaps";

   // over-ridden by user?
   char *s = getenv("COOT_PIXMAPS_DIR");
   if (s) {
      splash_screen_pixmap_dir = s;
   }

   // now add the application icon
   std::string app_icon_path =
      coot::util::append_dir_file(splash_screen_pixmap_dir,
				  "coot-icon.png");

   struct stat buf;
   int status = stat(app_icon_path.c_str(), &buf); 
   if (status == 0) { // icon file was found

#if (GTK_MAJOR_VERSION == 2)
      GdkPixbuf *icon_pixbuf =
	 gdk_pixbuf_new_from_file (app_icon_path.c_str(), NULL);
      if (icon_pixbuf) {
	 gtk_window_set_icon (GTK_WINDOW (window), icon_pixbuf);
	 gdk_pixbuf_unref (icon_pixbuf);
      }

#else
      // gtk 1, use:
      // 
      // void gdk_window_set_icon(GdkWindow	  *window, 
      // 				GdkWindow	  *icon_window,
      // 				GdkPixmap	  *pixmap,
      // 				GdkBitmap	  *mask);

      // Argh. How do I make a bitmap from a file?  This is just too
      // hard.  Give up.
      // GdkBitmap *icon = ...(app_icon_path.c_str());
      // gtk_window_set_icon(window, window, icon, icon);


//       /* load a pixmap from a file */
//       GdkBitmap *mask;
//       GtkStyle *style = gtk_widget_get_style( window->default_widget );

//       GdkPixmap *pixmap = gdk_pixmap_create_from_xpm( window, &mask,
// 						      &style->bg[GTK_STATE_NORMAL],
// 						      app_icon_path.c_str());
//       GtkWidget *pixmapwid = gtk_pixmap_new( pixmap, mask );
//       gtk_widget_show( pixmapwid );
//       gtk_container_add( GTK_CONTAINER(window), pixmapwid );

      
#endif
   }
}


void 
show_citation_request() { 

   std::cout << "\n   If have found this software to be useful, you are requested to cite:\n"
	     << "   Coot: model-building tools for molecular graphics" << std::endl;
   std::cout << "   Emsley P, Cowtan K" << std::endl;
   std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY\n";
   std::cout << "   60: 2126-2132 Part 12 Sp. Iss. 1 DEC 2004\n\n";

   std::cout << "   The reference for the REFMAC5 Dictionary is:\n";
   std::cout << "   REFMAC5 dictionary: organization of prior chemical knowledge and\n"
	     << "   guidelines for its use" << std::endl;
   std::cout << "   Vagin AA, Steiner RA, Lebedev AA, Potterton L, McNicholas S,\n"
	     << "   Long F, Murshudov GN" << std::endl;
   std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY " << std::endl;
   std::cout << "   60: 2184-2195 Part 12 Sp. Iss. 1 DEC 2004" << std::endl;

#ifdef HAVE_SSMLIB
    std::cout << "\n   If using \"SSM Superposition\", please cite:\n";

    std::cout << "   Secondary-structure matching (SSM), a new tool for fast\n"
	      << "   protein structure alignment in three dimensions" << std::endl;
    std::cout << "   Krissinel E, Henrick K" << std::endl;
    std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY" << std::endl;
    std::cout << "   60: 2256-2268 Part 12 Sp. Iss. 1 DEC 2004\n" << std::endl;
#endif // HAVE_SSMLIB
    
}


void setup_symm_lib() {

   // How should the setting up of symmetry work?
   //
   // First we check the environment variable SYMINFO.
   // 
   // If that is not set, then we look in the standard (hardwired at
   // compile time) place
   //
   // If both these fail then give an error message. 

   char *syminfo = getenv("SYMINFO");
   if (!syminfo) { 

      std::string symstring("SYMINFO=");

      // using PKGDATADIR will work for those who compiler, not the
      // binary users:
      std::string standard_file_name = PKGDATADIR; // xxx/share/coot
      standard_file_name += "/"; 
      standard_file_name += "syminfo.lib";

      struct stat buf;
      int status = stat(standard_file_name.c_str(), &buf); 
      if (status != 0) { // standard-residues file was not found in default location 

	 // This warning is only sensible for those who compile (or
	 // fink).  So let's test if SYMINFO was set before we write it
	 //
	 std::cout << "WARNING:: Symmetry library not found at "
		   << standard_file_name
		   << " and environment variable SYMINFO is not set." << std::endl;
	 std::cout << "WARNING:: Symmetry will not be possible\n";
	 
      } else { 

	 symstring += standard_file_name;

	 // Mind bogglingly enough, the string is part of the environment
	 // and malleable, so const char * of a local variable is not
	 // what we want at all.  
	 // 
	 //  We fire and forget, we don't want to change s.
	 // 
	 char * s = new char[symstring.length() + 1];
	 strcpy(s, symstring.c_str());
	 putenv(s);
	 // std::cout << "DEBUG:: SYMINFO set/not set? s is " << s <<std::endl;
      }
   }
}

void setup_rgb_reps() {

   // c.f. coot::package_data_dir() which now does this wrapping for us:
   
   // For binary installers, they use the environment variable:
   char *env = getenv("COOT_DATA_DIR");

   // Fall-back:
   std::string standard_file_name = PKGDATADIR; // xxx/share/coot

   if (env)
      standard_file_name = env;

   std::string colours_file = standard_file_name + "/";
   std::string colours_def = "colours.def";
   colours_file += colours_def;

   struct stat buf;
   int status = stat(colours_file.c_str(), &buf); 
   if (status == 0) { // colours file was found in default location
      RGBReps r(colours_file);
      std::cout << "INFO:: Colours file: " << colours_file << " loaded"
		<< std::endl;

      // test:
//       std::vector<std::string> test_col;
//       test_col.push_back("blue");
//       test_col.push_back("orange");
//       test_col.push_back("magenta");
//       test_col.push_back("cyan");

//       std::vector<int> iv = r.GetColourNumbers(test_col);
//       for (int i=0; i<iv.size(); i++) {
// 	 std::cout << "Colour number: " << i << "  " << iv[i] << std::endl;
//       }
      
      
   } else {
      std::cout << "WARNING! Can't find file: colours.def at " << standard_file_name
		<< std::endl;
   }
}


void check_reference_structures_dir() {

   char *coot_reference_structures = getenv("COOT_REF_STRUCTS");
   if (coot_reference_structures) {
      struct stat buf;
      int status = stat(coot_reference_structures, &buf); 
      if (status != 0) { // file was not found in default location either
	 std::cout << "WARNING:: The reference structures directory "
		   << "(COOT_REF_STRUCTS): "
		   << coot_reference_structures << " was not found." << std::endl;
	 std::cout << "          Ca->Mainchain will not be possible." << std::endl;
      }
   } else {

      // check in the default place: pkgdatadir = $prefix/share/coot
      std::string pkgdatadir = PKGDATADIR;
      std::string ref_structs_dir = pkgdatadir;
      ref_structs_dir += "/";
      ref_structs_dir += "reference-structures";
      struct stat buf;
      int status = stat(ref_structs_dir.c_str(), &buf); 
      if (status != 0) { // file was not found in default location either
	 std::cout << "WARNING:: No reference-structures found (in default location)."
		   << "          and COOT_REF_STRUCTS was not defined." << std::endl;
	 std::cout << "          Ca->Mainchain will not be possible." << std::endl;
      }
   }

}

#ifdef USE_MYSQL_DATABASE
int setup_database() {

   const char *host = "localhost";
   const char *user = "cootuser";
   const char *passwd = "password";

   const char *db = "cootsessions";
   unsigned int port = 0;
   const char *unix_socket = 0; 
   unsigned long client_flag = 0;

   // graphics_info_t::mysql = new MYSQL; // not needed.
   graphics_info_t::mysql = mysql_init(graphics_info_t::mysql);

   if (graphics_info_t::mysql) { 
      graphics_info_t::mysql = mysql_real_connect(graphics_info_t::mysql,
						  host, user, passwd, db, port,
						  unix_socket, client_flag);

      // We need to do a test here to see if that connection worked.
      // 
      if (graphics_info_t::mysql == 0) {

	 std::cout << "INFO:: Can't connect to database." << std::endl;

      } else { 

	 // We only want to do this if this person doesn't exist
	 // already...
	 std::string query("insert into user (userid, username) values");
	 std::pair<std::string, std::string> p = coot::get_userid_name_pair();
	 graphics_info_t::db_userid_username = p;
	 query += " ('";
	 query += p.first;
	 query += "','";
	 query += p.second;
	 query += "');";
	 unsigned long length = query.length();
      
	 int v = mysql_real_query(graphics_info_t::mysql,
				  query.c_str(), length);

	 if (v != 0) {
	    if (v == CR_COMMANDS_OUT_OF_SYNC)
	       std::cout << "WARNING:: MYSQL Commands executed in an"
			 << " improper order" << std::endl;
	    if (v == CR_SERVER_GONE_ERROR) 
	       std::cout << "WARNING:: MYSQL Server gone!"
			 << std::endl;
	    if (v == CR_SERVER_LOST) 
	       std::cout << "WARNING:: MYSQL Server lost during query!"
			 << std::endl;
	    if (v == CR_UNKNOWN_ERROR) 
	       std::cout << "WARNING:: MYSQL Server transaction had "
			 << "an uknown error!" << std::endl;
	 }
	 // std::cout << "start: mysql_real_query returned " << v << std::endl;

	 // set the session-id
	 time_t *timep = new time_t;
	 *timep = time(0); 
	 char *ct = ctime(timep);
	 delete timep;
	 std::string sct(ct);
	 // now remove the irritating carriage return at the end of the
	 // ctime string
	 std::string::size_type icarr = sct.find("\n");
	 if (icarr != std::string::npos) {
	    sct = sct.substr(0,icarr);
	 } 
	 std::string sessionid("session-");
	 sessionid += sct;
	 graphics_info_t::sessionid += sessionid;
      }

   } else {
      std::cout << "WARNING:: can't init mysql database structure"
		<< std::endl;
   }
}
#endif 
