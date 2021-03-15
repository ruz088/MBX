#!/usr/bin/env python
# coding: utf-8

# In[ ]:


#Libraries
import sys,os
import copy
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from matplotlib.ticker import MultipleLocator
from matplotlib import rcParams
import pandas as pd

def get_dic(clist):
  a = {}
  count = 1
  for i in sorted(clist):
    try:
      b=a[i]
    except:
      a[i] = count
      count += 1

  b = []
  for i in clist:
    b.append(a[i])

  return np.array(b)

# To remove axis margin
from mpl_toolkits.mplot3d.axis3d import Axis
if not hasattr(Axis, "_get_coord_info_old"):
  def _get_coord_info_new(self, renderer):
    mins, maxs, centers, deltas, tc, highs = self._get_coord_info_old(renderer)
    mins += deltas / 4
    maxs -= deltas / 4
    return mins, maxs, centers, deltas, tc, highs
  Axis._get_coord_info_old = Axis._get_coord_info  
  Axis._get_coord_info = _get_coord_info_new

# In[ ]:
if len(sys.argv) < 2:
  print("Usage: " + sys.argv[0] + "<csv_file>") 
  sys.exit()

folders = ["256_h2o_cubic_box"]
#folders = ["2048_h2o_droplet","256_h2o_cubic_box","2048_h2o_cubic_box","57_ch4_513_h2o_rectangular","342_ch4_228_h2o_rectangular","clathrate"]

legend_contrib = ["One-body Polynomials","Two-body Polynomials","Three-body Polynomials","Electrostatics","Dispersion","Buckingham","Force field","Lennard-Jones"]

colors = ['greenyellow','lime','darkgreen','brown','orange','royalblue','deeppink','darkviolet']

#Input files
dataFile = sys.argv[1]

#Lists
tf = pd.read_csv(dataFile,skipinitialspace=True);

for f in folders:
  tff = tf[tf['system'] == f] 
  e1b = (tff['E1B'])/tff['num_steps']
  e2b = (tff['E2B_LOCAL'] + tff['E2B_GHOST'])/tff['num_steps']
  e3b = (tff['E3B_LOCAL'] + tff['E3B_GHOST'])/tff['num_steps']
  elec = (tff['ELE'])/tff['num_steps']
  disp = (tff['DISP'] + tff['DISP_PME'])/tff['num_steps']
  buck = (tff['BUCK'])/tff['num_steps']
  ff = (tff['FF'])/tff['num_steps']
  lj = (tff['LJ'] + tff['LJ_PME'])/tff['num_steps']
  tot = (tff['loop_time'])/tff['num_steps']
 
  contrib = [e1b,e2b,e3b,elec,disp,buck,ff,lj]

  nomp = tff['num_threads']
  nmpi = tff['num_ranks']

  nomppos = get_dic(nomp)
  nmpipos = get_dic(nmpi)

  max_omp = max(nomppos)
  max_mpi = max(nmpipos)

  # Two plots per folder
  # Plot # 1: Absolute times, stacked bar plot 
  # Plot # 2: Scaling of all the contributions

  fig = plt.figure(figsize=(6,6))
  axs = fig.add_subplot(projection='3d')

  rcParams['font.family'] = 'Helvetica'
  
  width = 0.2
  nomppos = nomppos - width/2.0
  nmpipos = nmpipos - width/2.0

  
  top = e1b - e1b

  leg = []
  for i in range(len(contrib)):
    bottom = top 
    top = bottom + contrib[i] 
    axs.bar3d(nomppos,nmpipos,bottom,width,width,contrib[i],shade=False,color = colors[i], edgecolor='black')
    # for legend
    leg.append(plt.Rectangle((0, 0), 1, 1, fc=colors[i]))
    
  axs.legend(leg,legend_contrib, fontsize=8, loc='upper left')
  axs.set_xlim(max_omp+0.5,0.5)
  axs.set_ylim(max_mpi+0.5,0.5)

  axs.set_xlabel("Number of OpenMP threads",fontsize=16)
  axs.set_ylabel("Number of MPI ranks",fontsize=16)
  axs.set_zlabel("Time (s)",fontsize=16)

  plt.xticks(nomppos+width/2.0,nomp)
  plt.yticks(nmpipos+width/2.0,nmpi)


  #Tight layout
  #plt.tight_layout()
  
  #Save images
  name = 'individual_times_' + f
  
  figName = name +'.png'
  plt.savefig(figName, format='png', dpi=1000)
  figName = name +'.pdf'
  plt.savefig(figName, format='pdf', dpi=1000)
  
  os.system("cp " + name +".png $HOME")


  # Now plot #2. 
  fig, axs = plt.subplots(1, 1, figsize=(7,6))
  plt.grid(color='gray',linestyle='--')
  axs.set_axisbelow(True)
  cmap = plt.get_cmap('viridis')
  axs.scatter(nomppos,nmpipos,marker='o',color='gray',s=(nomp*nmpi)*200)
  sp = axs.scatter(nomppos,nmpipos,marker='o',c=(max(tot)/tot) / (nomp*nmpi),s=(max(tot)/tot)*200) 
  plt.colorbar(sp,fraction=0.046, pad=0.04)
  axs.set_xticks(nomppos)
  axs.set_xticklabels(nomp)
  axs.set_yticks(nmpipos)
  axs.set_yticklabels(nmpi)

  axs.set_xlabel('Number of OpenMP threads', fontsize=16)
  axs.set_ylabel('Number of MPI ranks', fontsize=16)

  axs.set_xlim([0.5,max(max(nomppos),max(nmpipos)) + 0.5])
  axs.set_ylim([0.5,max(max(nomppos),max(nmpipos)) + 0.5])


  #Tight layout
  plt.tight_layout()

  #Save images
  name = 'total_scaling_' + f

  figName = name +'.png'
  plt.savefig(figName, format='png', dpi=1000)
  figName = name +'.pdf'
  plt.savefig(figName, format='pdf', dpi=1000)

  os.system("cp " + name +".png $HOME")

  
  

