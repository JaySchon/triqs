# Copyright (c) 2016-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
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
# Authors: Thomas Ayral, Olivier Parcollet, Nils Wentzell

from triqs.gf import *
from h5 import *

#one constructor
m=MeshImFreq(beta=1, S='Fermion', n_iw=10)
G=GfImFreq(mesh=m, indices=[['a'],['b1','b2'],['c1', 'c2']])

#another constructor
G2=GfImFreq(beta=1.,statistic="Fermion",n_points=100, target_shape=(1,2,2))

assert G.data.shape==(20,1,2,2),"not ok"
assert G[0,0,0].data.shape==(20,), "not ok"

#H5 r/w
A=HDFArchive("Tv3.h5",'w')
A["G"]=G
del A

A2=HDFArchive("Tv3.h5",'r')
G3=A2["G"]
del A2
assert G3.data.shape==G.data.shape,"not ok:%s vs %s"%(G3.data.shape, G.data.shape)

#mpi bcast
import triqs.utility.mpi as mpi
G4=GfImFreq(beta=1.,statistic="Fermion",n_points=100, target_shape=(1,2,2))
if mpi.is_master_node():
   G4.data[:,:,:,:] = 5
   assert G4.data[0,0,0,0]==5, "not ok :%s"%(G4.data[0,0,0,0])
if not mpi.is_master_node():
   assert G4.data[0,0,0,0]==0, "not ok"
G4 = mpi.bcast(G4)
if not mpi.is_master_node():
   assert G4.data[0,0,0,0]==5, "not ok :%s"%(G4.data[0,0,0,0])


##Tv4
print("#############################")
G5=GfImFreq(mesh=m, target_shape=(1,2,2,1))
print(G5.data.shape)

assert G5.data.shape==(20,1,2,2,1),"not ok"
assert G5[0,0,0,0].data.shape==(20,), "not ok"

#ImTime, 
print("#############################")
G6=GfImTime(beta=1.,statistic="Fermion",n_points=100, target_shape=(1,2,2))
