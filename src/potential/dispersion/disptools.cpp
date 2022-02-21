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

#include "potential/dispersion/disptools.h"

namespace disp {

double tang_toennies(int n, const double& x) {
#ifdef DEBUG
    std::cerr << std::scientific << std::setprecision(10);
    std::cerr << "\nEntering " << __func__ << " in " << __FILE__ << std::endl;
    std::cerr << "n = " << n << " , x = " << x << std::endl;
#endif

    assert(n >= 0);
    int nn = n;
    double sum = 1.0 + x / nn;

    while (--nn != 0) sum = 1.0 + sum * x / nn;

    double tt = 1.0 - sum * std::exp(-x);

    if (std::fabs(tt) < 1.0e-8) {
        double term(1);
        for (nn = n; nn != 0; --nn) term *= x / nn;

        sum = 0.0;
        for (nn = n + 1; nn < 1000; ++nn) {
            term *= x / nn;
            sum += term;

            if (std::fabs(term / sum) < 1.0e-8) break;
        }

        tt = sum * std::exp(-x);
    }

#ifdef DEBUG
    std::cerr << std::scientific << std::setprecision(10);
    std::cerr << "\nExiting " << __func__ << " in " << __FILE__ << std::endl;
    std::cerr << "tt = " << tt << std::endl;
#endif

    return tt;
}

//----------------------------------------------------------------------------//

double disp6(const double C6, const double d6, const double c6i, const double c6j, const std::vector<double>& p1,
             const std::vector<double>& xyz2, std::vector<double>& grad1, std::vector<double>& grad2, double& phi1,
             std::vector<double>& phi2, const size_t nmon1, const size_t nmon2, const size_t start2, const size_t end2,
             const size_t atom_index1, const size_t atom_index2, const double disp_scale_factor, bool do_grads,
             const double cutoff, const double ewald_alpha, const std::vector<double>& box,
             const std::vector<double>& box_inverse, bool use_ghost, const std::vector<size_t>& islocal,
             const size_t isl1_offset, const size_t isl2_offset, std::vector<double>* virial) {
#ifdef DEBUG
    std::cerr << std::scientific << std::setprecision(10);
    std::cerr << "\nEntering " << __func__ << " in " << __FILE__ << std::endl;
    std::cerr << "C6 = " << C6 << " , d6 = " << d6 << " , c6i = " << c6i << " , c6j = " << c6j << std::endl;

    std::cerr << "p1:\n ";
    for (size_t i = 0; i < p1.size(); i++) {
        std::cerr << p1[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "xyz2:\n ";
    for (size_t i = 0; i < xyz2.size(); i++) {
        std::cerr << xyz2[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "grad1:\n ";
    for (size_t i = 0; i < grad1.size(); i++) {
        std::cerr << grad1[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "grad2:\n ";
    for (size_t i = 0; i < grad2.size(); i++) {
        std::cerr << grad2[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "phi1 = " << phi1 << std::endl;

    std::cerr << "phi2:\n ";
    for (size_t i = 0; i < phi2.size(); i++) {
        std::cerr << phi2[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "nmon1 = " << nmon1 << " , nmon2 = " << nmon2 << " , start2 = " << start2 << " , end2 = " << end2
              << std::endl;
    std::cerr << "atom_index1 = " << atom_index1 << " , atom_index2 = " << atom_index2
              << " , dip_scale_factor = " << disp_scale_factor << " , do_grads = " << do_grads << std::endl;
    std::cerr << "cutoff = " << cutoff << " , ewald_alpha = " << ewald_alpha << " , use_ghost = " << use_ghost
              << std::endl;

    std::cerr << "box:\n ";
    for (size_t i = 0; i < box.size(); i++) {
        std::cerr << box[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "box inverse:\n ";
    for (size_t i = 0; i < box_inverse.size(); i++) {
        std::cerr << box_inverse[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "islocal:\n ";
    for (size_t i = 0; i < islocal.size(); i++) {
        std::cerr << islocal[i] << " , ";
    }
    std::cerr << std::endl;

    if (virial != 0) {
        std::cerr << "virial:\n ";
        for (size_t i = 0; i < (*virial).size(); i++) {
            std::cerr << (*virial)[i] << " , ";
        }
        std::cerr << std::endl;
    }

    std::cerr << "isl1_offset = " << isl1_offset << " , isl2_offset = " << isl2_offset << std::endl;

#endif

    double disp = 0.0;
    double disp_lr_below_cutoff = 0.0;

    size_t nmon22 = nmon2 * 2;

    size_t shift_phi = atom_index2 * nmon2;
    size_t shift2 = shift_phi * 3;

    bool use_pbc = box.size();
    double g1[3], g2[3 * nmon2];
    std::fill(g1, g1 + 3, 0.0);
    std::fill(g2, g2 + 3 * nmon2, 0.0);
    //    #pragma simd
    const double* boxinv = box_inverse.data();
    const double* boxptr = box.data();
    double dispersion_energy = 0;
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

        const double inv_rsq = 1.0 / rsq;
        const double inv_r6 = inv_rsq * inv_rsq * inv_rsq;

        // Update phi for long range interactions
        // phi1 is a double value passed by reference
        // phi2 is a double array

        phi1 -= c6j * inv_r6;
        phi2[shift_phi + nv] -= c6i * inv_r6;

        bool include_pair = false;
        size_t isls = islocal[isl1_offset] + islocal[isl2_offset + nv];
        if (!use_ghost) include_pair = true;
        if (use_ghost && isls) include_pair = true;

        // If using cutoff, check for distances and get proper dispersion
        if (r <= cutoff && include_pair) {
            const double d6r = d6 * r;
            const double tt6 = disp::tang_toennies(6, d6r);

            const double inv_rsq = 1.0 / rsq;
            const double inv_r6 = inv_rsq * inv_rsq * inv_rsq;

            // Intermediates used in the dispersion PME terms
            double ar2 = ewald_alpha * ewald_alpha * rsq;
            double ar4 = ar2 * ar2;
            double ar6 = ar4 * ar2;
            double expterm = ewald_alpha ? std::exp(-ar2) : 1;

            const double e6 = C6 * tt6 * inv_r6;

            double ttsw_grad = 0;
            const double ttsw = switch_function(r, cutoff - 1.0, cutoff, ttsw_grad);
            const double c6sw = 1 - ttsw;
            const double c6sw_grad = -ttsw_grad;

            // The idea here is quite simple.  At short range we want the TT term (e6) to model dispersion.  At long
            // range this becomes C6i C6j / Rij^6, which is handled by PME.  The reciprocal space part of PME always
            // includes extra terms that contribute below the cutoff, even if that pair shouldn't contribute.  For
            // intermonomer pairs, this means there is the TT contribution that we want, but we have to remove the
            // part of the reciprocal space from C6i C6j / Rij^6 that was added in the reciprocal space term.  Similarly
            // for intramonomer terms, there should be no TT contribution or C6i C6j / Rij^6 term, so we use the scale
            // factor to prevent TT contributing, and then back out the reciprocal space C6i C6j / Rij^6 contribution.
            // See http://dx.doi.org/10.1021/acs.jctc.5b00726 for more details of this trick.
            double c6term = c6i * c6j * inv_r6;
            double pmeterm = c6i * c6j * (1 - (1 + ar2 + ar4 / 2) * expterm) * inv_r6;
            double pair_energy = ttsw * (disp_scale_factor * e6) + c6sw * disp_scale_factor * c6term - pmeterm;

            if (isls == 1) pair_energy *= 0.5;
            dispersion_energy -= pair_energy;

            if (do_grads) {
                const double e6term_grad = 6 * e6 * inv_rsq - C6 * std::pow(d6, 7) * if6 * std::exp(-d6r) / r;
                const double c6term_grad = 6 * c6term * inv_rsq;
                const double pmeterm_grad =
                    6 * c6i * c6j * (1 - (1 + ar2 + ar4 / 2 + ar6 / 6) * expterm) * inv_r6 * inv_rsq;
                const double ttgrad = ttsw * e6term_grad - ttsw_grad * e6 / r;
                const double c6grad = c6sw * c6term_grad - c6sw_grad * c6term / r;
                const double grad = disp_scale_factor * (ttgrad + c6grad) - pmeterm_grad;

                g1[0] += dx * grad;
                g2[nv] -= dx * grad;

                g1[1] += dy * grad;
                g2[nmon2 + nv] -= dy * grad;

                g1[2] += dz * grad;
                g2[nmon22 + nv] -= dz * grad;

                if (virial != 0) {
                    const double vscale = (isls == 1) ? 0.5 : 1.0;

                    (*virial)[0] -= dx * dx * grad * vscale;  //  update the virial for the atom pair
                    (*virial)[1] -= dx * dy * grad * vscale;
                    (*virial)[2] -= dx * dz * grad * vscale;

                    (*virial)[4] -= dy * dy * grad * vscale;
                    (*virial)[5] -= dy * dz * grad * vscale;

                    (*virial)[8] -= dz * dz * grad * vscale;

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

#ifdef DEBUG
    std::cerr << std::scientific << std::setprecision(10);
    std::cerr << "\nExiting " << __func__ << " in " << __FILE__ << std::endl;

    std::cerr << "grad1:\n ";
    for (size_t i = 0; i < grad1.size(); i++) {
        std::cerr << grad1[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "grad2:\n ";
    for (size_t i = 0; i < grad2.size(); i++) {
        std::cerr << grad2[i] << " , ";
    }
    std::cerr << std::endl;

    std::cerr << "phi1 = " << phi1 << std::endl;

    std::cerr << "phi2:\n ";
    for (size_t i = 0; i < phi2.size(); i++) {
        std::cerr << phi2[i] << " , ";
    }
    std::cerr << std::endl;

    if (virial != 0) {
        std::cerr << "virial:\n ";
        for (size_t i = 0; i < (*virial).size(); i++) {
            std::cerr << (*virial)[i] << " , ";
        }
        std::cerr << std::endl;
    }

    std::cerr << "dispersion_energy = " << dispersion_energy << std::endl;
#endif

    return dispersion_energy;
}

bool GetC6(std::string mon_id1, std::string mon_id2, size_t index1, size_t index2, double& out_C6, double& out_d6,
           std::vector<std::pair<std::string, std::string> > ignore_disp, nlohmann::json repdisp_j) {
    // Order the two monomer names and corresponding xyz
    bool swaped = false;
    if (mon_id2 < mon_id1) {
        std::string tmp = mon_id1;
        mon_id1 = mon_id2;
        mon_id2 = tmp;
        size_t tmp1 = index1;
        index1 = index2;
        index2 = tmp1;
        swaped = true;
    }

    std::vector<double> C6, d6;
    size_t nt2, i, j;

    out_C6 = 0.0;
    out_d6 = 0.0;

    if (std::find(ignore_disp.begin(), ignore_disp.end(), std::make_pair(mon_id1, mon_id2)) != ignore_disp.end() ||
        std::find(ignore_disp.begin(), ignore_disp.end(), std::make_pair(mon_id2, mon_id1)) != ignore_disp.end()) {
        out_C6 = 0.0;
        out_d6 = 0.0;
        return false;
    }

    bool done_with_it = false;

    // Check if pair is in json object
    try {
        std::vector<std::vector<std::string> > pairs = repdisp_j["pairs"];
        for (size_t k = 0; k < pairs.size(); k++) {
            if (mon_id1 == pairs[k][0] && mon_id2 == pairs[k][1]) {
                std::vector<std::vector<std::string> > types1 = repdisp_j["types1"];
                std::vector<std::vector<std::string> > types2 = repdisp_j["types2"];
                std::vector<std::vector<std::pair<std::vector<std::string>, double> > > c6_v = repdisp_j["c6"];
                std::vector<std::vector<std::pair<std::vector<std::string>, double> > > d6_v = repdisp_j["d6"];
                std::string si = types1[k][index1];
                std::string sj = types2[k][index2];

                for (size_t k2 = 0; k2 < c6_v[k].size(); k2++) {
                    if ((si == c6_v[k][k2].first[0] && sj == c6_v[k][k2].first[1]) ||
                        (si == c6_v[k][k2].first[1] && sj == c6_v[k][k2].first[0])) {
                        out_C6 = c6_v[k][k2].second;
                        done_with_it = true;
                        break;
                    }
                }

                for (size_t k2 = 0; k2 < d6_v[k].size(); k2++) {
                    if ((si == d6_v[k][k2].first[0] && sj == d6_v[k][k2].first[1]) ||
                        (si == d6_v[k][k2].first[1] && sj == d6_v[k][k2].first[0])) {
                        out_d6 = d6_v[k][k2].second;
                        done_with_it = true;
                        break;
                    }
                }
            }
        }
    } catch (...) {
        out_C6 = 0.0;
        out_d6 = 0.0;
    }

    if (done_with_it) return true;
    std::vector<size_t> types1, types2;

    // Monomers here have to be in alphabetical order: mon1 < mon2 ALWAYS
    if (mon_id1 == "h2o" && mon_id2 == "h2o") {
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
        C6.push_back(2.373212214147944e+02);  // kcal/mol * A^(-6) O -- O
        C6.push_back(8.349556669872743e+01);  // kcal/mol * A^(-6) O -- H
        C6.push_back(8.349556669872743e+01);  // kcal/mol * A^(-6) H -- O
        C6.push_back(2.009358600184719e+01);  // kcal/mol * A^(-6) H -- H

        d6.push_back(9.295485815062264e+00);  // A^(-1)
        d6.push_back(9.775202425217957e+00);  // A^(-1)
        d6.push_back(9.775202425217957e+00);  // A^(-1)
        d6.push_back(9.406475169954112e+00);  // A^(-1)

    } else if (mon_id1 == "f" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(3.488640000000000e+02);  // kcal/mol * A^(-6) F -- O
        C6.push_back(1.286780000000000e+02);  // kcal/mol * A^(-6) F -- H

        d6.push_back(3.586190000000000e+00);  // A^(-1)
        d6.push_back(2.697680000000000e+00);  // A^(-1)
    } else if (mon_id1 == "cl" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(7.461990000000000e+02);  // kcal/mol * A^(-6) Cl -- O
        C6.push_back(3.068900000000000e+02);  // kcal/mol * A^(-6) Cl -- H

        d6.push_back(3.275420000000000e+00);  // A^(-1)
        d6.push_back(2.782260000000000e+00);  // A^(-1)
    } else if (mon_id1 == "br" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        // Set the number of different types
        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(9.426500000000000e+02);  // kcal/mol * A^(-6) Br -- O
        C6.push_back(3.941680000000000e+02);  // kcal/mol * A^(-6) Br -- H

        d6.push_back(3.058250000000000e+00);  // A^(-1)
        d6.push_back(2.798040000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "i") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(1.294680000000000e+03);  // kcal/mol * A^(-6) I -- O
        C6.push_back(5.681559999999999e+02);  // kcal/mol * A^(-6) I -- H

        d6.push_back(2.723140000000000e+00);  // A^(-1)
        d6.push_back(2.799110000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "li") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(4.314700000000000e+01);  // kcal/mol * A^(-6) Li -- O
        C6.push_back(1.871220000000000e+01);  // kcal/mol * A^(-6) Li -- H

        d6.push_back(4.023330000000000e+00);  // A^(-1)
        d6.push_back(4.006630000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "na") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(1.762550000000000e+02);  // kcal/mol * A^(-6) Na -- O
        C6.push_back(8.578690000000000e+01);  // kcal/mol * A^(-6) Na -- H

        d6.push_back(3.769530000000000e+00);  // A^(-1)
        d6.push_back(3.822550000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "k") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(4.095830000000000e+02);  // kcal/mol * A^(-6) K -- O
        C6.push_back(2.181920000000000e+02);  // kcal/mol * A^(-6) K -- H

        d6.push_back(3.401250000000000e+00);  // A^(-1)
        d6.push_back(3.321390000000000e+00);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "rb") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(5.318040000000000e+02);  // kcal/mol * A^(-6) Rb -- O
        C6.push_back(2.861310000000000e+02);  // kcal/mol * A^(-6) Rb -- H

        d6.push_back(3.236530000000000e+00);  // A^(-1)
        d6.push_back(3.313640000000000e+00);  // A^(-1)
    } else if (mon_id1 == "cs" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(7.045400000000000e+02);  // kcal/mol * A^(-6) Cs -- O
        C6.push_back(3.845670000000000e+02);  // kcal/mol * A^(-6) Cs -- H

        d6.push_back(3.028640000000000e+00);  // A^(-1)
        d6.push_back(3.271530000000000e+00);  // A^(-1)
    } else if (mon_id1 == "he" and mon_id2 == "he") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(24.348011);  // kcal/mol * A^(-6) He -- He

        d6.push_back(4.02693);  // A^(-1)
        d6.push_back(4.02693);  // A^(-1)
    } else if (mon_id1 == "h2o" and mon_id2 == "he") {
        // Define the type of atom in each mon
        types2.push_back(0);

        types1.push_back(0);
        types1.push_back(1);
        types1.push_back(1);

        // Set the number of different types
        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(63.246969);  // kcal/mol * A^(-6) He -- O
        C6.push_back(26.208221);  // kcal/mol * A^(-6) He -- H

        d6.push_back(3.71873);  // A^(-1)
        d6.push_back(3.9707);   // A^(-1)
    } else if (mon_id1 == "ar" and mon_id2 == "h2o") {
        // Define the type of atom in each mon
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(382.1275);  // kcal/mol * A^(-6) Cs -- O
        C6.push_back(170.8771);  // kcal/mol * A^(-6) Cs -- H

        d6.push_back(3.43864);  // A^(-1)
        d6.push_back(3.45707);  // A^(-1)
        // =====>> BEGIN SECTION DISPERSION <<=====
        // ======>> PASTE CODE BELOW <<======
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
        C6.push_back(321.00932864);  // kcal/mol * A^(-6) C -- C
        C6.push_back(219.55020747);  // kcal/mol * A^(-6) C -- O
        C6.push_back(219.55020747);  // kcal/mol * A^(-6) O -- C
        C6.push_back(170.09525896);  // kcal/mol * A^(-6) O -- O

        d6.push_back(3.12663);  // A^(-1)
        d6.push_back(3.64236);  // A^(-1)
        d6.push_back(3.64236);  // A^(-1)
        d6.push_back(3.52744);  // A^(-1)

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
        C6.push_back(243.7007);  // kcal/mol * A^(-6)  A--A
        C6.push_back(98.6092);   // kcal/mol * A^(-6)  A--B
        C6.push_back(98.6092);   // kcal/mol * A^(-6)  A--B
        C6.push_back(39.9906);   // kcal/mol * A^(-6)  B--B
        d6.push_back(3.09382);   // A^(-1) A--A
        d6.push_back(3.44698);   // A^(-1) A--B
        d6.push_back(3.44698);   // A^(-1) A--B
        d6.push_back(3.83901);   // A^(-1) B--B

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
        C6.push_back(243.7007);  // kcal/mol * A^(-6)  A--A
        C6.push_back(98.6092);   // kcal/mol * A^(-6)  A--B
        C6.push_back(98.6092);   // kcal/mol * A^(-6)  A--B
        C6.push_back(39.9906);   // kcal/mol * A^(-6)  B--B
        d6.push_back(3.11493);   // A^(-1) A--A
        d6.push_back(3.4174);    // A^(-1) A--B
        d6.push_back(3.4174);    // A^(-1) A--B
        d6.push_back(3.78007);   // A^(-1) B--B

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
        C6.push_back(303.247);      // kcal/mol * A^(-6) C -- C
        C6.push_back(104.10825);    // kcal/mol * A^(-6) C -- O
        C6.push_back(104.10825);    // kcal/mol * A^(-6) O -- C
        C6.push_back(36.78116875);  // kcal/mol * A^(-6) O -- O

        d6.push_back(3.37925);  // A^(-1)
        d6.push_back(3.25885);  // A^(-1)
        d6.push_back(3.25885);  // A^(-1)
        d6.push_back(4.05972);  // A^(-1)
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
        C6.push_back(222.003);  // kcal/mol * A^(-6) C -- O
        C6.push_back(105.979);  // kcal/mol * A^(-6) C -- H
        C6.push_back(80.628);   // kcal/mol * A^(-6) O -- O
        C6.push_back(37.176);   // kcal/mol * A^(-6) O -- H

        d6.push_back(2.87176);  // A^(-1)
        d6.push_back(3.68542);  // A^(-1)
        d6.push_back(3.79757);  // A^(-1)
        d6.push_back(4.01558);  // A^(-1)
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
        C6.push_back(225.5857);  // kcal/mol * A^(-6) C -- O
        C6.push_back(130.8452);  // kcal/mol * A^(-6) C -- H
        C6.push_back(208.0754);  // kcal/mol * A^(-6) O -- O
        C6.push_back(94.19873);  // kcal/mol * A^(-6) O -- H

        d6.push_back(2.93819);  // A^(-1)
        d6.push_back(3.7359);   // A^(-1)
        d6.push_back(3.53045);  // A^(-1)
        d6.push_back(3.89503);  // A^(-1)

    } else if (mon_id1 == "ar" and mon_id2 == "cs") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(1500.0914);  // kcal/mol * A^(-6)  A--B
        d6.push_back(3.28039);    // A^(-1) A--B
    } else if (mon_id1 == "ar" and mon_id2 == "ar") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(774.5257);  // kcal/mol * A^(-6)  A--B
        d6.push_back(3.41808);   // A^(-1) A--B
    } else if (mon_id1 == "h2" and mon_id2 == "h2") {
        types1.push_back(0);
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(45.4166);  // kcal/mol * A^(-6)  A--A
        d6.push_back(3.11276);  // A^(-1) A--A
    } else if (mon_id1 == "h2" and mon_id2 == "h2o") {
        types1.push_back(0);
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(91.2878);  // kcal/mol * A^(-6)  A--C
        C6.push_back(40.6489);  // kcal/mol * A^(-6)  B--C
        d6.push_back(3.05339);  // A^(-1) A--C
        d6.push_back(3.62823);  // A^(-1) B--C
    } else if (mon_id1 == "ar" and mon_id2 == "h2o") {
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(1);
        types2.push_back(1);

        nt2 = 2;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(382.031);   // kcal/mol * A^(-6)  A--B
        C6.push_back(170.8082);  // kcal/mol * A^(-6)  A--C
        d6.push_back(3.48054);   // A^(-1) A--B
        d6.push_back(3.46238);   // A^(-1) A--C
    } else if (mon_id1 == "cs" and mon_id2 == "h2") {
        types1.push_back(0);

        types2.push_back(0);
        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(372.8086);  // kcal/mol * A^(-6)  A--B
        d6.push_back(3.24781);   // A^(-1) A--B
    } else if (mon_id1 == "na" and mon_id2 == "na") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(251.48);   // kcal/mol * A^(-6)  A--A
        d6.push_back(4.42822);  // A^(-1) A--A
    } else if (mon_id1 == "cl" and mon_id2 == "cl") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(3066.2919);  // kcal/mol * A^(-6)  A--A
        d6.push_back(1.82786);    // A^(-1) A--A
    } else if (mon_id1 == "cl" and mon_id2 == "na") {
        types1.push_back(0);

        types2.push_back(0);

        nt2 = 1;

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(784.5853);  // kcal/mol * A^(-6)  A--B
        d6.push_back(2.85113);   // A^(-1) A--B

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

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(169.526699905518);    // kcal/mol * A^(-6)  A--A
        C6.push_back(165.351572694970);    // kcal/mol * A^(-6)  A--B
        C6.push_back(174.39423682672617);  // kcal/mol * A^(-6)  A--C
        C6.push_back(165.351572694970);    // kcal/mol * A^(-6)  B--A
        C6.push_back(171.35934650288723);  // kcal/mol * A^(-6)  B--B
        C6.push_back(165.351572694970);    // kcal/mol * A^(-6)  B--C
        C6.push_back(174.39423682672617);  // kcal/mol * A^(-6)  C--A
        C6.push_back(165.351572694970);    // kcal/mol * A^(-6)  C--B
        C6.push_back(179.62003548877342);  // kcal/mol * A^(-6)  C--C

        d6.push_back(3.86891);  // A^(-1) A--A
        d6.push_back(3.19945);  // A^(-1) A--B
        d6.push_back(3.60907);  // A^(-1) A--C
        d6.push_back(3.19945);  // A^(-1) B--A
        d6.push_back(2.33813);  // A^(-1) B--B
        d6.push_back(4.19428);  // A^(-1) B--C
        d6.push_back(3.60907);  // A^(-1) C--A
        d6.push_back(4.19428);  // A^(-1) C--B
        d6.push_back(3.56601);  // A^(-1) C--C
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

        // Fill in (in order) the C6 and d6 coefficients
        C6.push_back(176.36123157691006);  // kcal/mol * A^(-6)  D--A
        C6.push_back(170.03653392132748);  // kcal/mol * A^(-6)  D--B
        C6.push_back(181.7351576894966);   // kcal/mol * A^(-6)  D--C
        C6.push_back(78.55577632783532);   // kcal/mol * A^(-6)  E--A
        C6.push_back(79.65812014580179);   // kcal/mol * A^(-6)  E--B
        C6.push_back(80.37292121526443);   // kcal/mol * A^(-6)  E--C

        d6.push_back(4.1869);   // A^(-1) D--A
        d6.push_back(3.22662);  // A^(-1) D--B
        d6.push_back(4.455);    // A^(-1) D--C
        d6.push_back(3.24055);  // A^(-1) E--A
        d6.push_back(6.4749);   // A^(-1) E--B
        d6.push_back(3.03227);  // A^(-1) E--C
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
        C6.push_back(285.259);  // kcal/mol * A^(-6)  A--A
        C6.push_back(217.0515);  // kcal/mol * A^(-6)  A--B
        C6.push_back(282.765);  // kcal/mol * A^(-6)  A--C
        C6.push_back(96.8133);  // kcal/mol * A^(-6)  A--D
        C6.push_back(213.7169);  // kcal/mol * A^(-6)  A--E
        C6.push_back(93.1343);  // kcal/mol * A^(-6)  A--F
        C6.push_back(282.1311);  // kcal/mol * A^(-6)  A--G
        C6.push_back(97.4931);  // kcal/mol * A^(-6)  A--H
        C6.push_back(217.0515);  // kcal/mol * A^(-6)  A--B
        C6.push_back(201.2604);  // kcal/mol * A^(-6)  B--B
        C6.push_back(222.4943);  // kcal/mol * A^(-6)  B--C
        C6.push_back(83.3556);  // kcal/mol * A^(-6)  B--D
        C6.push_back(178.8002);  // kcal/mol * A^(-6)  B--E
        C6.push_back(79.0243);  // kcal/mol * A^(-6)  B--F
        C6.push_back(221.8054);  // kcal/mol * A^(-6)  B--G
        C6.push_back(83.9664);  // kcal/mol * A^(-6)  B--H
        C6.push_back(282.765);  // kcal/mol * A^(-6)  A--C
        C6.push_back(222.4943);  // kcal/mol * A^(-6)  B--C
        C6.push_back(282.0071);  // kcal/mol * A^(-6)  C--C
        C6.push_back(98.0627);  // kcal/mol * A^(-6)  C--D
        C6.push_back(215.4393);  // kcal/mol * A^(-6)  C--E
        C6.push_back(94.0988);  // kcal/mol * A^(-6)  C--F
        C6.push_back(281.3319);  // kcal/mol * A^(-6)  C--G
        C6.push_back(98.7608);  // kcal/mol * A^(-6)  C--H
        C6.push_back(96.8133);  // kcal/mol * A^(-6)  A--D
        C6.push_back(83.3556);  // kcal/mol * A^(-6)  B--D
        C6.push_back(98.0627);  // kcal/mol * A^(-6)  C--D
        C6.push_back(35.5292);  // kcal/mol * A^(-6)  D--D
        C6.push_back(77.0538);  // kcal/mol * A^(-6)  D--E
        C6.push_back(33.8787);  // kcal/mol * A^(-6)  D--F
        C6.push_back(97.7871);  // kcal/mol * A^(-6)  D--G
        C6.push_back(35.791);  // kcal/mol * A^(-6)  D--H
        C6.push_back(213.7169);  // kcal/mol * A^(-6)  A--E
        C6.push_back(178.8002);  // kcal/mol * A^(-6)  B--E
        C6.push_back(215.4393);  // kcal/mol * A^(-6)  C--E
        C6.push_back(77.0538);  // kcal/mol * A^(-6)  D--E
        C6.push_back(167.7905);  // kcal/mol * A^(-6)  E--E
        C6.push_back(73.609);  // kcal/mol * A^(-6)  E--F
        C6.push_back(214.8606);  // kcal/mol * A^(-6)  E--G
        C6.push_back(77.605);  // kcal/mol * A^(-6)  E--H
        C6.push_back(93.1343);  // kcal/mol * A^(-6)  A--F
        C6.push_back(79.0243);  // kcal/mol * A^(-6)  B--F
        C6.push_back(94.0988);  // kcal/mol * A^(-6)  C--F
        C6.push_back(33.8787);  // kcal/mol * A^(-6)  D--F
        C6.push_back(73.609);  // kcal/mol * A^(-6)  E--F
        C6.push_back(32.3262);  // kcal/mol * A^(-6)  F--F
        C6.push_back(93.8508);  // kcal/mol * A^(-6)  F--G
        C6.push_back(34.1129);  // kcal/mol * A^(-6)  F--H
        C6.push_back(282.1311);  // kcal/mol * A^(-6)  A--G
        C6.push_back(221.8054);  // kcal/mol * A^(-6)  B--G
        C6.push_back(281.3319);  // kcal/mol * A^(-6)  C--G
        C6.push_back(97.7871);  // kcal/mol * A^(-6)  D--G
        C6.push_back(214.8606);  // kcal/mol * A^(-6)  E--G
        C6.push_back(93.8508);  // kcal/mol * A^(-6)  F--G
        C6.push_back(280.6705);  // kcal/mol * A^(-6)  G--G
        C6.push_back(98.4829);  // kcal/mol * A^(-6)  G--H
        C6.push_back(97.4931);  // kcal/mol * A^(-6)  A--H
        C6.push_back(83.9664);  // kcal/mol * A^(-6)  B--H
        C6.push_back(98.7608);  // kcal/mol * A^(-6)  C--H
        C6.push_back(35.791);  // kcal/mol * A^(-6)  D--H
        C6.push_back(77.605);  // kcal/mol * A^(-6)  E--H
        C6.push_back(34.1129);  // kcal/mol * A^(-6)  F--H
        C6.push_back(98.4829);  // kcal/mol * A^(-6)  G--H
        C6.push_back(36.0451);  // kcal/mol * A^(-6)  H--H
        d6.push_back(5.10251);  // A^(-1) A--A
        d6.push_back(0.124088);  // A^(-1) A--B
        d6.push_back(3.35382);  // A^(-1) A--C
        d6.push_back(0.199081);  // A^(-1) A--D
        d6.push_back(2.59766);  // A^(-1) A--E
        d6.push_back(1.77292);  // A^(-1) A--F
        d6.push_back(2.98567);  // A^(-1) A--G
        d6.push_back(0.0895376);  // A^(-1) A--H
        d6.push_back(0.124088);  // A^(-1) A--B
        d6.push_back(3.44299);  // A^(-1) B--B
        d6.push_back(2.58472);  // A^(-1) B--C
        d6.push_back(4.35562);  // A^(-1) B--D
        d6.push_back(3.82);  // A^(-1) B--E
        d6.push_back(2.4897);  // A^(-1) B--F
        d6.push_back(3.80478);  // A^(-1) B--G
        d6.push_back(3.72512);  // A^(-1) B--H
        d6.push_back(3.35382);  // A^(-1) A--C
        d6.push_back(2.58472);  // A^(-1) B--C
        d6.push_back(3.41495);  // A^(-1) C--C
        d6.push_back(0.298141);  // A^(-1) C--D
        d6.push_back(2.6994);  // A^(-1) C--E
        d6.push_back(3.80065);  // A^(-1) C--F
        d6.push_back(2.55108);  // A^(-1) C--G
        d6.push_back(5.40438);  // A^(-1) C--H
        d6.push_back(0.199081);  // A^(-1) A--D
        d6.push_back(4.35562);  // A^(-1) B--D
        d6.push_back(0.298141);  // A^(-1) C--D
        d6.push_back(2.79273);  // A^(-1) D--D
        d6.push_back(1.86683);  // A^(-1) D--E
        d6.push_back(1.45553);  // A^(-1) D--F
        d6.push_back(2.7634);  // A^(-1) D--G
        d6.push_back(1.41784);  // A^(-1) D--H
        d6.push_back(2.59766);  // A^(-1) A--E
        d6.push_back(3.82);  // A^(-1) B--E
        d6.push_back(2.6994);  // A^(-1) C--E
        d6.push_back(1.86683);  // A^(-1) D--E
        d6.push_back(0.0889704);  // A^(-1) E--E
        d6.push_back(3.20248);  // A^(-1) E--F
        d6.push_back(4.1739);  // A^(-1) E--G
        d6.push_back(3.15108);  // A^(-1) E--H
        d6.push_back(1.77292);  // A^(-1) A--F
        d6.push_back(2.4897);  // A^(-1) B--F
        d6.push_back(3.80065);  // A^(-1) C--F
        d6.push_back(1.45553);  // A^(-1) D--F
        d6.push_back(3.20248);  // A^(-1) E--F
        d6.push_back(4.63545);  // A^(-1) F--F
        d6.push_back(1.60478);  // A^(-1) F--G
        d6.push_back(1.80232);  // A^(-1) F--H
        d6.push_back(2.98567);  // A^(-1) A--G
        d6.push_back(3.80478);  // A^(-1) B--G
        d6.push_back(2.55108);  // A^(-1) C--G
        d6.push_back(2.7634);  // A^(-1) D--G
        d6.push_back(4.1739);  // A^(-1) E--G
        d6.push_back(1.60478);  // A^(-1) F--G
        d6.push_back(4.78365);  // A^(-1) G--G
        d6.push_back(2.29189);  // A^(-1) G--H
        d6.push_back(0.0895376);  // A^(-1) A--H
        d6.push_back(3.72512);  // A^(-1) B--H
        d6.push_back(5.40438);  // A^(-1) C--H
        d6.push_back(1.41784);  // A^(-1) D--H
        d6.push_back(3.15108);  // A^(-1) E--H
        d6.push_back(1.80232);  // A^(-1) F--H
        d6.push_back(2.29189);  // A^(-1) G--H
        d6.push_back(3.64866);  // A^(-1) H--H
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
        C6.push_back(205.3667);  // kcal/mol * A^(-6)  A--I
        C6.push_back(192.1523);  // kcal/mol * A^(-6)  B--I
        C6.push_back(210.8095);  // kcal/mol * A^(-6)  C--I
        C6.push_back(79.2861);  // kcal/mol * A^(-6)  D--I
        C6.push_back(169.8574);  // kcal/mol * A^(-6)  E--I
        C6.push_back(75.111);  // kcal/mol * A^(-6)  F--I
        C6.push_back(210.1481);  // kcal/mol * A^(-6)  G--I
        C6.push_back(79.874);  // kcal/mol * A^(-6)  H--I
        C6.push_back(99.459);  // kcal/mol * A^(-6)  A--J
        C6.push_back(85.032);  // kcal/mol * A^(-6)  B--J
        C6.push_back(100.6164);  // kcal/mol * A^(-6)  C--J
        C6.push_back(36.3406);  // kcal/mol * A^(-6)  D--J
        C6.push_back(78.8865);  // kcal/mol * A^(-6)  E--J
        C6.push_back(34.6549);  // kcal/mol * A^(-6)  F--J
        C6.push_back(100.3408);  // kcal/mol * A^(-6)  G--J
        C6.push_back(36.607);  // kcal/mol * A^(-6)  H--J
        d6.push_back(3.93094);  // A^(-1) A--I
        d6.push_back(3.77489);  // A^(-1) B--I
        d6.push_back(3.27305);  // A^(-1) C--I
        d6.push_back(3.91331);  // A^(-1) D--I
        d6.push_back(4.06027);  // A^(-1) E--I
        d6.push_back(4.07768);  // A^(-1) F--I
        d6.push_back(2.99619);  // A^(-1) G--I
        d6.push_back(3.74982);  // A^(-1) H--I
        d6.push_back(3.90853);  // A^(-1) A--J
        d6.push_back(4.26899);  // A^(-1) B--J
        d6.push_back(3.27063);  // A^(-1) C--J
        d6.push_back(4.5696);  // A^(-1) D--J
        d6.push_back(3.66502);  // A^(-1) E--J
        d6.push_back(3.885);  // A^(-1) F--J
        d6.push_back(6.57386);  // A^(-1) G--J
        d6.push_back(3.72043);  // A^(-1) H--J
        // =====>> END SECTION DISPERSION <<=====
    } else {
        out_C6 = 0.0;
        out_d6 = 0.0;
        return false;
    }

    i = types1[index1];
    j = types2[index2];

    out_C6 = C6[i * nt2 + j];
    out_d6 = d6[i * nt2 + j];

    return true;
}

}  // namespace disp

//----------------------------------------------------------------------------//
