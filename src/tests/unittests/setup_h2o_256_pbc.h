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

#ifndef UNITTESTS_SETUP_H2O_256_PBC_H
#define UNITTESTS_SETUP_H2O_256_PBC_H

#define SETUP_H2O_256_PBC                                                                                             \
    const size_t n_monomers = 256;                                                                                    \
    const size_t n_at = 3;                                                                                            \
    std::vector<std::string> monomer_names(n_monomers, "h2o");                                                        \
    std::vector<double> box = {19.568812174570969, 0.0, 0.0, 0.0, 19.568812174570969, 0.0, 0.0, 0.0,                  \
                               19.568812174570969};                                                                   \
    std::vector<std::string> atom_names(n_monomers* n_at);                                                            \
    for (size_t i = 0; i < n_monomers; i++) {                                                                         \
        atom_names[n_at * i] = "O";                                                                                   \
        atom_names[n_at * i + 1] = "H";                                                                               \
        atom_names[n_at * i + 2] = "H";                                                                               \
    }                                                                                                                 \
    std::vector<double> coords{                                                                                       \
        6.81541,    -6.23031,   -8.83384,   6.90722,     -6.70395,   -9.66585,   5.87488,    -6.02412,    -8.65157,   \
        -6.79399,   1.63692,    -4.27846,   -7.38218,    1.64854,    -5.05827,   -6.70205,   0.768067,    -3.86103,   \
        7.64997,    6.89851,    -0.44102,   6.83036,     7.24833,    -0.115663,  8.25494,    7.60494,     -0.254373,  \
        -3.95461,   -4.94007,   1.84065,    -4.89763,    -4.79708,   1.90288,    -3.55693,   -4.90211,    0.980804,   \
        3.32394,    -3.73144,   -6.41742,   4.10699,     -4.337,     -6.46042,   3.02982,    -3.96839,    -5.51857,   \
        0.726345,   1.24245,    1.88187,    1.02763,     0.412585,   1.46225,    -0.130602,  1.02727,     2.28462,    \
        0.496256,   6.47346,    5.21431,    0.598724,    5.44889,    5.07944,    -0.250295,  6.67136,     4.64599,    \
        -6.38221,   9.36494,    5.02963,    -7.15223,    9.53905,    4.48421,    -6.37272,   8.43789,     5.26921,    \
        9.67211,    0.120403,   5.13816,    10.4422,     0.448158,   4.71025,    8.97956,    0.204355,    4.48586,    \
        1.96725,    8.27909,    1.50811,    2.5319,      8.20483,    0.755243,   1.24852,    8.82818,     1.17169,    \
        -1.32789,   -4.41218,   -7.10401,   -1.1633,     -5.20824,   -6.66186,   -1.38181,   -3.78354,    -6.42653,   \
        -7.17021,   6.51819,    -4.67548,   -7.86532,    5.90843,    -4.34906,   -7.42451,   6.71248,     -5.60808,   \
        1.85512,    8.77621,    -6.51985,   1.29341,     7.97373,    -6.51375,   2.05322,    8.89386,     -7.46865,   \
        -5.17101,   -8.86064,   1.08734,    -4.59229,    -8.3332,    0.51246,    -5.78794,   -8.20901,    1.47576,    \
        -3.52763,   -4.15705,   -8.62697,   -2.89823,    -4.32231,   -7.88275,   -4.42519,   -4.29094,    -8.37853,   \
        7.35463,    2.24833,    1.65051,    7.96636,     1.64947,    1.14684,    6.45516,    1.88052,     1.54843,    \
        3.38624,    -6.53254,   -9.33126,   2.60892,     -6.01264,   -9.30079,   3.84061,    -6.33312,    -10.1547,   \
        6.52279,    -0.0989697, -1.95241,   6.79579,     0.792672,   -2.05622,   6.5022,     -0.172618,   -0.995695,  \
        -8.08327,   -8.27382,   -4.24559,   -8.02471,    -9.02875,   -3.73319,   -7.73003,   -8.63554,    -5.1163,    \
        -8.51421,   3.50073,    8.85441,    -8.36709,    4.36456,    9.30553,    -8.99222,   2.89636,     9.42944,    \
        -5.60712,   -6.0561,    -1.97174,   -6.33465,    -6.18122,   -1.35633,   -6.04711,   -6.01297,    -2.85543,   \
        9.45386,    1.13735,    0.363252,   9.91472,     0.371879,   0.0161044,  10.1083,    1.72139,     0.761287,   \
        2.14302,    4.07473,    -8.79265,   1.36572,     3.77612,    -9.36336,   1.73476,    4.26876,     -7.89891,   \
        7.16016,    8.96605,    7.95549,    8.08609,     8.70728,    7.90126,    6.75293,    8.43809,     8.65367,    \
        -3.63789,   -7.74918,   -0.759266,  -4.37941,    -7.30046,   -1.21143,   -2.95622,   -7.09084,    -0.549366,  \
        -6.22166,   -9.73749,   9.65873,    -6.58247,    -10.439,    9.04091,    -6.58507,   -8.88229,    9.39688,    \
        5.64052,    2.7912,     -0.820655,  6.56611,     3.21493,    -0.906928,  5.01446,    3.40455,     -1.22551,   \
        7.14039,    -6.72026,   1.31532,    7.86852,     -6.7957,    0.661683,   6.85766,    -7.70926,    1.45935,    \
        -9.54282,   -2.75836,   -8.07165,   -8.65694,    -2.74597,   -8.49503,   -9.59297,   -3.67853,    -7.79314,   \
        9.49129,    7.72719,    4.56252,    8.95546,     8.41114,    4.22893,    8.92113,    7.00961,     4.8154,     \
        3.42596,    -8.49596,   -3.09512,   2.732,       -8.39303,   -3.75751,   2.96348,    -8.80662,    -2.32383,   \
        7.66636,    3.05736,    4.25835,    7.71257,     2.73775,    3.3609,     7.51942,    2.25341,     4.78112,    \
        7.50107,    -8.10543,   -7.03791,   6.64628,     -8.50034,   -6.87528,   7.32989,    -7.6563,     -7.896,     \
        0.947922,   -6.61202,   6.38351,    0.4145,      -5.99799,   5.8016,     0.504401,   -6.63232,    7.24392,    \
        -0.618453,  -0.686212,  -8.18369,   0.101136,    -0.973593,  -7.57309,   -0.642437,  -1.43456,    -8.79105,   \
        4.08262,    -0.705076,  7.49133,    4.20241,     -1.61878,   7.82038,    3.15914,    -0.415014,   7.68195,    \
        5.06593,    7.44897,    0.522988,   5.01803,     6.57837,    0.949349,   4.79454,    7.26988,     -0.361623,  \
        -1.44013,   7.92732,    8.57392,    -1.38497,    8.73963,    7.91047,    -0.506228,  7.57142,     8.49665,    \
        0.242787,   -4.28018,   -2.56878,   1.11565,     -4.04358,   -2.83175,   0.110999,   -3.60778,    -1.82997,   \
        3.70688,    6.70393,    -1.5698,    3.96643,     5.79319,    -1.83309,   2.76616,    6.77958,     -1.95194,   \
        8.11649,    -4.95937,   5.4121,     8.00188,     -5.84905,   5.00673,    9.06788,    -4.85473,    5.56207,    \
        1.58523,    0.93641,    -1.73968,   2.01336,     1.51112,    -1.17122,   2.11017,    0.135569,    -1.534,     \
        1.13603,    -4.81581,   -8.53954,   0.208894,    -4.89232,   -8.20227,   1.64114,    -5.21836,    -7.86686,   \
        -6.85811,   7.50033,    8.52259,    -7.77266,    7.45152,    8.19787,    -6.99022,   6.91156,     9.28046,    \
        3.74422,    -7.41448,   1.17088,    4.45686,     -8.02845,   1.45133,    3.11043,    -7.86117,    0.561787,   \
        -4.6828,    -3.49616,   -0.742654,  -4.29252,    -2.8458,    -1.35838,   -5.00379,   -4.23529,    -1.24891,   \
        -3.98766,   -8.91671,   4.14618,    -3.75835,    -9.19402,   3.31875,    -4.8041,    -9.40702,    4.27684,    \
        -4.41896,   0.09055,    -5.47019,   -5.18364,    -0.390714,  -5.8137,    -4.48944,   0.913952,    -6.01008,   \
        -2.37644,   6.71821,    1.05017,    -2.63744,    5.81431,    1.1349,     -3.15675,   7.19671,     1.33732,    \
        2.78594,    9.18336,    7.70468,    1.99185,     8.52357,    7.6736,     2.61215,    9.87698,     7.03382,    \
        -6.60074,   3.18648,    -2.01498,   -6.97516,    2.83525,    -2.85711,   -6.12835,   2.47731,     -1.58844,   \
        5.19055,    -6.01028,   7.83324,    4.72759,     -6.22945,   7.01565,    5.41803,    -5.05442,    7.80184,    \
        6.69188,    0.516421,   5.72726,    5.95145,     0.707061,   5.17425,    6.29549,    0.319155,    6.56771,    \
        -2.15173,   3.4014,     -1.92125,   -1.82392,    2.61781,    -1.48833,   -3.00431,   3.7422,      -1.49605,   \
        -7.97925,   4.58032,    6.33763,    -7.78476,    3.91918,    5.76312,    -8.13317,   4.22467,     7.22283,    \
        -4.14844,   -8.28501,   7.71733,    -4.27032,    -7.34607,   7.88242,    -4.9979,    -8.69497,    7.77827,    \
        -4.1727,    -9.7376,    -3.75572,   -4.94971,    -9.7088,    -3.12998,   -3.48233,   -10.2678,    -3.44045,   \
        -2.95309,   3.99831,    1.77948,    -2.22688,    3.56006,    2.08355,    -3.72355,   3.3871,      1.94449,    \
        5.08849,    -3.54107,   1.255,      4.75357,     -3.9259,    2.12063,    5.44045,    -4.21946,    0.714631,   \
        -2.46071,   -7.98385,   -5.16226,   -1.97649,    -7.28601,   -4.79609,   -3.01916,   -8.41485,    -4.50612,   \
        -4.72335,   4.4669,     7.82626,    -5.10797,    4.97409,    8.61173,    -5.40971,   4.60602,     7.15326,    \
        5.72301,    -5.88865,   -6.07744,   5.57426,     -6.03875,   -5.09541,   6.41155,    -6.50877,    -6.26188,   \
        -4.03236,   1.59457,    -9.50821,   -3.30637,    1.85261,    -10.1211,   -3.70918,   0.760645,    -9.16127,   \
        -8.97938,   -2.13245,   6.78266,    -9.45205,    -1.48052,   6.2098,     -9.45093,   -2.22519,    7.6371,     \
        5.01014,    8.21194,    -9.36497,   5.67792,     7.93088,    -8.71302,   4.47052,    8.92611,     -8.98217,   \
        8.85706,    -0.168032,  -6.82305,   8.13364,     -0.570117,  -6.3003,    9.2858,     -0.980009,   -6.96525,   \
        4.51728,    1.04454,    1.45643,    4.99968,     0.31375,    1.1158,     4.42596,    1.77107,     0.790449,   \
        0.989615,   7.08246,    7.98993,    0.939032,    6.97412,    7.04121,    1.55962,    6.47457,     8.41886,    \
        9.33335,    4.29123,    -6.60643,   8.43707,     4.19043,    -6.7254,    9.75321,    3.70882,     -7.23354,   \
        -4.54658,   4.94685,    -2.32626,   -5.18684,    4.22295,    -2.24453,   -4.94157,   5.7212,      -1.90131,   \
        8.18338,    5.0717,     8.97365,    7.22515,     5.06511,    8.63161,    8.46529,    4.17633,     9.02407,    \
        3.46426,    -7.88476,   -6.11441,   2.56508,     -7.60033,   -6.12081,   4.02105,    -7.11022,    -6.34405,   \
        -7.07262,   -2.59787,   -9.18882,   -6.65622,    -2.459,     -10.0614,   -6.66745,   -3.42841,    -8.84072,   \
        0.578358,   -6.03667,   0.0518778,  1.31218,     -5.47673,   -0.30204,   0.878736,   -6.19662,    0.953509,   \
        6.67178,    1.46411,    -4.80463,   7.55966,     1.1388,     -4.49416,   6.75125,    1.67172,     -5.74153,   \
        0.914441,   7.00586,    -2.91002,   1.42212,     7.03213,    -3.74172,   0.252237,   6.29537,     -2.8079,    \
        -7.17188,   -7.30583,   2.29638,    -6.45681,    -7.0091,    2.89599,    -7.67707,   -7.98885,    2.75194,    \
        -7.37602,   -4.0737,    5.34396,    -6.56876,    -3.67991,   5.68426,    -8.04941,   -3.4157,     5.63061,    \
        0.127036,   -2.95046,   7.55597,    0.896493,    -3.41433,   7.21072,    -0.145102,  -2.54204,    6.72372,    \
        8.44062,    -9.55318,   3.36265,    8.12134,     -9.61629,   2.4755,     8.19748,    -8.69815,    3.72318,    \
        4.70719,    -3.91228,   4.19727,    3.77196,     -3.55665,   4.28934,    5.33993,    -3.26935,    4.46541,    \
        -2.48864,   5.25298,    4.45734,    -2.17358,    5.64719,    5.32261,    -1.71766,   5.05253,     3.86126,    \
        7.37746,    -6.94308,   3.98465,    7.55555,     -6.54908,   3.11445,    6.40088,    -6.92717,    4.12419,    \
        8.14355,    0.600444,   -9.55292,   7.93865,     1.34045,    -10.1351,   7.5662,     0.689905,    -8.73891,   \
        7.78162,    6.10587,    -4.49126,   7.13568,     6.57626,    -3.91417,   7.31522,    5.45295,     -5.04449,   \
        8.79479,    8.90742,    -4.82647,   8.43568,     7.95573,    -4.80982,   8.07724,    9.48511,     -4.50826,   \
        4.22304,    2.8517,     -4.51653,   3.91178,     2.24608,    -5.24976,   5.07902,    2.47897,     -4.28461,   \
        2.62242,    0.247338,   -9.07983,   2.12754,     1.02016,    -9.34121,   2.11421,    -0.584392,   -9.21845,   \
        5.44578,    -3.09052,   -2.34253,   5.75166,     -2.21821,   -2.59853,   5.17608,    -3.04106,    -1.44969,   \
        -8.52047,   7.92415,    2.07346,    -8.27217,    7.02047,    1.91216,    -8.99997,   7.93679,     2.8913,     \
        0.910671,   -8.76453,   -4.42478,   0.976793,    -9.38039,   -5.15386,   0.523265,   -9.06337,    -3.58563,   \
        4.7222,     -6.61213,   4.88384,    3.90787,     -7.17592,   5.02047,    4.49639,    -5.69603,    4.6256,     \
        -3.20009,   -3.3818,    4.58622,    -3.02499,    -2.92351,   3.78785,    -3.49885,   -4.24976,    4.24388,    \
        -6.27225,   -4.58413,   -7.62065,   -6.57639,    -4.30815,   -6.73683,   -6.34775,   -5.56081,    -7.80516,   \
        -5.23334,   2.62577,    1.75611,    -6.18209,    2.72091,    1.88819,    -5.16975,   2.23405,     0.868013,   \
        -6.44789,   6.23436,    1.36662,    -7.02701,    5.59688,    0.93844,    -6.00104,   5.85908,     2.11004,    \
        -7.12166,   -8.55786,   7.28504,    -8.02563,    -8.56778,   6.9259,     -6.70847,   -9.04973,    6.54291,    \
        -7.27976,   0.121241,   3.43381,    -6.35008,    -0.109539,  3.47676,    -7.67781,   -0.609752,   2.88347,    \
        8.67751,    2.13303,    7.66463,    8.281,       1.65853,    6.97311,    9.45889,    2.44603,     7.20707,    \
        1.94556,    -8.39501,   -0.644423,  1.44042,     -7.55675,   -0.716913,  1.23527,    -8.98944,    -0.796306,  \
        -3.5363,    -0.80646,   -8.03399,   -2.6054,     -1.09768,   -8.06581,   -3.67341,   -0.635484,   -7.11268,   \
        7.66489,    7.16842,    -8.81241,   8.38757,     7.84297,    -8.73923,   8.06356,    6.42207,     -9.21897,   \
        8.25905,    -6.64189,   -2.87217,   8.38933,     -6.7806,    -3.83042,   7.89048,    -5.7521,     -2.77953,   \
        6.41035,    5.39083,    -7.32496,   5.53511,     5.67291,    -7.13166,   6.86383,    6.17427,     -7.67282,   \
        -9.11386,   4.89603,    2.85584,    -9.06665,    5.30607,    3.74722,    -10.0107,   5.01949,     2.59408,    \
        0.562238,   0.658456,   -4.35948,   1.0481,      0.896745,   -3.55029,   1.1222,     0.855483,    -5.05952,   \
        -3.48238,   4.59819,    -6.99336,   -3.84765,    5.20645,    -6.28201,   -2.90787,   4.06054,     -6.46115,   \
        -1.84158,   -0.718186,  6.95859,    -2.75992,    -0.335396,  6.911,      -1.8576,    -1.12149,    7.83886,    \
        2.392,      -4.12694,   6.70315,    2.14454,     -5.08132,   6.70843,    3.08183,    -3.98315,    7.36587,    \
        -0.687831,  8.35904,    -8.18001,   -0.709108,   8.28988,    -9.13092,   -0.780803,  9.30749,     -8.05267,   \
        2.58201,    -0.616484,  4.85109,    3.00844,     -1.01876,   5.61815,    3.177,      0.114553,    4.61769,    \
        -5.72262,   6.90303,    6.0142,     -6.24056,    6.9433,     6.7632,     -4.80593,   6.93643,     6.40664,    \
        3.65258,    3.64906,    5.45809,    3.50825,     4.58936,    5.67938,    3.81042,    3.23542,     6.33612,    \
        -9.75456,   8.17847,    7.27903,    -9.71468,    8.4321,     6.33642,    -9.88473,   7.22613,     7.24813,    \
        2.21818,    -2.42221,   2.56085,    1.65464,     -2.126,     3.29325,    1.80907,    -3.19323,    2.11555,    \
        1.37129,    4.69026,    -5.96017,   0.902013,    3.9889,     -5.57254,   0.752961,   5.4372,      -6.02961,   \
        6.08578,    5.17378,    5.1488,     6.67525,     4.49311,    4.80407,    6.7746,     5.85094,     5.35754,    \
        6.44544,    -2.98845,   -8.97025,   7.20926,     -2.53609,   -9.3956,    6.59853,    -3.09929,    -8.00718,   \
        5.70749,    7.99552,    -3.45804,   6.0922,      8.70102,    -2.97149,   4.85263,    7.76963,     -3.14176,   \
        4.7487,     -6.09479,   -3.32527,   5.00398,     -5.84101,   -2.40595,   4.24485,    -6.94646,    -3.29431,   \
        -9.20312,   -3.27296,   -1.48567,   -8.77822,    -4.16832,   -1.26155,   -10.0715,   -3.56128,    -1.84195,   \
        -8.30576,   4.9496,     -0.821233,  -7.84136,    4.17954,    -1.17254,   -9.05779,   4.60705,     -0.286739,  \
        6.80659,    -2.22062,   4.9775,     6.60499,     -1.37729,   5.31567,    6.82688,    -2.78249,    5.77586,    \
        -2.02323,   3.00161,    8.74714,    -1.59367,    2.71318,    7.86437,    -2.70973,   3.59311,     8.39858,    \
        -6.72275,   2.16661,    5.47524,    -6.42844,    1.966,      6.38222,    -7.23819,   1.43195,     5.08712,    \
        -1.39118,   7.89994,    3.3464,     -2.32234,    8.05294,    3.56547,    -1.34339,   7.65604,     2.42412,    \
        -5.53236,   -1.59483,   1.31876,    -5.18863,    -2.28567,   0.755356,   -4.79028,   -1.33241,    1.84946,    \
        -8.46201,   8.63867,    -3.09562,   -7.99769,    7.90665,    -3.55338,   -9.24937,   8.82516,     -3.62092,   \
        -0.386369,  1.56835,    6.75087,    -1.02526,    0.791983,   6.76048,    -0.0996268, 1.61299,     5.87987,    \
        3.3804,     3.77036,    -2.09566,   2.48871,     3.60406,    -1.82271,   3.49923,    3.41048,     -3.01833,   \
        9.04744,    -7.33275,   -0.451866,  9.33896,     -8.28436,   -0.376057,  8.76496,    -7.14955,    -1.35573,   \
        -2.76353,   -5.17506,   -2.83316,   -1.99901,    -5.69167,   -3.15731,   -3.51109,   -5.82105,    -2.83647,   \
        -1.13949,   0.990784,   -0.627776,  -0.527878,   0.880253,   -1.3299,    -0.693432,  1.32795,     0.161063,   \
        0.880498,   -6.7735,    -6.79906,   0.537287,    -7.40121,   -7.46764,   0.585979,   -7.14231,    -5.93347,   \
        -0.185226,  4.59838,    3.21103,    -0.0379474,  4.91564,    2.2889,     0.368685,   3.79831,     3.40813,    \
        2.98898,    7.62728,    -4.49,      3.44842,     6.9209,     -4.89398,   2.6993,     8.21636,     -5.22483,   \
        6.90733,    -3.70879,   7.38198,    7.41617,     -4.22898,   6.76732,    7.50345,    -3.20514,    7.94919,    \
        2.40807,    -3.80185,   -0.14204,   2.44784,     -3.90631,   -1.09485,   3.33424,    -3.49834,    0.0623875,  \
        6.41119,    1.19799,    -7.51538,   5.84174,     0.505232,   -7.674,     5.89079,    1.94615,     -7.89122,   \
        -1.93948,   7.49781,    -3.91214,   -1.77805,    7.01564,    -3.04963,   -2.78968,   7.11956,     -4.2714,    \
        -6.76886,   -4.34111,   2.47592,    -7.07005,    -4.36741,   3.39004,    -6.87418,   -3.3858,     2.26963,    \
        8.66051,    -3.6003,    3.04761,    9.06283,     -4.42345,   2.78922,    8.39407,    -3.67241,    3.97645,    \
        5.51216,    -0.872068,  -5.38289,   4.8322,      -0.843867,  -6.08603,   5.72375,    -0.00595523, -5.01628,   \
        -3.93154,   -2.12303,   -3.04346,   -3.60151,    -2.99238,   -3.14079,   -3.42051,   -1.58731,    -3.64,      \
        -1.86204,   -0.427936,  -4.47627,   -1.03394,    0.0528735,  -4.67052,   -2.64618,   -0.103972,   -4.92055,   \
        4.75561,    2.28034,    7.79715,    4.89586,     2.64891,    8.69794,    4.47376,    1.39956,     8.032,      \
        7.84967,    -3.93631,   -2.66969,   7.9891,      -3.55936,   -3.56829,   6.87886,    -3.83375,    -2.54404,   \
        -6.70719,   -6.10752,   -4.40186,   -5.98926,    -6.26242,   -5.08635,   -7.22738,   -6.94199,    -4.29185,   \
        7.31285,    -7.85114,   8.18167,    6.44757,     -7.48767,   7.8577,     7.31257,    -8.8217,     8.13236,    \
        -0.869957,  4.11965,    -8.52279,   -1.39688,    4.69875,    -7.96566,   -1.39904,   3.73671,     -9.2156,    \
        -7.39159,   5.76595,    -9.05152,   -7.85361,    6.25184,    -8.30213,   -6.48094,   5.70446,     -8.71208,   \
        -6.01846,   -7.25,      -8.22082,   -5.07961,    -7.32667,   -7.97138,   -6.48722,   -7.97761,    -7.79956,   \
        -7.41793,   -3.25641,   -5.05349,   -7.15281,    -3.98557,   -4.54009,   -8.39296,   -3.11139,    -4.84844,   \
        -5.06422,   2.25585,    -7.0697,    -4.73339,    2.24145,    -7.90526,   -5.06149,   3.21171,     -6.89375,   \
        5.90871,    -1.06781,   0.688121,   5.52276,     -1.92641,   0.787987,   6.69338,    -1.09253,    1.31828,    \
        -1.13191,   -7.10655,   8.32534,    -1.84394,    -6.50146,   8.34151,    -1.37811,   -7.81929,    7.71625,    \
        -9.66428,   -9.26793,   9.58724,    -9.5999,     -8.39792,   9.18181,    -9.2237,    -9.87107,    8.95039,    \
        8.45818,    -0.737146,  -3.70933,   7.91026,     -0.443389,  -2.91172,   9.23531,    -0.149639,   -3.62921,   \
        -8.69438,   -6.55985,   -9.44168,   -9.23769,    -6.25975,   -8.69673,   -7.88145,   -7.02038,    -9.20812,   \
        1.53544,    -2.18482,   -9.6681,    1.06585,     -2.69256,   -8.95624,   0.9333,     -2.19844,    -10.493,    \
        6.95846,    -9.20659,   -3.21589,   7.068,       -8.42696,   -2.64153,   6.11541,    -8.98934,    -3.67721,   \
        -6.7198,    -1.09965,   -6.49331,   -6.387,      -1.54661,   -7.31379,   -7.04586,   -1.80484,    -5.93836,   \
        3.03978,    6.10263,    9.44872,    2.50534,     5.53036,    10.0745,    3.56164,    6.75522,     9.88875,    \
        -9.11928,   -5.13864,   7.78307,    -9.03928,    -5.5129,    8.68772,    -8.26766,   -4.86079,    7.44674,    \
        0.737453,   5.67455,    0.668601,   1.15229,     6.54852,    0.96944,    0.404532,   5.85618,     -0.208841,  \
        4.47666,    1.41897,    4.21804,    4.366,       2.36244,    4.45647,    4.52335,    1.34968,     3.22864,    \
        -0.536099,  -6.14001,   -4.17382,   -0.00237429, -6.90241,   -3.95006,   -0.11215,   -5.3777,     -3.74944,   \
        0.0681075,  1.71673,    -7.15818,   0.175449,    2.57809,    -7.52678,   -0.17608,   1.05121,     -7.85341,   \
        -1.7126,    -9.25523,   7.03185,    -2.71263,    -9.05848,   6.95876,    -1.35028,   -9.32481,    6.11655,    \
        1.22668,    2.47637,    4.3831,     1.38981,     1.93086,    3.6169,     2.10625,    2.92038,     4.5568,     \
        -8.63245,   -0.698236,  -0.97893,   -7.75132,    -0.794589,  -1.39482,   -9.07708,   -1.54555,    -1.27389,   \
        5.22742,    8.67497,    3.29991,    5.29671,     7.94151,    2.7031,     4.28327,    8.60382,     3.6128,     \
        -7.90301,   -6.16997,   -0.294335,  -7.70687,    -6.45037,   0.56613,    -8.82226,   -6.41252,    -0.40292,   \
        -1.34282,   6.16952,    -1.43683,   -1.6442,     6.39614,    -0.529057,  -1.6614,    5.29503,     -1.71698,   \
        -6.5174,    -6.67407,   4.86774,    -6.81848,    -5.82068,   5.19792,    -5.79733,   -6.88531,    5.43657,    \
        2.77147,    -8.53068,   5.62906,    2.0049,      -7.97799,   5.75615,    2.65753,    -9.15133,    4.95606,    \
        -0.149328,  -2.25283,   -0.343284,  -0.621543,   -1.50911,   -0.617103,  0.725262,   -1.841,      -0.276686,  \
        -3.65957,   2.93021,    4.75389,    -3.20218,    3.77235,    4.70404,    -4.54717,   3.29595,     4.69493,    \
        -3.95033,   8.14807,    -9.57379,   -4.55105,    8.8555,     -9.82038,   -3.05983,   8.32205,     -10.0049,   \
        5.42337,    5.86186,    2.30409,    5.30034,     5.65199,    3.2322,     6.36656,    5.59861,     2.15465,    \
        6.46076,    7.7931,     5.65774,    6.67736,     8.37235,    6.34021,    5.98086,    8.18781,     4.88387,    \
        -9.77568,   -7.73821,   6.94553,    -10.711,     -7.80278,   7.28896,    -9.56328,   -6.80556,    7.0826,     \
        -6.51737,   -1.1293,    7.84482,    -7.3486,     -1.53717,   7.53636,    -6.86473,   -0.227259,   8.16948,    \
        -6.1362,    9.54205,    -1.79044,   -6.29296,    9.64602,    -0.816846,  -6.93206,   9.14513,     -2.12088,   \
        -0.0210997, -1.73682,   4.68288,    -0.82292,    -1.40233,   4.20173,    0.585283,   -0.991169,   4.8269,     \
        -8.97918,   -1.69219,   1.95248,    -8.40778,    -1.92886,   1.13231,    -9.47473,   -2.49736,    2.19861,    \
        -0.239239,  -7.81858,   2.37434,    -0.898472,   -7.34816,   2.84664,    0.251329,   -8.40928,    3.04741,    \
        -4.73539,   5.6741,     -9.03954,   -4.12578,    5.47054,    -8.32089,   -4.47498,   6.63219,     -9.30145,   \
        2.74661,    8.34222,    4.21349,    3.04457,     7.65783,    4.88151,    2.30836,    7.92744,     3.46543,    \
        -5.06385,   -3.08696,   6.48756,    -5.03148,    -2.25574,   6.99757,    -4.39208,   -2.97464,    5.77261,    \
        5.75224,    4.91332,    7.81055,    4.94407,     5.23564,    8.23657,    5.61872,    4.7816,      6.82294,    \
        9.14358,    5.65534,    6.2858,     8.57694,     5.3752,     6.99721,    9.97535,    5.14433,     6.37545,    \
        9.55571,    9.12794,    -7.69067,   9.33878,     9.8846,     -8.30986,   8.99613,    9.30251,     -6.89699,   \
        -1.8595,    -2.22505,   9.51883,    -1.3699,     -2.8935,    9.00232,    -2.46581,   -2.79009,    10.0722,    \
        3.02338,    1.25124,    -6.36236,   2.11805,     1.45038,    -6.70861,   3.34646,    0.970965,    -7.18646,   \
        2.83445,    -9.28093,   -9.32549,   2.93411,     -9.6195,    -10.2316,   3.14121,    -8.36494,    -9.2779,    \
        -1.66152,   0.935333,   3.5345,     -2.00789,    1.73938,    3.92787,    -2.4388,    0.450977,    3.30162,    \
        -5.1435,    5.61822,    3.48891,    -4.20275,    5.44907,    3.68607,    -5.44792,   5.98983,     4.34744,    \
        -6.30037,   -0.766434,  -2.90619,   -6.60309,    -1.46377,   -3.46261,   -5.61953,   -1.15939,    -2.40473,   \
        -4.43381,   8.11297,    2.44563,    -5.15462,    7.43719,    2.47904,    -4.8186,    8.80399,     1.91576,    \
        2.78523,    -1.18725,   -3.48108,   3.69315,     -0.988851,  -3.85583,   2.18021,    -1.09885,    -4.22344,   \
        0.0188978,  -8.45944,   -8.82034,   0.936173,    -8.6858,    -8.99471,   -0.302944,  -8.27397,    -9.70186,   \
        -8.95121,   6.74983,    -7.09515,   -9.4948,     7.4222,     -7.49165,   -9.5084,    6.15693,     -6.56195,   \
        -9.38186,   9.4293,     -0.261821,  -9.24972,    8.9215,     -1.04057,   -9.09137,   8.84707,     0.459747,   \
        -5.04412,   7.21411,    -0.702949,  -5.511,      6.89261,    0.0813801,  -5.52271,   7.93975,     -1.14492,   \
        9.45748,    2.01955,    -4.9791,    9.52384,     1.57059,    -5.77846,   9.86295,    2.87769,     -5.09977,   \
        -5.14999,   0.874793,   -0.361073,  -5.18921,    -0.0469662, -0.0469248, -4.69942,   0.924823,    -1.22307,   \
        8.21745,    2.29839,    -2.12213,   8.69403,     2.22724,    -2.94048,   8.77007,    2.07691,     -1.36249,   \
        0.321075,   9.73555,    4.54254,    0.950215,    9.01001,    4.78941,    -0.422518,  9.30891,     4.16124,    \
        -2.52634,   -2.41534,   1.88741,    -1.74584,    -2.1156,    1.46442,    -2.39011,   -3.33943,    1.90157,    \
        9.15782,    -5.32864,   -7.56539,   9.13418,     -5.57835,   -6.6623,    8.27155,    -5.29893,    -7.8874,    \
        4.16475,    -3.15292,   8.8559,     4.83484,     -3.15562,   9.51251,    3.30267,    -2.99285,    9.27731,    \
        4.62947,    -1.15428,   -7.84499,   5.35277,     -1.6511,    -8.22307,   4.09398,    -0.741981,   -8.53469,   \
        7.94638,    -0.666901,  2.91024,    8.89359,     -0.809174,  2.63855,    7.70868,    -1.5571,     3.27769,    \
        -1.3502,    3.27483,    -5.30119,   -0.709402,   2.57087,    -5.56736,   -1.27696,   3.41227,     -4.36487,   \
        8.08009,    5.04226,    1.40103,    7.9533,      5.50236,    0.528342,   7.71145,    4.15053,     1.28771,    \
        -0.0173459, 9.57761,    -2.28726,   -0.935349,   9.44418,    -2.00224,   0.310702,   8.71309,     -2.56703,   \
        9.29494,    -7.02889,   -5.35334,   9.93921,     -7.66151,   -4.9752,    8.68911,    -7.52295,    -5.90932,   \
        -8.05292,   2.48576,    1.83727,    -7.98352,    1.70147,    2.42523,    -8.45221,   3.17965,     2.34215,    \
        -0.732766,  -2.73196,   -4.92148,   -1.2784,     -1.87616,   -4.88931,   -0.799871,  -3.13478,    -4.05418,   \
        -9.53586,   -5.69679,   2.19236,    -9.7746,     -6.01725,   1.28842,    -8.62457,   -5.89872,    2.34066,    \
        -9.31014,   5.12133,    -3.55878,   -10.2826,    5.17922,    -3.68265,   -9.20798,   5.10751,     -2.61733,   \
        -9.59446,   2.52399,    -8.65776,   -8.90379,    2.09471,    -8.05377,   -10.1306,   1.7303,      -8.78625,   \
        8.78991,    -2.08571,   8.99876,    8.60075,     -1.16871,   9.1955,     9.22997,    -2.41078,    9.90645,    \
        -0.249593,  -4.89313,   4.52181,    -0.0927041,  -3.95477,   4.57343,    -1.16647,   -4.93966,    4.31559,    \
        -3.60081,   -5.30656,   8.38939,    -4.40014,    -5.05429,   7.88933,    -3.56835,   -4.94744,    9.2867,     \
        -3.31909,   6.50909,    6.87329,    -2.70062,    7.07354,    7.32022,    -3.47612,   5.90184,     7.58547,    \
        9.59553,    -2.84978,   -4.96192,   9.19934,     -2.03963,   -4.66793,   9.1902,     -2.91362,    -5.83082,   \
        -0.461594,  9.34059,    0.688926,   -0.652121,   10.157,     1.15461,    -1.14248,   9.36331,     0.00503864, \
        -4.07842,   1.42692,    -2.72522,   -3.5962,     2.24395,    -2.85387,   -4.73198,   1.3193,      -3.39277,   \
        5.03747,    3.56762,    -9.06884,   4.12469,     3.85288,    -8.93223,   5.68171,    4.22059,     -8.82649,   \
        -4.3148,    0.143222,   3.2026,     -4.4369,     0.990834,   2.81235,    -4.41727,   0.253898,    4.1973,     \
        -8.60725,   -8.87173,   4.30969,    -9.41744,    -9.10237,   3.84381,    -8.83781,   -8.10231,    4.86066,    \
        -6.50004,   1.58573,    8.36394,    -6.9866,     2.37512,    8.60333,    -5.66041,   1.68646,     8.76522,    \
        6.84637,    -3.28639,   -6.02315,   6.60432,     -4.23801,   -5.89501,   6.22053,    -2.79935,    -5.45761,   \
        -3.317,     -7.38656,   -7.75752,   -2.84888,    -7.84338,   -8.48714,   -2.96515,   -7.57373,    -6.86468,   \
        2.08925,    2.98616,    0.16921,    1.52162,     2.56311,    0.80089,    2.04206,    3.93499,     0.387657,   \
        5.49218,    -9.67082,   -5.85574,   5.50341,     -10.54,     -5.47925,   4.5583,     -9.40299,    -6.18052,   \
        6.2324,     -9.28007,   1.47808,    5.93027,     -9.82658,   2.25042,    5.88503,    -9.77537,    0.756384,   \
        1.43446,    2.36436,    8.6485,     2.33463,     2.53518,    8.38303,    0.959207,   2.12084,     7.81443,    \
        4.01453,    5.30356,    -5.63297,   3.10082,     5.05339,    -5.82087,   4.20683,    4.5452,      -5.07653,   \
        -2.5308,    -6.50146,   3.58014,    -2.9966,     -6.15046,   2.75659,    -3.19759,   -7.11516,    3.89734,    \
        1.45686,    -1.7804,    -6.33239,   0.98871,     -2.3268,    -5.7255,    2.27788,    -2.24183,    -6.55899,   \
        -7.46692,   -9.18105,   -6.72596,   -6.80713,    -9.88052,   -6.72097,   -8.23477,   -9.61649,    -7.01519,   \
        -5.25053,   8.47004,    -6.64173,   -4.77487,    9.00637,    -7.27021,   -4.82567,   8.72813,     -5.7858,    \
        5.20389,    -5.75827,   -0.375343,  4.51134,     -6.1592,    0.160019,   5.99561,    -6.10293,    0.0557868,  \
        3.83049,    7.02645,    6.41059,    4.78045,     7.09653,    6.2526,     3.53062,    7.777,       6.92908,    \
        2.85171,    -4.07552,   -3.72567,   3.74055,     -4.52724,   -3.61532,   2.98443,    -3.13305,    -3.65827,   \
        0.00562273, 6.98306,    -6.06711,   -0.501146,   7.13692,    -5.24102,   -0.611655,  7.25193,     -6.80756,   \
        -4.28539,   0.232026,   5.991,      -5.12544,    0.0471125,  6.41968,    -4.17796,   1.17156,     5.83537,    \
        2.27355,    -0.567829,  0.705378,   3.10563,     -0.0916092, 0.936346,   2.42254,    -1.31674,    1.30434,    \
        -1.97932,   -5.53862,   -0.0474719, -2.08823,    -5.2145,    -0.915728,  -1.0061,    -5.64591,    0.0644596,  \
        2.15985,    -5.55981,   2.8426,     1.87041,     -5.66572,   3.75793,    3.04225,    -6.07044,    2.75485,    \
        -2.76045,   9.10922,    -1.11865,   -3.24713,    8.31356,    -1.17237,   -3.30879,   9.88155,     -0.857509,  \
        -4.35886,   5.90379,    -4.7153,    -5.21737,    6.29799,    -4.8033,    -4.37123,   5.49415,     -3.85287,   \
        -7.62108,   1.31046,    -6.98994,   -7.4467,     0.406472,   -6.80163,   -6.71322,   1.70871,     -7.12655};

#endif
