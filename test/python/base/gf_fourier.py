# Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Michel Ferrero, Nils Wentzell

from triqs.gf import *

# ==== Matrix-valued Green functions

n_iw = 100
n_tau = 6 * n_iw + 1
beta=20.0

t_mesh = MeshImTime(beta=beta, statistic='Fermion', n_tau=n_tau)
w_mesh = MeshImFreq(beta=beta, statistic='Fermion', n_iw=n_iw)

gt = Gf(mesh=t_mesh, target_shape=[1,1])
gw = Gf(mesh=w_mesh, target_shape=[1,1])
gw2 = gw.copy()

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << Fourier(gw)

# Check that we are PH symmetric
assert abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-7

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0,0,0].real) < 1e-7
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

gw2 << Fourier(0.3*gt)
assert abs(gw2.data[0,0,0].real) < 1e-7

# ==== Matrix-real-valued Green functions

gw2 << Fourier(gt.real)
gw3 = make_gf_from_fourier(gt.real)

# Check that the backward transform yields the initial gw
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

# ==== Scalar-valued Green functions

gt = gt[0,0]
gw = gw[0,0]
gw2 = gw2[0,0]

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << Fourier(gw)

# Check that we are PH symmetric
assert abs(gt.data[0] - gt.data[-1]) < 1e-7

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0].real) < 1e-7
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

gw2 << Fourier(0.3*gt)
assert abs(gw2.data[0].real) < 1e-7

# ==== With Tailfit

tail, err = fit_tail(gw, make_zero_tail(gw, 1))

gt2 = gt.copy()
gt2 << Fourier(gw, tail)
gt3 = make_gf_from_fourier(gw, gt.mesh, tail)

gw2 << Fourier(gt2, tail)
gw3 = make_gf_from_fourier(gt3, w_mesh, tail)

assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

gw3 = make_gf_from_fourier(gt3.real, w_mesh, tail)
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

# # ==== Block Green functions

blgt = BlockGf(block_list=[gt, 2 * gt])
blgw = BlockGf(block_list=[gw, 2 * gw])
blgw2 = blgw.copy()

blgw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
blgt << Fourier(blgw)
set_from_fourier(blgt, blgw)
blgt << make_gf_from_fourier(blgw)

# Check that we are PH symmetric
assert abs(blgt['0'].data[0] - blgt['0'].data[-1]) < 1e-7

blgw2 << Fourier(blgt)
set_from_fourier(blgw2, blgt)
blgw3 = make_gf_from_fourier(blgt)

# Check that forward and backward transform gives Identity
for bl in ['0', '1']:
    assert abs(blgw2[bl].data[0].real) < 1e-7
    assert max(abs(blgw[bl].data[:] - blgw2[bl].data[:])) < 1e-7
    assert max(abs(blgw[bl].data[:] - blgw3[bl].data[:])) < 1e-7
