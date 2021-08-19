import ctypes
from ctypes import cdll

mbxlib = cdll.LoadLibrary('../../../install/lib/libmbx.so')

def initialize_system(coordinates,numer_of_atoms_of_monomers,atom_names,monomer_names,json_file):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nats = (ctypes.c_int * len(numer_of_atoms_of_monomers)) (*numer_of_atoms_of_monomers)

  ats_b = [bytes(i,encoding='utf8') for i in atom_names]
  ats = (ctypes.c_char_p * len(ats_b)) (*ats_b)

  mons_b = [bytes(i,encoding='utf8') for i in monomer_names]
  mons = (ctypes.c_char_p * len(mons_b)) (*mons_b)

  nmon = ctypes.c_int(len(mons))

  jsn = (ctypes.c_char_p) (bytes(json_file,encoding='utf8'))

  mbxlib.initialize_system_py_(crd,nats,ats,mons,ctypes.byref(nmon),jsn)

def get_energy_nograd(coordinates,number_of_atoms):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)

  energy = ctypes.c_double(0.0)

  mbxlib.get_energy_(crd,ctypes.byref(nat),ctypes.byref(energy))

  return energy.value

def get_energy_grad(coordinates,number_of_atoms):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)

  energy = ctypes.c_double(0.0)
  gradl = [0.0]*len(coordinates)
  grad = (ctypes.c_double * len(coordinates)) (*gradl)

  mbxlib.get_energy_g_(crd,ctypes.byref(nat),ctypes.byref(energy),grad)

  grad_out = [grad[i] for i in range(len(grad))]

  return energy.value, grad_out

def get_energy_pbc_nograd(coordinates,number_of_atoms,cell_vectors):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)
  box = (ctypes.c_double * len(cell_vectors)) (*cell_vectors)

  energy = ctypes.c_double(0.0)

  mbxlib.get_energy_pbc_(crd,ctypes.byref(nat),box,ctypes.byref(energy))

  return energy.value

def get_energy_pbc_grad(coordinates,number_of_atoms,cell_vectors):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)
  box = (ctypes.c_double * len(cell_vectors)) (*cell_vectors)

  energy = ctypes.c_double(0.0)
  gradl = [0.0]*len(coordinates)
  grad = (ctypes.c_double * len(coordinates)) (*gradl)

  mbxlib.get_energy_pbc_g_(crd,ctypes.byref(nat),box,ctypes.byref(energy),grad)

  grad_out = [grad[i] for i in range(len(grad))]

  return energy.value, grad_out

def set_coordinates(coordinates,number_of_atoms):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)

  mbxlib.set_real_xyz_(crd,ctypes.byref(nat))

def get_potential_and_electric_field_on_points(coordinates, number_of_atoms):
  crd = (ctypes.c_double * len(coordinates)) (*coordinates)
  nat = ctypes.c_int(number_of_atoms)
  
  phil = [0.0]*number_of_atoms
  phi = (ctypes.c_double * number_of_atoms) (*phil)
  
  efl = [0.0]*len(coordinates)
  ef = (ctypes.c_double * len(coordinates)) (*efl)
  
  mbxlib.get_potential_and_electric_field_on_points_(crd,phi,ef,ctypes.byref(nat))

  phi_out = [phi[i] for i in range(len(phi))]
  ef_out = [ef[i] for i in range(len(ef))]
  
  return phi_out,ef_out

def set_box(box):
  length = len(box)
  l = ctypes.c_int(length)
  boxv = (ctypes.c_double * len(box)) (*box)

  mbxlib.set_box_(ctypes.byref(l),boxv)

def finalize_system():
  mbxlib.finalize_system_()

