/******************************************************************************
Copyright 2019 The Regents of the University of California.
All Rights Reserved.

Permission to copy, modify and distribute any part of this Software for
educational, research and non-profit purposes, without fee, and without
a written agreement is hereby granted, provided that the above copyright
notice, this paragraph and the following three paragraphs appear in all
copies.

Those desiring to incorporate this Software into commercial products or
use for commercial purposes should contact the:
Office of Innovation & Commercialization
University of California, San Diego
9500 Gilman Drive, Mail Code 0910
La Jolla, CA 92093-0910
Ph: (858) 534-5815
FAX: (858) 534-7345
E-MAIL: invent@ucsd.edu

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE, EVEN IF THE UNIVERSITY
OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS. THE UNIVERSITY OF CALIFORNIA MAKES NO
REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED OR
EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE
SOFTWARE WILL NOT INFRINGE ANY PATENT, TRADEMARK OR OTHER RIGHTS.
******************************************************************************/

#include "potential/buckingham/bucktools.h"
#include <string>
#include <vector>

namespace buck {

//----------------------------------------------------------------------------//

double Repulsion(const double a, const double b, const double* p1, const double* xyz2, double* grad1, double* grad2,
                 const size_t nmon1, const size_t nmon2, const size_t start2, const size_t end2,
                 const size_t atom_index1, const size_t atom_index2, bool do_grads, const double cutoff,
                 const std::vector<double>& box, const std::vector<double>& box_inverse, bool use_ghost,
                 const std::vector<size_t>& islocal, const size_t isl1_offset, const size_t isl2_offset,
                 std::vector<double>* virial) {
    size_t nmon22 = nmon2 * 2;

    size_t shift2 = atom_index2 * nmon2 * 3;

    bool use_pbc = box.size();
    double g1[3], g2[3 * nmon2];
    std::fill(g1, g1 + 3, 0.0);
    std::fill(g2, g2 + 3 * nmon2, 0.0);
    const double* boxinv = box_inverse.data();
    const double* boxptr = box.data();
    double repulsion_energy = 0;
    for (size_t nv = start2; nv < end2; nv++) {
        double dx = p1[0] - xyz2[shift2 + nv];
        double dy = p1[1] - xyz2[nmon2 + shift2 + nv];
        double dz = p1[2] - xyz2[nmon22 + shift2 + nv];

        // Apply minimum image convetion
        if (use_pbc) {
            double tmp1 = boxinv[0] * dx + boxinv[3] * dy + boxinv[6] * dz;
            double tmp2 = boxinv[1] * dx + boxinv[4] * dy + boxinv[7] * dz;
            double tmp3 = boxinv[2] * dx + boxinv[5] * dy + boxinv[8] * dz;

            tmp1 -= std::floor(tmp1 + 0.5);
            tmp2 -= std::floor(tmp2 + 0.5);
            tmp3 -= std::floor(tmp3 + 0.5);

            dx = boxptr[0] * tmp1 + boxptr[3] * tmp2 + boxptr[6] * tmp3;
            dy = boxptr[1] * tmp1 + boxptr[4] * tmp2 + boxptr[7] * tmp3;
            dz = boxptr[2] * tmp1 + boxptr[5] * tmp2 + boxptr[8] * tmp3;
        }

        const double rsq = dx * dx + dy * dy + dz * dz;
        const double r = std::sqrt(rsq);

        const double inv_r = 1.0 / r;

        bool include_pair = false;
        size_t isls = islocal[isl1_offset] + islocal[isl2_offset + nv];
        if (!use_ghost) include_pair = true;
        if (use_ghost && isls) include_pair = true;

        // If using cutoff, check for distances and get proper dispersion
        if (r <= cutoff && include_pair) {
            const double br = -b * r;
            const double fac = a * exp(br);

            double ttsw_grad = 0;
            const double ttsw = switch_function(r, cutoff - 1.0, cutoff, ttsw_grad);

            double pair_energy = ttsw * fac;

            if (isls == 1) pair_energy *= 0.5;
            repulsion_energy += pair_energy;

            if (do_grads) {
                // TODO check that this is correct and gradients are properly calculated
                // in switch area
                // Complciated due to the small energy/grad in this area.
                const double grad = ttsw * (b * inv_r * fac) - ttsw_grad * fac / r;

                g1[0] -= dx * grad;
                g2[nv] += dx * grad;

                g1[1] -= dy * grad;
                g2[nmon2 + nv] += dy * grad;

                g1[2] -= dz * grad;
                g2[nmon22 + nv] += dz * grad;

                if (virial != 0) {
                    const double vscale = (isls == 1) ? 0.5 : 1.0;

                    (*virial)[0] += dx * dx * grad * vscale;
                    (*virial)[1] += dx * dy * grad * vscale;
                    (*virial)[2] += dx * dz * grad * vscale;
                    (*virial)[4] += dy * dy * grad * vscale;
                    (*virial)[5] += dy * dz * grad * vscale;
                    (*virial)[8] += dz * dz * grad * vscale;

                    (*virial)[3] = (*virial)[1];
                    (*virial)[6] = (*virial)[2];
                    (*virial)[7] = (*virial)[5];
                }
            }
        }
    }

    if (do_grads) {
        grad1[0] += g1[0];
        grad1[1] += g1[1];
        grad1[2] += g1[2];
        for (size_t i = start2; i < end2; i++) {
            grad2[shift2 + i] += g2[i];
            grad2[shift2 + nmon2 + i] += g2[nmon2 + i];
            grad2[shift2 + nmon22 + i] += g2[nmon22 + i];
        }
    }

    return repulsion_energy;
}

bool GetBuckParams(std::string mon_id1, std::string mon_id2, size_t index1, size_t index2,
                   std::vector<std::pair<std::string, std::string> > buck_pairs, double& out_a, double& out_b,
                   nlohmann::json repdisp_j) {
    // Order the two monomer names and corresponding xyz
    if (mon_id2 < mon_id1) {
        std::string tmp = mon_id1;
        mon_id1 = mon_id2;
        mon_id2 = tmp;
        size_t tmp1 = index1;
        index1 = index2;
        index2 = tmp1;
    }

    if (std::find(buck_pairs.begin(), buck_pairs.end(), std::make_pair(mon_id1, mon_id2)) == buck_pairs.end() &&
        std::find(buck_pairs.begin(), buck_pairs.end(), std::make_pair(mon_id2, mon_id1)) == buck_pairs.end()) {
        out_a = 0.0;
        out_b = 0.0;
        return false;
    }

    std::vector<double> a, b;
    std::vector<size_t> types1, types2;
    size_t nt2, i, j;

    bool done_with_it = false;

    // Check if pair is in json object
    try {
        std::vector<std::vector<std::string> > pairs = repdisp_j["pairs"];
        for (size_t k = 0; k < pairs.size(); k++) {
            if (mon_id1 == pairs[k][0] && mon_id2 == pairs[k][1]) {
                std::vector<std::vector<std::string> > types1 = repdisp_j["types1"];
                std::vector<std::vector<std::string> > types2 = repdisp_j["types2"];
                std::vector<std::vector<std::pair<std::vector<std::string>, double> > > a_v = repdisp_j["a"];
                std::vector<std::vector<std::pair<std::vector<std::string>, double> > > d6_v = repdisp_j["d6"];
                std::string si = types1[k][index1];
                std::string sj = types2[k][index2];

                for (size_t k2 = 0; k2 < a_v[k].size(); k2++) {
                    if ((si == a_v[k][k2].first[0] && sj == a_v[k][k2].first[1]) ||
                        (si == a_v[k][k2].first[1] && sj == a_v[k][k2].first[0])) {
                        out_a = a_v[k][k2].second;
                        done_with_it = true;
                        break;
                    }
                }

                for (size_t k2 = 0; k2 < d6_v[k].size(); k2++) {
                    if ((si == d6_v[k][k2].first[0] && sj == d6_v[k][k2].first[1]) ||
                        (si == d6_v[k][k2].first[1] && sj == d6_v[k][k2].first[0])) {
                        out_b = d6_v[k][k2].second;
                        done_with_it = true;
                        break;
                    }
                }
            }
        }
    } catch (...) {
        out_a = 0.0;
        out_b = 0.0;
    }

    if (done_with_it) return true;

    // Monomers here have to be in alphabetical order: mon1 < mon2 ALWAYS
    if (mon_id1 == "f" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(35920.3);  // kcal/mol F -- O
        a.push_back(800.553);  // kcal/mol F -- H

        b.push_back(3.586190000000000e+00);  // A^(-1)
        b.push_back(2.697680000000000e+00);  // A^(-1)
    } else if (mon_id1 == "cl" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(50180.4);  // kcal/mol * A^(-6) Cl -- O
        a.push_back(2594.28);  // kcal/mol * A^(-6) Cl -- H

        b.push_back(3.275420000000000e+00);  // A^(-1)
        b.push_back(2.782260000000000e+00);  // A^(-1)
    } else if (mon_id1 == "br" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(37682.2);  // kcal/mol * A^(-6) Br -- O
        a.push_back(3804.53);  // kcal/mol * A^(-6) Br -- H

        b.push_back(3.058250000000000e+00);  // A^(-1)
        b.push_back(2.798040000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "i") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(22210.0);  // kcal/mol * A^(-6) I -- O
        a.push_back(6215.10);  // kcal/mol * A^(-6) I -- H

        b.push_back(2.723140000000000e+00);  // A^(-1)
        b.push_back(2.799110000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "li") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(32318.0);  // kcal/mol * A^(-6) Li -- O
        a.push_back(3245.78);  // kcal/mol * A^(-6) Li -- H

        b.push_back(4.023330000000000e+00);  // A^(-1)
        b.push_back(4.006630000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "na") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(47827.7);  // kcal/mol * A^(-6) Na -- O
        a.push_back(4992.61);  // kcal/mol * A^(-6) Na -- H

        b.push_back(3.769530000000000e+00);  // A^(-1)
        b.push_back(3.822550000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "k") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(49986.5);  // kcal/mol * A^(-6) K -- O
        a.push_back(4951.5);   // kcal/mol * A^(-6) K -- H

        b.push_back(3.401250000000000e+00);  // A^(-1)
        b.push_back(3.321390000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "rb") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(48456.3);  // kcal/mol * A^(-6) Rb -- O
        a.push_back(6794.51);  // kcal/mol * A^(-6) Rb -- H

        b.push_back(3.236530000000000e+00);  // A^(-1)
        b.push_back(3.313640000000000e+00);  // A^(-1)
    } else if (mon_id1 == "cs" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(42431.0);  // kcal/mol * A^(-6) Cs -- O
        a.push_back(9403.73);  // kcal/mol * A^(-6) Cs -- H

        b.push_back(3.028640000000000e+00);  // A^(-1)
        b.push_back(3.271530000000000e+00);  // A^(-1)
    } else if (mon_id1 == "co2" && mon_id2 == "co2") {
        // Define the type of atom in each mon
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(9038.48);  // kcal/mol * A^(-6) C -- C
        a.push_back(12608.9);  // kcal/mol * A^(-6) C -- O
        a.push_back(12608.9);  // kcal/mol * A^(-6) O -- C
        a.push_back(24274.7);  // kcal/mol * A^(-6) O -- O

        b.push_back(3.12663);  // A^(-1)
        b.push_back(3.64236);  // A^(-1)
        b.push_back(3.64236);  // A^(-1)
        b.push_back(3.52744);  // A^(-1)

    } else if (mon_id1 == "nh3pbe0d3bj" and mon_id2 == "nh3pbe0d3bj") {
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(19267.3);  // kcal/mol A--A
        a.push_back(3232.98);  // kcal/mol A--B
        a.push_back(3232.98);  // kcal/mol A--B
        a.push_back(980.337);  // kcal/mol B--B
        b.push_back(3.09382);  // A^(-1) A--A
        b.push_back(3.44698);  // A^(-1) A--B
        b.push_back(3.44698);  // A^(-1) A--B
        b.push_back(3.83901);  // A^(-1) B--B

    } else if (mon_id1 == "nh3" and mon_id2 == "nh3") {
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(20413.5);  // kcal/mol A--A
        a.push_back(3259.49);  // kcal/mol A--B
        a.push_back(3259.49);  // kcal/mol A--B
        a.push_back(965.289);  // kcal/mol B--B
        b.push_back(3.11493);  // A^(-1) A--A
        b.push_back(3.4174);   // A^(-1) A--B
        b.push_back(3.4174);   // A^(-1) A--B
        b.push_back(3.78007);  // A^(-1) B--B

    } else if (mon_id1 == "ch4" && mon_id2 == "ch4") {
        // Define the type of atom in each mon
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(42713.9);  // kcal/mol * A^(-6) C -- C
        a.push_back(3258.86);  // kcal/mol * A^(-6) C -- O
        a.push_back(3258.86);  // kcal/mol * A^(-6) O -- C
        a.push_back(2594.4);   // kcal/mol * A^(-6) O -- O

        b.push_back(3.37925);  // A^(-1)
        b.push_back(3.25885);  // A^(-1)
        b.push_back(3.25885);  // A^(-1)
        b.push_back(4.05972);  // A^(-1)
    } else if (mon_id1 == "ch4" && mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(11447.5);  // kcal/mol * A^(-6) C -- O
        a.push_back(4887.62);  // kcal/mol * A^(-6) C -- H
        a.push_back(6182.32);  // kcal/mol * A^(-6) O -- O
        a.push_back(1480.08);  // kcal/mol * A^(-6) O -- H

        b.push_back(2.87176);  // A^(-1)
        b.push_back(3.68542);  // A^(-1)
        b.push_back(3.79757);  // A^(-1)
        b.push_back(4.01558);  // A^(-1)
    } else if (mon_id1 == "co2" && mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(4735.44);  // kcal/mol * A^(-6) C -- O
        a.push_back(4956.27);  // kcal/mol * A^(-6) C -- H
        a.push_back(30678.4);  // kcal/mol * A^(-6) O -- O
        a.push_back(4559.97);  // kcal/mol * A^(-6) O -- H

        b.push_back(2.93819);  // A^(-1)
        b.push_back(3.7359);   // A^(-1)
        b.push_back(3.53045);  // A^(-1)
        b.push_back(3.89503);  // A^(-1)

    } else if (mon_id1 == "n2o5" and mon_id2 == "n2o5") {
        // Define the type of atom in each mon
        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(2);
        types2.push_back(2);
        types2.push_back(2);
        types2.push_back(2);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);
        types1.push_back(2);
        types1.push_back(2);
        types1.push_back(2);
        types1.push_back(2);

        // Set the number of different types
        nt2 = 3;

        // Fill in (in order) the A and b coefficients
        a.push_back(52582.5);  // kcal/mol  A--A
        a.push_back(7849.47);  // kcal/mol  A--B
        a.push_back(34641.1);  // kcal/mol  A--C
        a.push_back(7849.47);  // kcal/mol  B--A
        a.push_back(2692.58);  // kcal/mol  B--B
        a.push_back(49876.0);  // kcal/mol  B--C
        a.push_back(34641.1);  // kcal/mol  C--A
        a.push_back(49876.0);  // kcal/mol  C--B
        a.push_back(31773.9);  // kcal/mol  C--C

        b.push_back(3.86891);  // A^(-1) A--A
        b.push_back(3.19945);  // A^(-1) A--B
        b.push_back(3.60907);  // A^(-1) A--C
        b.push_back(3.19945);  // A^(-1) B--A
        b.push_back(2.33813);  // A^(-1) B--B
        b.push_back(4.19428);  // A^(-1) B--C
        b.push_back(3.60907);  // A^(-1) C--A
        b.push_back(4.19428);  // A^(-1) C--B
        b.push_back(3.56601);  // A^(-1) C--C

    } else if (mon_id1 == "h2o" and mon_id2 == "n2o5") {
        // Define the type of atom in each mon
        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);
        types2.push_back(2);
        types2.push_back(2);
        types2.push_back(2);
        types2.push_back(2);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 3;

