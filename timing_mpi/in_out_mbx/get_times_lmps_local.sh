#!/bin/sh

#FILE_LIST=( `ls $1*` )
#FILE_LIST=(`ls */logs/log.lammps*`)
FILE_LIST=(`ls 256_h2o_cubic_box/logs/log.lammps*`)
#FILE_LIST=(`ls */logs/log.lammps*master-dev`)
#FILE_LIST=(`ls */logs/log.lammps*lammps-perf`)
num_files=${#FILE_LIST[@]}

OUT=lmps_$1.csv
rm -rf ${OUT} ; touch ${OUT}

echo "num_files= ${num_files}"

lmps_functions=("INIT " "UPDATE_XYZ " "ACCUMULATE_F " "FF " "E1B" "E2B_LOCAL" "E2B_GHOST" "E3B_LOCAL" "E3B_GHOST")
lmps_functions+=("DISP " "DISP_PME" "LJ " "LJ_PME " "BUCK " "ELE ")
lmps_functions+=("INIT_FULL" "UPDATE_XYZ_FULL" "ACCUMULATE_F_FULL")
#lmps_functions+=("INIT_PME" "UPDATE_XYZ_PME" "ACCUMULATE_F_PME")
lmps_functions+=("ELE_PERMDIP_REAL" "ELE_PERMDIP_PME" "ELE_DIPFIELD_REAL" "ELE_DIPFIELD_PME" "ELE_GRAD_REAL" "ELE_GRAD_PME" "ELE_GRAD_FIN" "ELE_COMM_REVFOR")
num_lmps_functions=${#lmps_functions[@]}

# -- Write header
printf "filename, system, num_atoms, num_steps, num_ranks, num_threads, loop_time, " >> ${OUT}
for (( i=0; i<${num_lmps_functions}-1; i++));
do
  printf "${lmps_functions[$i]//[[:blank:]]/}, " >> ${OUT}
done
printf "${lmps_functions[(${num_lmps_functions}-1)]//[[:blank:]]/}" >> ${OUT}
printf "\n" >> ${OUT}

# -- Collect data from log

for (( nf=0; nf<${num_files}; nf++));
do

  FILE=${FILE_LIST[${nf}]}
  echo "Parsing FILE= ${FILE}"

# -- -- Run settings
  lmps_settings[0]=`echo ${FILE} | cut -d'/' -f 1`
  lmps_settings[1]=`grep Loop ${FILE} | awk '{print $12}'`
  lmps_settings[2]=`grep Loop ${FILE} | awk '{print $9}'`

  NAME=`echo ${FILE} | rev | cut -d'/' -f-1 | rev`
  NAME=`echo ${NAME} | sed 's/[\.,n,s,r,t,o,-]/ /g'`
  lmps_settings[3]=`echo ${NAME} | awk '{print $4}'` 
  lmps_settings[4]=`echo ${NAME} | awk '{print $5}'`
  lmps_settings[5]=`grep Loop ${FILE} | awk '{print $4}'`
  num_lmps_settings=${#lmps_settings[@]}

  # -- -- Timings (to be simplifed...)
  for (( i=0; i<${num_lmps_functions}; i++));
  do
#      printf "${lmps_functions[$i]}, " >> ${OUT}
      lmps_timings[$i]=`grep "${lmps_functions[$i]}" ${FILE} | awk '{print $6}'`
#      printf "i= $i  FUNC= |${lmps_functions[$i]}|  TIME= |${lmps_timings[$i]}|\n"
  done
  num_lmps_timings=${#lmps_timings[@]}

# -- Write data to file
  printf "${FILE}, " >> ${OUT}

  for (( i=0; i<${num_lmps_settings}; i++));
  do
    printf "${lmps_settings[$i]}, " >> ${OUT}
  done

  for (( i=0; i<${num_lmps_timings}-1; i++));
  do
    printf "${lmps_timings[$i]}, " >> ${OUT}
  done
  printf "${lmps_timings[${num_lmps_timings}-1]} " >> ${OUT}

  printf "\n" >> ${OUT}
 
done


