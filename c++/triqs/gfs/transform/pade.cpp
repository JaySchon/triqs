// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include "../../gfs.hpp"
//#include "pade.hpp"
#include <triqs/arrays.hpp>
#include <triqs/utility/pade_approximants.hpp>

namespace triqs::gfs {

  typedef std::complex<double> dcomplex;

  void pade(gf_view<refreq, scalar_valued> gr, gf_const_view<imfreq, scalar_valued> gw, int n_points, double freq_offset) {

    // make sure the GFs have the same structure
    //assert(gw.shape() == gr.shape());

    if (n_points < 0 || n_points > gw.mesh().last_index() + 1)
      TRIQS_RUNTIME_ERROR << "Pade argument n_points (" << n_points
                          << ") should be positive and not be greater than the positive number of Matsubara frequencies ("
                          << gw.mesh().last_index() + 1 << ")\n";

    gr() = 0.0;

    nda::vector<dcomplex> z_in(n_points); // complex points
    nda::vector<dcomplex> u_in(n_points); // values at these points

    for (int i = 0; i < n_points; ++i) {
      z_in(i) = gw.mesh()(i);
      u_in(i) = gw[i];
    }

    triqs::utility::pade_approximant PA(z_in, u_in);

    for (auto om : gr.mesh()) {
      dcomplex e = om + dcomplex(0.0, 1.0) * freq_offset;
      gr[om]     = PA(e);
    }
  }

} // namespace triqs::gfs