        // Fill in (in order) the A and b coefficients
        a.push_back(93032.9);   // kcal/mol  D--A
        a.push_back(12989.0);   // kcal/mol  D--B
        a.push_back(153274.0);  // kcal/mol  D--C
        a.push_back(2460.89);   // kcal/mol  E--A
        a.push_back(514436.0);  // kcal/mol  E--B
        a.push_back(1251.73);   // kcal/mol  E--C

        b.push_back(4.1869);   // A^(-1) D--A
        b.push_back(3.22662);  // A^(-1) D--B
        b.push_back(4.455);    // A^(-1) D--C
        b.push_back(3.24055);  // A^(-1) E--A
        b.push_back(6.4749);   // A^(-1) E--B
        b.push_back(3.03227);  // A^(-1) E--C
        // =====>> BEGIN SECTION BUCKINGHAM <<=====
        // ======>> PASTE CODE BELOW <<======
    } else if (mon_id1 == "h2" and mon_id2 == "h2") {
        types1.push_back(0);
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(822.645);  // kcal/mol A--A
        b.push_back(3.11276);  // A^(-1) A--A
    } else if (mon_id1 == "h2" and mon_id2 == "h2o") {
        types1.push_back(0);
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(2651.63);  // kcal/mol A--C
        a.push_back(839.321);  // kcal/mol B--C
        b.push_back(3.05339);  // A^(-1) A--C
        b.push_back(3.62823);  // A^(-1) B--C
    } else if (mon_id1 == "ar" and mon_id2 == "h2o") {
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(55341.7);  // kcal/mol A--B
        a.push_back(5733.11);  // kcal/mol A--C
        b.push_back(3.48054);  // A^(-1) A--B
        b.push_back(3.46238);  // A^(-1) A--C
    } else if (mon_id1 == "ar" and mon_id2 == "ar") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(102057.0);  // kcal/mol * A^(-6)  A--B
        b.push_back(3.41808);   // A^(-1) A--B
    } else if (mon_id1 == "cs" and mon_id2 == "h2") {
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(18116.1);  // kcal/mol A--B
        b.push_back(3.24781);  // A^(-1) A--B
    } else if (mon_id1 == "na" and mon_id2 == "na") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(33569.6);  // kcal/mol A--A
        b.push_back(4.42822);  // A^(-1) A--A
    } else if (mon_id1 == "cl" and mon_id2 == "cl") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(2943.76);  // kcal/mol A--A
        b.push_back(1.82786);  // A^(-1) A--A
    } else if (mon_id1 == "cl" and mon_id2 == "na") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(33676.8);  // kcal/mol A--B
        b.push_back(2.85113);  // A^(-1) A--B
    } else if (mon_id1 == "ar" and mon_id2 == "cs") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(157654.0);  // kcal/mol A--B
        b.push_back(3.28039);   // A^(-1) A--B
    } else if (mon_id1 == "nma" and mon_id2 == "nma") {
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(2);
        types1.push_back(3);
        types1.push_back(3);
        types1.push_back(3);
        types1.push_back(4);
        types1.push_back(5);
        types1.push_back(6);
        types1.push_back(7);
        types1.push_back(7);
        types1.push_back(7);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(2);
        types2.push_back(3);
        types2.push_back(3);
        types2.push_back(3);
        types2.push_back(4);
        types2.push_back(5);
        types2.push_back(6);
        types2.push_back(7);
        types2.push_back(7);
        types2.push_back(7);

        nt2 = 8;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(556555.0);  // kcal/mol A--A
        a.push_back(7.92193e-08);  // kcal/mol A--B
        a.push_back(29188.4);  // kcal/mol A--C
        a.push_back(-1.78185);  // kcal/mol A--D
        a.push_back(76.8329);  // kcal/mol A--E
        a.push_back(14.2023);  // kcal/mol A--F
        a.push_back(13519.6);  // kcal/mol A--G
        a.push_back(0.404365);  // kcal/mol A--H
        a.push_back(7.92193e-08);  // kcal/mol A--B
        a.push_back(24072.9);  // kcal/mol B--B
        a.push_back(7770.8);  // kcal/mol B--C
        a.push_back(7632.12);  // kcal/mol B--D
        a.push_back(48600.7);  // kcal/mol B--E
        a.push_back(578.297);  // kcal/mol B--F
        a.push_back(57936.7);  // kcal/mol B--G
        a.push_back(4479.02);  // kcal/mol B--H
        a.push_back(29188.4);  // kcal/mol A--C
        a.push_back(7770.8);  // kcal/mol B--C
        a.push_back(97835.2);  // kcal/mol C--C
        a.push_back(-0.0713436);  // kcal/mol C--D
        a.push_back(10862.7);  // kcal/mol C--E
        a.push_back(7637.64);  // kcal/mol C--F
        a.push_back(9910.14);  // kcal/mol C--G
        a.push_back(61043.8);  // kcal/mol C--H
        a.push_back(-1.78185);  // kcal/mol A--D
        a.push_back(7632.12);  // kcal/mol B--D
        a.push_back(-0.0713436);  // kcal/mol C--D
        a.push_back(478.114);  // kcal/mol D--D
        a.push_back(81.4476);  // kcal/mol D--E
        a.push_back(0.213064);  // kcal/mol D--F
        a.push_back(2119.36);  // kcal/mol D--G
        a.push_back(0.262675);  // kcal/mol D--H
        a.push_back(76.8329);  // kcal/mol A--E
        a.push_back(48600.7);  // kcal/mol B--E
        a.push_back(10862.7);  // kcal/mol C--E
        a.push_back(81.4476);  // kcal/mol D--E
        a.push_back(3.14148e-05);  // kcal/mol E--E
        a.push_back(3233.9);  // kcal/mol E--F
        a.push_back(216817.0);  // kcal/mol E--G
        a.push_back(2125.49);  // kcal/mol E--H
        a.push_back(14.2023);  // kcal/mol A--F
        a.push_back(578.297);  // kcal/mol B--F
        a.push_back(7637.64);  // kcal/mol C--F
        a.push_back(0.213064);  // kcal/mol D--F
        a.push_back(3233.9);  // kcal/mol E--F
        a.push_back(1288.03);  // kcal/mol F--F
        a.push_back(24.851);  // kcal/mol F--G
        a.push_back(54.2694);  // kcal/mol F--H
        a.push_back(13519.6);  // kcal/mol A--G
        a.push_back(57936.7);  // kcal/mol B--G
        a.push_back(9910.14);  // kcal/mol C--G
        a.push_back(2119.36);  // kcal/mol D--G
        a.push_back(216817.0);  // kcal/mol E--G
        a.push_back(24.851);  // kcal/mol F--G
        a.push_back(1080390.0);  // kcal/mol G--G
        a.push_back(294.077);  // kcal/mol G--H
        a.push_back(0.404365);  // kcal/mol A--H
        a.push_back(4479.02);  // kcal/mol B--H
        a.push_back(61043.8);  // kcal/mol C--H
        a.push_back(0.262675);  // kcal/mol D--H
        a.push_back(2125.49);  // kcal/mol E--H
        a.push_back(54.2694);  // kcal/mol F--H
        a.push_back(294.077);  // kcal/mol G--H
        a.push_back(1247.46);  // kcal/mol H--H
        b.push_back(5.10251);  // A^(-1) A--A
        b.push_back(0.124088);  // A^(-1) A--B
        b.push_back(3.35382);  // A^(-1) A--C
        b.push_back(0.199081);  // A^(-1) A--D
        b.push_back(2.59766);  // A^(-1) A--E
        b.push_back(1.77292);  // A^(-1) A--F
        b.push_back(2.98567);  // A^(-1) A--G
        b.push_back(0.0895376);  // A^(-1) A--H
        b.push_back(0.124088);  // A^(-1) A--B
        b.push_back(3.44299);  // A^(-1) B--B
        b.push_back(2.58472);  // A^(-1) B--C
        b.push_back(4.35562);  // A^(-1) B--D
        b.push_back(3.82);  // A^(-1) B--E
        b.push_back(2.4897);  // A^(-1) B--F
        b.push_back(3.80478);  // A^(-1) B--G
        b.push_back(3.72512);  // A^(-1) B--H
        b.push_back(3.35382);  // A^(-1) A--C
        b.push_back(2.58472);  // A^(-1) B--C
        b.push_back(3.41495);  // A^(-1) C--C
        b.push_back(0.298141);  // A^(-1) C--D
        b.push_back(2.6994);  // A^(-1) C--E
        b.push_back(3.80065);  // A^(-1) C--F
        b.push_back(2.55108);  // A^(-1) C--G
        b.push_back(5.40438);  // A^(-1) C--H
        b.push_back(0.199081);  // A^(-1) A--D
        b.push_back(4.35562);  // A^(-1) B--D
        b.push_back(0.298141);  // A^(-1) C--D
        b.push_back(2.79273);  // A^(-1) D--D
        b.push_back(1.86683);  // A^(-1) D--E
        b.push_back(1.45553);  // A^(-1) D--F
        b.push_back(2.7634);  // A^(-1) D--G
        b.push_back(1.41784);  // A^(-1) D--H
        b.push_back(2.59766);  // A^(-1) A--E
        b.push_back(3.82);  // A^(-1) B--E
        b.push_back(2.6994);  // A^(-1) C--E
        b.push_back(1.86683);  // A^(-1) D--E
        b.push_back(0.0889704);  // A^(-1) E--E
        b.push_back(3.20248);  // A^(-1) E--F
        b.push_back(4.1739);  // A^(-1) E--G
        b.push_back(3.15108);  // A^(-1) E--H
        b.push_back(1.77292);  // A^(-1) A--F
        b.push_back(2.4897);  // A^(-1) B--F
        b.push_back(3.80065);  // A^(-1) C--F
        b.push_back(1.45553);  // A^(-1) D--F
        b.push_back(3.20248);  // A^(-1) E--F
        b.push_back(4.63545);  // A^(-1) F--F
        b.push_back(1.60478);  // A^(-1) F--G
        b.push_back(1.80232);  // A^(-1) F--H
        b.push_back(2.98567);  // A^(-1) A--G
        b.push_back(3.80478);  // A^(-1) B--G
        b.push_back(2.55108);  // A^(-1) C--G
        b.push_back(2.7634);  // A^(-1) D--G
        b.push_back(4.1739);  // A^(-1) E--G
        b.push_back(1.60478);  // A^(-1) F--G
        b.push_back(4.78365);  // A^(-1) G--G
        b.push_back(2.29189);  // A^(-1) G--H
        b.push_back(0.0895376);  // A^(-1) A--H
        b.push_back(3.72512);  // A^(-1) B--H
        b.push_back(5.40438);  // A^(-1) C--H
        b.push_back(1.41784);  // A^(-1) D--H
        b.push_back(3.15108);  // A^(-1) E--H
        b.push_back(1.80232);  // A^(-1) F--H
        b.push_back(2.29189);  // A^(-1) G--H
        b.push_back(3.64866);  // A^(-1) H--H
    } else if (mon_id1 == "h2o" and mon_id2 == "nma") {
        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(2);
        types2.push_back(3);
        types2.push_back(3);
        types2.push_back(3);
        types2.push_back(4);
        types2.push_back(5);
        types2.push_back(6);
        types2.push_back(7);
        types2.push_back(7);
        types2.push_back(7);

        nt2 = 8;

        // Fill in (in order) the C6 and d6 coefficients
        a.push_back(16817.3);  // kcal/mol A--I
        a.push_back(50566.5);  // kcal/mol B--I
        a.push_back(24946.2);  // kcal/mol C--I
        a.push_back(2949.63);  // kcal/mol D--I
        a.push_back(84432.3);  // kcal/mol E--I
        a.push_back(3555.07);  // kcal/mol F--I
        a.push_back(30748.5);  // kcal/mol G--I
        a.push_back(3042.02);  // kcal/mol H--I
        a.push_back(8519.91);  // kcal/mol A--J
        a.push_back(4740.34);  // kcal/mol B--J
        a.push_back(2526.05);  // kcal/mol C--J
        a.push_back(1068.36);  // kcal/mol D--J
        a.push_back(1754.12);  // kcal/mol E--J
        a.push_back(625.414);  // kcal/mol F--J
        a.push_back(9573.94);  // kcal/mol G--J
        a.push_back(603.102);  // kcal/mol H--J
        b.push_back(3.93094);  // A^(-1) A--I
        b.push_back(3.77489);  // A^(-1) B--I
        b.push_back(3.22455);  // A^(-1) C--I
        b.push_back(3.42937);  // A^(-1) D--I
        b.push_back(4.01131);  // A^(-1) E--I
        b.push_back(3.76554);  // A^(-1) F--I
        b.push_back(3.44064);  // A^(-1) G--I
        b.push_back(3.33389);  // A^(-1) H--I
        b.push_back(3.90853);  // A^(-1) A--J
        b.push_back(4.26899);  // A^(-1) B--J
        b.push_back(3.4567);  // A^(-1) C--J
        b.push_back(3.73928);  // A^(-1) D--J
        b.push_back(3.04496);  // A^(-1) E--J
        b.push_back(3.47858);  // A^(-1) F--J
        b.push_back(4.18877);  // A^(-1) G--J
        b.push_back(3.33517);  // A^(-1) H--J
        // =====>> END SECTION BUCKINGHAM <<=====
    } else {
        out_a = 0.0;
        out_b = 0.0;
        return false;
    }

    i = types1[index1];
    j = types2[index2];

    out_a = a[i * nt2 + j];
    out_b = b[i * nt2 + j];

    return true;
}

}  // namespace buck

//----------------------------------------------------------------------------//
