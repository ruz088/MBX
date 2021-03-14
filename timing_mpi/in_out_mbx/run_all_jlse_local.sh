#!/bin/sh
#COBALT -n 1 -t 180 -q skylake_8180

#module load intelmpi/2019-intel gcc
#module load tbb

WRK=$PWD
#FRC=/home/knightc/projects/mb-pol/tests/bin/compare_lammpstrj/src/compare_force

LMP_DIR=$HOME/codes/lammps/src
LMP_EXE=lmp_mpi_mbx
#LMP_AVX2=lmp_jlse_thing
#LMP_AVX512=lmp_jlse_skylake

#MBX_DIR=/home/knightc/projects/mb-pol/tests/perftests/single_point-json-timings
#MBX_AVX2=single_point-avx2
#MBX_AVX512=single_point-avx512

#BRH=master-dev
#BRH=lammps-perf

#export FI_PROVIDER=sockets
#export FI_SOCKETS_IFACE=eth0
#export FI_PROVIDER_PATH=/soft/compilers/intel-2019/impi/2019.4.243/intel64/libfabric/lib/prov

export OMP_WAIT_POLICY=ACTIVE
#export KMP_AFFINITY="granularity=fine,compact,1,0" 

LMP_ARG="-in lmp.in -v NSTEPS 10"
NUM_RANKS_LMPS=(1 1 1 1 2 2 2 4 4 8)
NUM_THREADS_LMPS=(1 2 4 8 1 2 4 1 2 1)

#MBX_ARG="lmp.nrg mbx.json 10"
#NUM_RANKS_MBX=(1 1 1 1 1 1 1)
#NUM_THREADS_MBX=(1 2 4 8 14 28 56)

cd $WRK

#SYSTEMS=`ls -d */`
SYSTEMS="256_h2o_cubic_box"

for test in ${SYSTEMS}
do
    echo "Running test... ${test}"
    cd $test

# -- Run LAMMPS

    for (( i=0; i<${#NUM_RANKS_LMPS[@]}; i++));
    do

      NRANKS=${NUM_RANKS_LMPS[$i]}
      NTHREADS=${NUM_THREADS_LMPS[$i]}

      echo "LMPS: NRANKS= ${NRANKS}  NTHREADS= ${NTHREADS}"
      env OMP_NUM_THREADS=${NTHREADS} mpirun -np ${NRANKS} ${LMP_DIR}/${LMP_EXE} ${LMP_ARG} -log logs/log.lammps.r${NRANKS}.o${NTHREADS}
#      env OMP_NUM_THREADS=${NTHREADS} mpirun -np ${NRANKS} ${LMP_DIR}/${LMP_AVX512}-${BRH} ${LMP_ARG} -log logs/log.lammps.r${NRANKS}.o${NTHREADS}-${LMP_AVX512}-${BRH}

    done
    rm -f lmp.lammpstrj log.cite

# -- Run MBX
    for (( i=0; i<${#NUM_THREADS_MBX[@]}; i++));
    do

      NRANKS=1
      NTHREADS=${NUM_THREADS_MBX[$i]}

      echo "MBX: NRANKS= ${NRANKS}  NTHREADS= ${NTHREADS}"
#      env OMP_NUM_THREADS=${NTHREADS} ${MBX_DIR}/${MBX_AVX2}-${BRH}   ${MBX_ARG} > logs/mbx.r${NRANKS}.o${NTHREADS}-${MBX_AVX2}-${BRH} 2>&1
#      env OMP_NUM_THREADS=${NTHREADS} ${MBX_DIR}/${MBX_AVX512}-${BRH} ${MBX_ARG} > logs/mbx.r${NRANKS}.o${NTHREADS}-${MBX_AVX512}-${BRH} 2>&1

    done

    cd ../
done


