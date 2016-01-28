
#ifdef USE_PYTHON
#include <Python.h>  // before system includes to stop "POSIX_C_SOURCE" redefined problems
#endif

// #define RDKIT_HAS_CAIRO_SUPPORT

#ifdef MAKE_ENHANCED_LIGAND_TOOLS
#ifdef RDKIT_HAS_CAIRO_SUPPORT
#include <cairo.h>
#include <MolDraw2DCairo.h>
#include "lidia-core/rdkit-interface.hh"

#else
#include "lidia-core/rdkit-interface.hh"


#endif // RDKIT_HAS_CAIRO_SUPPORT

#include "utils/coot-utils.hh"
#endif // MAKE_ENHANCED_LIGAND_TOOLS

#include "graphics-info.h"
#include "globjects.h" //includes gtk/gtk.h


#include "c-interface-image-widget.hh"

GtkWidget *test_get_image_widget_for_comp_id(const std::string &comp_id) {

   GtkWidget *r = 0;

#ifdef RDKIT_HAS_CAIRO_SUPPORT
   
   std::string smiles="CO[C@@H](O)C1=C(O[C@H](F)Cl)C(C#N)=C1ONNC[NH3+]";
   RDKit::ROMol *m_local = RDKit::SmilesToMol(smiles);
   TEST_ASSERT(m_local);
   RDDepict::compute2DCoords(*m_local);
   WedgeMolBonds(*m_local,&(m_local->getConformer()));
   std::string png_file_name = "image-" + comp_id + ".png";

   { 
      RDKit::MolDraw2DCairo drawer(300,300);
      drawer.drawMolecule(*m_local);
      drawer.finishDrawing();
      std::string dt = drawer.getDrawingText();
      // std::cout << "PE-debug drawing-text :" << dt << ":" << std::endl;
      std::cout << "drawingtext is of length " << dt.length() << std::endl;
      drawer.writeDrawingText(png_file_name.c_str());
   }
#endif // RDKIT_HAS_CAIRO_SUPPORT
   
   return r;
}


GtkWidget *get_image_widget_for_comp_id(const std::string &comp_id) {

   GtkWidget *r = 0;

#ifdef MAKE_ENHANCED_LIGAND_TOOLS
#ifdef RDKIT_HAS_CAIRO_SUPPORT

   graphics_info_t g;
   g.Geom_p()->try_dynamic_add(comp_id, g.cif_dictionary_read_number++);
   std::pair<bool, coot::dictionary_residue_restraints_t> dict =
      g.Geom_p()->get_monomer_restraints(comp_id);
   
   if (dict.first) {

      try { 
	 RDKit::RWMol m = rdkit_mol(dict.second);

	 RDKit::MolOps::removeHs(m);
	 
	 int iconf_2d = RDDepict::compute2DCoords(m);
	 std::cout << "iconf_2d is " << iconf_2d << std::endl;
	 WedgeMolBonds(m, &(m.getConformer(iconf_2d)));

	 std::string smb = RDKit::MolToMolBlock(m, true, iconf_2d);
	 std::string fn = "test-" + comp_id + ".mol";
	 std::ofstream f(fn.c_str());
	 if (f)
	    f << smb << std::endl;
	 f.close();

	 
	 if (true) {
	    // try old style, write to a PIL

	 } 
	 
	 int n_conf = m.getNumConformers();
	 std::cout << "n_conf for " << comp_id << " is " << n_conf << std::endl;
	 if (n_conf > 0) {

	    std::string png_file_name = "image-" + comp_id + ".png";

	    { 
	       RDKit::MolDraw2DCairo drawer(250, 250);
	       drawer.drawMolecule(m);
	       drawer.finishDrawing();
	       std::string dt = drawer.getDrawingText();
	       // std::cout << "PE-debug drawing-text :" << dt << ":" << std::endl;
	       drawer.writeDrawingText(png_file_name.c_str());
	    }

	    
	    if (coot::file_exists(png_file_name)) { 
	       std::cout << "calling gtk_image_new_from_file() "  << std::endl;
	       r = gtk_image_new_from_file(png_file_name.c_str());
	    } else {
	       std::cout << "png file name " << png_file_name << " does not exist "
			 << std::endl;
	    }
	 }
      }
      catch (...) {
	 std::cout << "hack caught an exception " << std::endl;
      }
	 
   } else {
      std::cout << "No dictionary for rdkit_mol from " << comp_id << std::endl;
   }
   
#endif   // RDKIT_HAS_CAIRO_SUPPORT
#endif   // MAKE_ENHANCED_LIGAND_TOOLS
   return r;
}
