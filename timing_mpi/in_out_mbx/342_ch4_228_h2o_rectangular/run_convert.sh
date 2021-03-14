#!/bin/sh

CONVERT=/Users/cjknight/Documents/Catalyst/Support/2019/Paesani/tests/bin/convert_xyz_to_nrg/src/convert_xyz
MBX=/Users/cjknight/Documents/Catalyst/Support/2019/Paesani/tests/single_point-json/single_point

${CONVERT} -in input.xyz -out lmp -nrg -lmps -lattice 18.58263 0.0 0.0  0.0 18.58263 0.0   0.0 0.0 59.46441 -shift 0.5 0.5 0.5

${MBX} input.nrg mbx.json > mbx_input.out 2>&1
${MBX} lmp.nrg mbx.json > mbx_lmp.out 2>&1



