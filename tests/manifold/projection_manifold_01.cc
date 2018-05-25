// ---------------------------------------------------------------------
//
// Copyright (C) 2016 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------

// Test that a Manifold constructed using only the projection works.

#include "../tests.h"


// all include files you need here
#include <deal.II/base/std_cxx14/memory.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

template <int dim, int spacedim>
class MyManifold : public Manifold<dim, spacedim>
{
public:
  Point<spacedim>
  project_to_manifold(const ArrayView<const Point<spacedim>> &vertices,
                      const Point<spacedim> &candidate) const override
  {
    // Shift the y coordinate to 4*x*(1-x)
    Point<spacedim> p = candidate;
    if (spacedim > 1)
      p[1] = 4 * p[0] * (1 - p[0]);
    return p;
  }

  std::unique_ptr<Manifold<dim, spacedim>>
  clone() const override
  {
    return std_cxx14::make_unique<MyManifold<dim, spacedim>>();
  }
};

// Helper function
template <int dim, int spacedim>
void
test(unsigned int ref = 1)
{
  deallog << "Testing dim=" << dim << ", spacedim=" << spacedim << std::endl;
  MyManifold<dim, spacedim> manifold;

  // Test that we get the right thing with two simple points.
  Point<spacedim> p0, p1;
  p1[0] = 1.0;

  Point<spacedim> p2 = manifold.get_intermediate_point(p0, p1, .5);
  deallog << "p2(0.5): " << p2 << std::endl;

  p2 = manifold.get_intermediate_point(p0, p1, .2);
  deallog << "p2(0.2): " << p2 << std::endl;
}

int
main()
{
  initlog();

  test<1, 1>();
  test<1, 2>();
  test<2, 2>();
  test<2, 3>();
  test<3, 3>();

  return 0;
}
