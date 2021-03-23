#include <cmath>
#include <cassert>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <numeric>

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "io_tools/read_nrg.h"
#include "io_tools/write_nrg.h"

#include "bblock/system.h"

//#define NUMGRADS
//#define PRINT_GRADS
//#define PRINT_VIRIAL
namespace {

static std::vector<bblock::System> systems;
static std::vector<bblock::System> MM_system;
static std::vector<bblock::System> QM_system;
}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.nrg> [mbx.json]" << std::endl;
        return 0;
    }

    try {
        std::ifstream ifs(argv[1]);

        if (!ifs) {
            throw std::runtime_error("could not open the NRG file");
        }

        tools::ReadNrg(argv[1], systems);
        ifs.close();

    } catch (const std::exception& e) {
        std::cerr << " ** Error ** : " << e.what() << std::endl;
        return 1;
    }

    std::vector<double> box;

    if (argc > 2) {
        systems[0].SetUpFromJson(argv[2]);
    } else {
        systems[0].SetUpFromJson();
    }


   // double qmmm_en = systems[0].QMMM_setup(true); 

    std::vector<int> qm_ind = systems[0].get_qm_indeces();

    size_t tnm=systems[0].GetNumMon();

    std::vector<int> mm_ind;

    std::vector<int> total_ind(tnm);

    int tot_num_mon = 0;

    for (size_t pp=0; pp<tnm; pp++) {
         tot_num_mon +=1;
    }

    std::iota(std::begin(total_ind), std::end(total_ind), 1);

    for (int pp=1; pp<=tot_num_mon; pp++) {
       if (std::find(qm_ind.begin(), qm_ind.end(),pp) != qm_ind.end()) {
          //std::cout<< "QM " << pp<< std::endl;
          ;
       } else {
           //std::cout<< "MM " << pp<< std::endl;
           mm_ind.push_back(pp);
       }

    }
    std::cout<< "mm system size " << mm_ind.size() <<" "<< mm_ind[0] << std::endl;

    tools::ReadNrg(argv[1], MM_system,&qm_ind); // MM system
    tools::ReadNrg(argv[1], QM_system,&mm_ind); // QM system 


    //std::cout << "potato" << std::endl;
    std::vector<double> MM_xyz = MM_system[0].GetRealXyz();
    std::vector<double> QM_xyz = QM_system[0].GetRealXyz();
    std::vector<double> MM_virt_xyz = MM_system[0].GetXyz();

    std::vector<double> MM_charges = MM_system[0].GetRealCharges();
    std::vector<double> QM_charges = QM_system[0].GetRealCharges();
    
    std::vector<double> MM_virt_charges = MM_system[0].GetCharges();
    std::vector<double> Total_virt_charges = systems[0].GetCharges();

    std::vector<double> Total_real_charges = systems[0].GetCharges();


    std::vector<std::string> MM_atomnames = MM_system[0].GetRealAtomNames();
    std::vector<std::string> QM_atomnames = QM_system[0].GetRealAtomNames();

    std::vector<std::string> MM_virt_atomnames = MM_system[0].GetAtomNames();


    double QM_total_charge = std::accumulate(QM_charges.begin(), QM_charges.end(),
                               decltype(QM_charges)::value_type(0));

    //std::vector<std::vector<double>> qm_mod_charges=systems[0].get_qm_modcharges();

    for (size_t i=0; i < Total_real_charges.size(); i++) { 
        std::cout << " real charges before mod " << Total_real_charges[i] << std::endl;
    }
    
    //std::cout << " Electrostatics before mod " << systems[0].Electrostatics(true) << std::endl;


    //std::cout << "QM MOD CHARGES " << std::endl;
    //std::cout << qm_mod_charges[0][0] << std::endl;

    //systems[0].set_qm_charge(qm_ind,qm_mod_charges);
   
    //std::vector<double> Total_virt_charges2 = systems[0].GetCharges();

    //for (size_t i=0; i < Total_virt_charges.size(); i++) {
    //    std::cout << " system charges after mod " << Total_virt_charges2[i] << std::endl;
    // }
    //std::cout << " Regular Electrostatics = " << systems[0].Electrostatics(true) << std::endl;
    //std::cout << " Electrostatics after mod = " << systems[0].CustomElectrostatics(qm_ind,qm_mod_charges,true) << std::endl;

    //std::cout << "turnip" << std::endl;


    //std::cout << "MM SYSTEM " << std::endl;
    //for (size_t pp=0; pp < MM_xyz.size(); pp++) {
    //    std::cout << MM_xyz[pp] << std::endl;
    //}
    //std::cout << "QM SYSTEM " << std::endl;
    //for (size_t pp=0; pp < QM_xyz.size(); pp++) {
    //    std::cout << QM_xyz[pp] << std::endl;
    //}
    //std::cout << "TURNIP " << std::endl;
    //
    //
    //
    //GET MM Non electrostatic energy
    //
    double E1b_mm, E2b_qmmm, E3b_qmmm, Edisp_qmmm, Ebuck_qmmm, Etotal, EMM_nonelec, Eelec_qmmm;
    E1b_mm = MM_system[0].OneBodyEnergy(true);
    E2b_qmmm = systems[0].TwoBodyEnergy(true)- QM_system[0].TwoBodyEnergy(true);
    E3b_qmmm = systems[0].ThreeBodyEnergy(true)- QM_system[0].ThreeBodyEnergy(true);
    Edisp_qmmm = systems[0].Dispersion(true)- QM_system[0].Dispersion(true);
    Ebuck_qmmm = systems[0].Buckingham(true)- QM_system[0].Buckingham(true);

    EMM_nonelec = E1b_mm + E2b_qmmm + E3b_qmmm + Edisp_qmmm + Ebuck_qmmm;

    std::vector<std::string> qm_auxparams = systems[0].get_qm_auxparams();
    if (systems[0].get_qm_code() == "qchem") {

            
        std::ofstream qin("qmmm_input.qin");

        qin << "$molecule\n";
        qin << systems[0].get_qm_charge() << " " << systems[0].get_qm_spin() << "\n";

        for (size_t pp=0; pp<QM_xyz.size()/3; pp++) {
             qin << QM_atomnames[pp] << " " << QM_xyz[3*pp] << " " << QM_xyz[3*pp+1] << " " << QM_xyz[3*pp+2] << "\n";
	}
        qin << "$end\n";


        qin << "\n";


        qin << "$rem\n";
        qin << "jobtype sp\n";
        qin << "method " << systems[0].get_qm_theory() << "\n";
        qin << "basis " << systems[0].get_qm_basis() << "\n";
        qin << "MBX true\n";
        qin << "MBX_molecule " << mm_ind.size() << "\n";
       
   

        for (size_t pp=0; pp<qm_auxparams.size(); pp++) {
             qin << qm_auxparams[pp] << "\n";
        }
        
        qin << "$end\n";
 
        qin << "\n"; 
      
        qin << "$MBX\n";
        for (size_t pp=0; pp<MM_virt_xyz.size()/3; pp++) {
             qin << MM_virt_atomnames[pp] << " " << MM_virt_xyz[3*pp] << " " << MM_virt_xyz[3*pp+1] << " " << MM_virt_xyz[3*pp+2] << " " << MM_virt_charges[pp] <<"\n";
        }
        qin << "$end\n";

        qin.close();

    } else if (systems[0].get_qm_code() == "psi4") {

        std::ofstream psi4in("qmmm_job.py");
        

        psi4in << "import psi4\n";

        
        if (systems[0].get_qm_mem()!="") { 

            psi4in << "psi4.set_memory(\'" << systems[0].get_qm_mem() << "\')\n";
        }

        psi4in << "psi4.set_num_threads(" << systems[0].get_qm_threads() << ")\n";

        psi4in << "geo = \"\"\"\n";
        
        psi4in << systems[0].get_qm_charge() << " " << systems[0].get_qm_spin() << "\n";   

        for (size_t pp=0; pp<QM_xyz.size()/3; pp++) {
             psi4in << QM_atomnames[pp] << " " << QM_xyz[3*pp] << " " << QM_xyz[3*pp+1] << " " << QM_xyz[3*pp+2] << "\n";
        }
        psi4in << "symmetry c1\n";
        psi4in << "no_reorient\n";
        psi4in << "no_com\n";
        psi4in << "\"\"\"\n";

        psi4in << "molsys = psi4.geometry(geo)\n";

        psi4in << "qfield=psi4.QMMM()\n";
        for (size_t pp=0; pp<MM_virt_xyz.size()/3; pp++) {
             psi4in <<  "qfield.addChargeAngstrom( " << MM_virt_charges[pp] <<","<< MM_virt_xyz[3*pp] << "," << MM_virt_xyz[3*pp+1] << "," << MM_virt_xyz[3*pp+2] << ")\n";
        }
        psi4in << "qfield.populateExtern()\n";
        psi4in << "psi4.core.set_global_option_python('EXTERN', qfield.extern)\n";
       
 

        psi4in << "method = \"" << systems[0].get_qm_theory() << "/" << systems[0].get_qm_basis() << "\"\n";

        for (size_t pp=0; pp<qm_auxparams.size(); pp++) {
             psi4in << qm_auxparams[pp] << "\n";
        }


        psi4in << "energy = psi4.energy(method, molecule=molsys)\n";


        psi4in << "print('Energy(au) = {:12.6f}'.format(energy))";


        psi4in.close();

        system("python qmmm_job.py > qm.out");

        //std::cout << systems[0].get_qm_code() << " is not implemented yet. exiting. " << std::endl;
        //std::exit(0);
        //std::cout << " POTATO YOU SHOULD NOT BE SEEING THIS MESSAGE. SOMETHING IS WRONG. " << std::endl;

    } else if (systems[0].get_qm_code() == "chronus") {

         std::cout << systems[0].get_qm_code() << " is not implemented yet. exiting. " << std::endl;
         std::exit(0);
         std::cout << " POTATO YOU SHOULD NOT BE SEEING THIS MESSAGE. SOMETHING IS WRONG. " << std::endl;
    } else if (systems[0].get_qm_code() == "gaussian") {

        std::ofstream gin("qmmm_job.com");


        //gin << "%chk=qmmm.chk\n";
        gin << "%Mem=" << systems[0].get_qm_mem() << "\n";
        gin << "%NprocShared=" << systems[0].get_qm_threads() << "\n";
        gin << "#P " << systems[0].get_qm_theory() << "/" << systems[0].get_qm_basis()<< " SP Symmetry=None\n";
        if (qm_auxparams.size() > 0 ) {
            gin << "Int=UltraFine " << qm_auxparams[0] << " SCF=(YQC,Big,Direct)\n";
        } else {
            gin << "Int=UltraFine SCF=(YQC,Big,Direct)\n";
        }
 
        gin << "Charge=angstroms Population=(MK,ReadRadii)\n";
        gin << "\n";
        gin << "QMMM\n";
        gin << "\n";
        gin << systems[0].get_qm_charge() << " " << systems[0].get_qm_spin() << "\n";
        for (size_t pp=0; pp<QM_xyz.size()/3; pp++) {
             gin << QM_atomnames[pp] << " " << QM_xyz[3*pp] << " " << QM_xyz[3*pp+1] << " " << QM_xyz[3*pp+2] << "\n";
        }
        gin << "\n";
        for (size_t pp=0; pp<MM_virt_xyz.size()/3; pp++) {
             if (MM_virt_charges[pp] !=0 ) {
                 gin <<  " " << MM_virt_xyz[3*pp] << " " << MM_virt_xyz[3*pp+1] << " " << MM_virt_xyz[3*pp+2] << " " << MM_virt_charges[pp] << "\n";
             }
        }
        gin << "\n";

        gin.close();
        //std::exit(0);

        system("g09 < qmmm_job.com > qmmm_job.out");
        //std::exit(0);
        std::fstream outfile;       
        outfile.open("qmmm_job.out",std::ios::in); 
        double eself; 
        double eqm;
	int qdet =0;
        std::vector<double> qmcharges; 
        if (outfile.is_open()){   //checking whether the file is open
            std::string qline;
            while(getline(outfile, qline)){ //read data from file object and put it into string.
                std::istringstream iss(qline);
                std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                      std::istream_iterator<std::string>{}};
                       //std::cout << "t test 2 " << std::endl;
                       //std::cout << tokens[0] << std::endl;
                if (tokens.size() > 0) {
                    if (tokens[0]=="Self" and tokens[1]=="energy" and tokens[2]=="of" and tokens[3]=="the" and tokens[4]=="charges" and tokens[5]=="=") {
                        eself=std::stod(tokens[6]);
                    }
                    if (tokens[0]=="SCF" and tokens[1]=="Done:" and tokens[3]=="=") {
                        eqm=std::stod(tokens[4]);
                    }
                    if (tokens[0]=="Sum" and tokens[1]=="of" and tokens[2]=="ESP" and tokens[3]=="charges") {
                        qdet=0;
                    }
                    if (qdet==2) {
                       qmcharges.push_back(std::stod(tokens[2]));
                    }
                    if (qdet==1) {
                        qdet=2;
                    }
                    //std::cout << "t test 4 " << std::endl;
                    if (tokens[0]=="ESP" and tokens[1]=="charges:") {
                        qdet=1;
                    }
                      
                }	           
            }
            outfile.close(); //close the file object.  
        }
            
        std::vector<std::vector<double>> tq_charges;
         int counter=0;
         //std::cout << "t test 3 " << std::endl;
         for (size_t i=0; i <qm_ind.size(); i++) {
            // std::cout << "qm indes " << qm_ind[i]-1 << std::endl; 
             size_t nats=systems[0].GetMonNumAt(qm_ind[i]-1);
             std::vector<double> mq_charges;
            // std::cout << "t test 4 " << std::endl;
             for (size_t j=0; j<nats; j++ ) {
                  mq_charges.push_back(qmcharges[counter]);
                  std::cout << "t charge " << i << " " << qmcharges[counter] << std::endl;
                  counter +=1;
              }
             tq_charges.push_back(mq_charges);
             mq_charges.clear();
        }

        double Ebackpol=systems[0].CustomInducedElectrostatics(qm_ind,tq_charges,true);
        std::cout << "ebackpol = " << Ebackpol << std::endl;
        double Eelec_mmperm = MM_system[0].CustomPermanentElectrostatics(qm_ind,tq_charges,true);
        Eelec_qmmm=Ebackpol + (eqm-eself)*627.509+Eelec_mmperm;
        tq_charges.clear();

        

       



    } else {

         std::cout << systems[0].get_qm_code() << " is not implemented yet. exiting. " << std::endl;
         std::exit(0);
         std::cout << " POTATO YOU SHOULD NOT BE SEEING THIS MESSAGE. SOMETHING IS WRONG. " << std::endl;
    }

    std::cout << " Nonelec QM/MM Components : " << EMM_nonelec << std::endl; 
    std::cout << " (MM nonelec + QM/MM nonelec cross terms)" << std::endl;
    std::cout << " Total QM/MM Energy : " << EMM_nonelec + Eelec_qmmm << " kcal/mol ( " << (EMM_nonelec + Eelec_qmmm)/627.509 << " a.u )"<< std::endl;
} 
