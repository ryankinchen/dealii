// ---------------------------------------------------------------------
//
// Copyright (C) 2013 - 2017 by the deal.II authors
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



// correctness of matrix-free initialization when the mapping changes and the
// indices are not updated, only the mapping info

#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/mapping_q_eulerian.h>

#include "../tests.h"


std::ofstream logfile("output");

#include "get_functions_common.h"


template <int dim, int fe_degree>
void
test()
{
  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria);
  tria.refine_global(1);

  FE_Q<dim>       fe(fe_degree);
  DoFHandler<dim> dof(tria);
  dof.distribute_dofs(fe);

  ConstraintMatrix constraints;
  constraints.close();

  deallog << "Testing " << dof.get_fe().get_name() << std::endl;
  // use this for info on problem
  // std::cout << "Number of cells: " <<
  // dof.get_triangulation().n_active_cells()
  //          << std::endl;
  // std::cout << "Number of degrees of freedom: " << dof.n_dofs() << std::endl;
  // std::cout << "Number of constraints: " << constraints.n_constraints() <<
  // std::endl;

  Vector<double> solution(dof.n_dofs());

  // create vector with random entries
  for (unsigned int i = 0; i < dof.n_dofs(); ++i)
    {
      if (constraints.is_constrained(i))
        continue;
      const double entry = random_value<double>();
      solution(i)        = entry;
    }

  constraints.distribute(solution);

  FESystem<dim>   fe_sys(dof.get_fe(), dim);
  DoFHandler<dim> dofh_eulerian(dof.get_triangulation());
  dofh_eulerian.distribute_dofs(fe_sys);

  MatrixFree<dim, double> mf_data;
  Vector<double>          shift(dofh_eulerian.n_dofs());
  for (unsigned int i = 0; i < 2; ++i)
    {
      if (i == 1)
        {
          shift(0) = 0.121;
          shift(1) = -0.005;
        }
      MappingQEulerian<dim> mapping(2, dofh_eulerian, shift);

      {
        const QGauss<1>                                  quad(fe_degree + 1);
        typename MatrixFree<dim, double>::AdditionalData data;
        data.tasks_parallel_scheme =
          MatrixFree<dim, double>::AdditionalData::none;
        data.mapping_update_flags = update_gradients | update_hessians;
        if (i == 1)
          data.initialize_indices = false;
        mf_data.reinit(mapping, dof, constraints, quad, data);
      }

      MatrixFreeTest<dim, fe_degree, fe_degree + 1> mf(mf_data, mapping);
      mf.test_functions(solution);
    }
}
