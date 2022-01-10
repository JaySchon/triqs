// Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
// Authors: tayral

#include <triqs/test_tools/gfs.hpp>

#include <triqs/lattice/cluster_mesh.hpp>

TEST(ClusterMesh, FindCellDims) {

  EXPECT_THROW(find_cell_dims(matrix<double>{{{1. / 4.5, 0., 0.}, {0., 1., 0.}, {0., 0., 1.}}}), triqs::runtime_error);

  auto k_s1     = find_cell_dims(matrix<double>{{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}}});
  auto k_s1_res = utility::mini_vector<int, 3>{1, 1, 1};
  EXPECT_EQ(k_s1, k_s1_res);
  auto k_s2     = find_cell_dims(inverse(matrix<double>{{{1., 2., 0.}, {2., 1., 0.}, {0., 0., 1.}}}));
  auto k_s2_res = utility::mini_vector<int, 3>{3, 1, 1};
  EXPECT_EQ(k_s2, k_s2_res);
  auto k_s3     = find_cell_dims(inverse(matrix<double>{{{1., 2., 0.}, {-2., 2., 0.}, {0., 0., 1.}}}));
  auto k_s3_res = utility::mini_vector<int, 3>{3, 2, 1};
  EXPECT_EQ(k_s3, k_s3_res);
  auto k_s4     = find_cell_dims(inverse(matrix<double>{{{6., 6., 0.}, {-6., 6., 0.}, {0., 0., 1.}}}));
  auto k_s4_res = utility::mini_vector<int, 3>{12, 6, 1};
  EXPECT_EQ(k_s4, k_s4_res);
}

TEST(ClusterMesh, Basic) {
  auto m1 = cluster_mesh(make_unit_matrix<double>(3), 3 * make_unit_matrix<int>(3));
  EXPECT_EQ(m1.size(), 3 * 3 * 3);
}

MAKE_MAIN;