#### Figure
#xi = np.linspace(-100, 100 ,1000, endpoint = True)
#yi = xi
#
#for i in range(len(folders)):
#
#  #Creationg of figure
#  fig, axs = plt.subplots(1, 1, figsize=(6,6))
#  rcParams['font.family'] = 'Helvetica'
#
#  for j in range(len(reordered_times[i]) -1,-1,-1):
#    axs.bar([k + 1 for k in range(len(nprocs))], reordered_times[i][j],0.40,tick_label = nprocs, color = colors[j])
#
#  a = [mm for mm in reversed(legend_contrib)]
#  axs.legend(a, fontsize=12, loc=0, frameon=False)
#
#  axs.tick_params(top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=6)
#  axs.tick_params(which='minor', top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=4)
#  #
#  axs.set_xlabel('Number of Cores', fontsize=16)
#  axs.set_ylabel('Time (ms)', fontsize=16)
#
#
#  #Tight layout
#  plt.tight_layout()
#  
#  #Save images
#  name = 'individual_times_' + folders[i]
#  
#  figName = name +'.png'
#  plt.savefig(figName, format='png', dpi=1000)
#  figName = name +'.pdf'
#  plt.savefig(figName, format='pdf', dpi=1000)
#  
#  os.system("cp " + name +".png $HOME")
#  
#
## Now going for the scaling plot
#
#for i in range(len(folders)):
#
#  #Creationg of figure
#  fig, axs = plt.subplots(1, 1, figsize=(6,6))
#  rcParams['font.family'] = 'Helvetica'
#
#  for j in range(len(scalings[i])):
#    axs.plot(nprocs, scalings[i][j],marker='o', color = colors[j],linestyle='-')
#  
#  axs.plot(nprocs,tot_scaling[i],marker='s',color='black',linestyle='-')
#  
#  a = copy.deepcopy(legend_contrib)
#  a.append("Total")
#  axs.legend(a, fontsize=12, loc=0, frameon=False)
#
#  axs.tick_params(top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=6)
#  axs.tick_params(which='minor', top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=4)
#  axs.set_xlabel('Number of Cores', fontsize=16)
#  axs.set_ylabel('Time (ms)', fontsize=16)
#
#  axs.set_xlim([0.5,max(nprocs) + 0.5])
#  axs.set_ylim([0.5,max(nprocs) + 0.5])
#
#  axs.plot(xi,yi, '--',color ='grey')
#
#  #Tight layout
#  plt.tight_layout()
#
#  #Save images
#  name = 'scaling_' + folders[i]
#
#  figName = name +'.png'
#  plt.savefig(figName, format='png', dpi=1000)
#  figName = name +'.pdf'
#  plt.savefig(figName, format='pdf', dpi=1000)
#
#  os.system("cp " + name +".png $HOME")



##Left
#axs.scatter(reference[0], calc[0], marker='s',color='#FF8000', facecolors='none')
#axs.scatter(reference[1], calc[1], marker='^',color='#EE220C', facecolors='none')
#axs.set_xlim([-5,50])
#axs.set_ylim([-5,50])
#axs.xaxis.set_minor_locator(MultipleLocator(5))
#axs.yaxis.set_minor_locator(MultipleLocator(5))
#axs.xaxis.set_major_locator(MultipleLocator(10))
#axs.yaxis.set_major_locator(MultipleLocator(10))
#axs.tick_params(top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=6)
#axs.tick_params(which='minor', top='on', bottom='on', left='on', right='on', direction='in', labelsize=15, length=4)
#
#axs.set_xlabel('CCSD(T)/AV(DT)Z', fontsize=16)
#axs.set_ylabel('MB-nrg (kcal/mol)', fontsize=16)
#axs.legend(["MB-nrg CM5 " + percentage + ", BE < " + str(int(e_split)) + " kcal/mol", "MB-nrg CM5 " + percentage + ", BE > " + str(int(e_split)) + " kcal/mol"], fontsize=12, loc=2, frameon=False)
##axs.text(-2, 53, r'a) CH$_4$: TTM-nrg', fontsize=15)
#axs.plot(xi,yi, '--',color ='k')

##Tight layout
#plt.tight_layout()
#
##Save images
#name = 'individual_times'
#
#figName = name +'.png'
#plt.savefig(figName, format='png', dpi=1000)
#figName = name +'.pdf'
#plt.savefig(figName, format='pdf', dpi=1000)
#
#os.system("cp " + name +".png $HOME")


