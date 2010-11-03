
//----------------------------  template.cc  ---------------------------
//    $Id$
//    Version: $Name$
//
//    Copyright (C) 2005, 2008, 2010 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------  template.cc  ---------------------------


// like grid_in_out but write in gnuplot format

#include "../tests.h"
#include <fstream>
#include <base/logstream.h>
#include <grid/tria.h>
#include <grid/grid_in.h>
#include <grid/grid_out.h>

#include <string>

std::ofstream logfile("grid_in_out_02/output");

template <int dim, int spacedim>
void test(std::string filename) {
    Triangulation<dim, spacedim> tria;
    GridIn<dim, spacedim> gi;
    gi.attach_triangulation (tria);
    std::ifstream in (filename.c_str());
    gi.read_ucd (in);

    GridOut grid_out;
    grid_out.write_gnuplot (tria, logfile);
}

int main ()
{
  deallog.attach(logfile);
  deallog.depth_console(0);

  test<2,3>("grids/square.inp");
  test<2,3>("grids/sphere_1.inp");

  return 0;
}

