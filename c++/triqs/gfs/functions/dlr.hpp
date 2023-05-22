// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Hugo U.R. Strand

#pragma once
#include "triqs/gfs/gf/gf.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/dlr_imfreq.hpp"
#include "../..//mesh/dlr_coeffs.hpp"
namespace triqs::gfs {

  using mesh::dlr_coeffs;
  using mesh::dlr_imfreq;
  using mesh::dlr_imtime;

  //-------------------------------------------------------
  // Transformation of DLR Green's functions
  // ------------------------------------------------------

  auto make_gf_dlr_coeffs(MemoryGf<dlr_imtime> auto const &g) {
    auto result   = gf{dlr_coeffs{g.mesh()}, g.target_shape()};
    result.data() = result.mesh().dlr_it().vals2coefs(g.data());
    return result;
  }

  auto make_gf_dlr_coeffs(MemoryGf<dlr_imfreq> auto const &g) {
    auto result   = gf{dlr_coeffs{g.mesh()}, g.target_shape()};
    auto beta_inv = 1. / result.mesh().beta();
    result.data() = beta_inv * result.mesh().dlr_if().vals2coefs(g.data());
    return result;
  }

  auto make_gf_dlr_imtime(MemoryGf<dlr_coeffs> auto const &g) {
    auto result   = gf{dlr_imtime{g.mesh()}, g.target_shape()};
    result.data() = g.mesh().dlr_it().coefs2vals(g.data());
    return result;
  }

  auto make_gf_dlr_imfreq(MemoryGf<dlr_coeffs> auto const &g) {
    auto result   = gf{dlr_imfreq{g.mesh()}, g.target_shape()};
    auto beta     = result.mesh().beta();
    result.data() = beta * g.mesh().dlr_if().coefs2vals(g.data());
    return result;
  }

  auto make_gf_imtime(MemoryGf<dlr_coeffs> auto const &g, long n_tau) {
    auto result = gf{mesh::imtime{g.mesh().beta(), g.mesh().statistic(), n_tau}, g.target_shape()};
    for (auto const &tau : result.mesh()) result[tau] = g(tau.value());
    return result;
  }

  auto make_gf_imfreq(MemoryGf<dlr_coeffs> auto const &g, long n_iw = 1025) {
    auto result = gf{mesh::imfreq{g.mesh().beta(), g.mesh().statistic(), n_iw}, g.target_shape()};
    for (auto const &w : result.mesh()) result[w] = g(w.value());
    return result;
  }

  // OPFIXME decide strategy
  // OPFIXME Discussion on going with Jason.
  auto make_gf_dlr_imfreq(MemoryGf<mesh::imfreq> auto const &g, double Lambda, double eps) {
    auto result = gf{dlr_imfreq{g.mesh().beta(), g.mesh().statistic(), Lambda, eps}, g.target_shape()};
    // Check that the g mesh is big enough ...
    //auto [wmin, wmax] = result.mesh().min_max_frequencies();
    //auto n_max        = g.mesh().w_max().n;
    //if ((std::abs(wmin.n) > n_max) or (wmax.n > n_max)) throw std::runtime_error("make_gf_dlr_imfreq: the requested grid is too large");
    for (auto const &w : result.mesh()) result[w] = g(w.value());
    return result;
  }

} // namespace triqs::gfs
