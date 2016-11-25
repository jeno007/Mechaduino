//Contains the declaration of the state variables for the control loop
#include <Wire.h>
#include "State.h"
#include "Parameters.h"


//---- interrupt vars ----
volatile int r = 0;                 //target angle
volatile long y = 0;                //current angle
volatile long y_1 = 0;                //last angle

volatile int raw_0 = 0;             // current measured angle
volatile int raw_1 = 0;             // last measured angle
volatile int raw_diff = 0;          // diff of both
volatile int u = 0;                 // control effort

volatile long ITerm = 0;            // Integral term

volatile int e_0 = 0;               // current error term
volatile int e_1 = 0;               // last error term

volatile bool dir = true;           // flag for dir setting
volatile bool enabled = true;       // flag for enabled setting

bool calibration_running = false;   // flag for calibration
bool frequency_test = false;        // flag for frequency test


//----current settings-----
const float rSense = 0.150;                           // resistor value
const int uMAX = ((1024 * iMAX * 10 * rSense) / 3.3);  // max voltage for the vref pins


//---- Step settings -----
const int counts_per_revolution = 16384;                              // encoder counts for 360 degrees
const float angle_per_step = 360.0 / steps_per_revolution;            // only needed for calibration routine by now
const int stepangle = 36000 / (steps_per_revolution*microstepping);   // angle of one step as int
volatile const int PA = (36000 / steps_per_revolution);               // angle of one fullstep
volatile int step_target = 0;                                         // target as step gets incremented if an step is received


//--- lookup tables for the coils ---
const PROGMEM int_fast16_t sin_lookup[] = {725, 727, 728, 729, 730, 732, 733, 734, 735, 737, 738, 739, 740, 742, 743, 744, 745, 746, 748, 749, 750, 751, 753, 754, 755, 756, 757, 759, 760, 761, 762, 763, 765, 766, 767, 768, 769, 770, 772, 773, 774, 775, 776, 777, 779, 780, 781, 782, 783, 784, 786, 787, 788, 789, 790, 791, 792, 794, 795, 796, 797, 798, 799, 800, 801, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 813, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 889, 890, 891, 892, 893, 894, 895, 896, 896, 897, 898, 899, 900, 901, 902, 902, 903, 904, 905, 906, 907, 907, 908, 909, 910, 911, 912, 912, 913, 914, 915, 916, 916, 917, 918, 919, 920, 920, 921, 922, 923, 923, 924, 925, 926, 927, 927, 928, 929, 930, 930, 931, 932, 933, 933, 934, 935, 935, 936, 937, 938, 938, 939, 940, 940, 941, 942, 943, 943, 944, 945, 945, 946, 947, 947, 948, 949, 949, 950, 951, 951, 952, 953, 953, 954, 955, 955, 956, 957, 957, 958, 959, 959, 960, 960, 961, 962, 962, 963, 963, 964, 965, 965, 966, 966, 967, 968, 968, 969, 969, 970, 971, 971, 972, 972, 973, 973, 974, 974, 975, 976, 976, 977, 977, 978, 978, 979, 979, 980, 980, 981, 981, 982, 982, 983, 983, 984, 984, 985, 985, 986, 986, 987, 987, 988, 988, 989, 989, 990, 990, 990, 991, 991, 992, 992, 993, 993, 994, 994, 994, 995, 995, 996, 996, 997, 997, 997, 998, 998, 999, 999, 999, 1000, 1000, 1000, 1001, 1001, 1002, 1002, 1002, 1003, 1003, 1003, 1004, 1004, 1004, 1005, 1005, 1006, 1006, 1006, 1007, 1007, 1007, 1007, 1008, 1008, 1008, 1009, 1009, 1009, 1010, 1010, 1010, 1011, 1011, 1011, 1011, 1012, 1012, 1012, 1012, 1013, 1013, 1013, 1014, 1014, 1014, 1014, 1015, 1015, 1015, 1015, 1015, 1016, 1016, 1016, 1016, 1017, 1017, 1017, 1017, 1017, 1018, 1018, 1018, 1018, 1018, 1019, 1019, 1019, 1019, 1019, 1019, 1020, 1020, 1020, 1020, 1020, 1020, 1021, 1021, 1021, 1021, 1021, 1021, 1021, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1021, 1021, 1021, 1021, 1021, 1021, 1020, 1020, 1020, 1020, 1020, 1020, 1019, 1019, 1019, 1019, 1019, 1019, 1018, 1018, 1018, 1018, 1018, 1017, 1017, 1017, 1017, 1017, 1016, 1016, 1016, 1016, 1015, 1015, 1015, 1015, 1015, 1014, 1014, 1014, 1014, 1013, 1013, 1013, 1012, 1012, 1012, 1012, 1011, 1011, 1011, 1011, 1010, 1010, 1010, 1009, 1009, 1009, 1008, 1008, 1008, 1007, 1007, 1007, 1007, 1006, 1006, 1006, 1005, 1005, 1004, 1004, 1004, 1003, 1003, 1003, 1002, 1002, 1002, 1001, 1001, 1000, 1000, 1000, 999, 999, 999, 998, 998, 997, 997, 997, 996, 996, 995, 995, 994, 994, 994, 993, 993, 992, 992, 991, 991, 990, 990, 990, 989, 989, 988, 988, 987, 987, 986, 986, 985, 985, 984, 984, 983, 983, 982, 982, 981, 981, 980, 980, 979, 979, 978, 978, 977, 977, 976, 976, 975, 974, 974, 973, 973, 972, 972, 971, 971, 970, 969, 969, 968, 968, 967, 966, 966, 965, 965, 964, 963, 963, 962, 962, 961, 960, 960, 959, 959, 958, 957, 957, 956, 955, 955, 954, 953, 953, 952, 951, 951, 950, 949, 949, 948, 947, 947, 946, 945, 945, 944, 943, 943, 942, 941, 940, 940, 939, 938, 938, 937, 936, 935, 935, 934, 933, 933, 932, 931, 930, 930, 929, 928, 927, 927, 926, 925, 924, 923, 923, 922, 921, 920, 920, 919, 918, 917, 916, 916, 915, 914, 913, 912, 912, 911, 910, 909, 908, 907, 907, 906, 905, 904, 903, 902, 902, 901, 900, 899, 898, 897, 896, 896, 895, 894, 893, 892, 891, 890, 889, 889, 888, 887, 886, 885, 884, 883, 882, 881, 880, 880, 879, 878, 877, 876, 875, 874, 873, 872, 871, 870, 869, 868, 867, 867, 866, 865, 864, 863, 862, 861, 860, 859, 858, 857, 856, 855, 854, 853, 852, 851, 850, 849, 848, 847, 846, 845, 844, 843, 842, 841, 840, 839, 838, 837, 836, 835, 834, 833, 832, 831, 829, 828, 827, 826, 825, 824, 823, 822, 821, 820, 819, 818, 817, 816, 815, 813, 812, 811, 810, 809, 808, 807, 806, 805, 804, 803, 801, 800, 799, 798, 797, 796, 795, 794, 792, 791, 790, 789, 788, 787, 786, 784, 783, 782, 781, 780, 779, 777, 776, 775, 774, 773, 772, 770, 769, 768, 767, 766, 765, 763, 762, 761, 760, 759, 757, 756, 755, 754, 753, 751, 750, 749, 748, 746, 745, 744, 743, 742, 740, 739, 738, 737, 735, 734, 733, 732, 730, 729, 728, 727, 725, 724, 723, 722, 720, 719, 718, 716, 715, 714, 713, 711, 710, 709, 707, 706, 705, 704, 702, 701, 700, 698, 697, 696, 694, 693, 692, 690, 689, 688, 687, 685, 684, 683, 681, 680, 679, 677, 676, 674, 673, 672, 670, 669, 668, 666, 665, 664, 662, 661, 660, 658, 657, 655, 654, 653, 651, 650, 649, 647, 646, 644, 643, 642, 640, 639, 637, 636, 635, 633, 632, 630, 629, 628, 626, 625, 623, 622, 621, 619, 618, 616, 615, 613, 612, 611, 609, 608, 606, 605, 603, 602, 600, 599, 598, 596, 595, 593, 592, 590, 589, 587, 586, 584, 583, 581, 580, 579, 577, 576, 574, 573, 571, 570, 568, 567, 565, 564, 562, 561, 559, 558, 556, 555, 553, 552, 550, 549, 547, 546, 544, 543, 541, 540, 538, 537, 535, 534, 532, 530, 529, 527, 526, 524, 523, 521, 520, 518, 517, 515, 514, 512, 510, 509, 507, 506, 504, 503, 501, 500, 498, 496, 495, 493, 492, 490, 489, 487, 485, 484, 482, 481, 479, 478, 476, 474, 473, 471, 470, 468, 466, 465, 463, 462, 460, 459, 457, 455, 454, 452, 450, 449, 447, 446, 444, 442, 441, 439, 438, 436, 434, 433, 431, 430, 428, 426, 425, 423, 421, 420, 418, 416, 415, 413, 412, 410, 408, 407, 405, 403, 402, 400, 398, 397, 395, 394, 392, 390, 389, 387, 385, 384, 382, 380, 379, 377, 375, 374, 372, 370, 369, 367, 365, 364, 362, 360, 359, 357, 355, 354, 352, 350, 349, 347, 345, 344, 342, 340, 338, 337, 335, 333, 332, 330, 328, 327, 325, 323, 322, 320, 318, 316, 315, 313, 311, 310, 308, 306, 305, 303, 301, 299, 298, 296, 294, 293, 291, 289, 287, 286, 284, 282, 281, 279, 277, 275, 274, 272, 270, 268, 267, 265, 263, 262, 260, 258, 256, 255, 253, 251, 249, 248, 246, 244, 243, 241, 239, 237, 236, 234, 232, 230, 229, 227, 225, 223, 222, 220, 218, 216, 215, 213, 211, 209, 208, 206, 204, 202, 201, 199, 197, 195, 194, 192, 190, 188, 187, 185, 183, 181, 180, 178, 176, 174, 173, 171, 169, 167, 165, 164, 162, 160, 158, 157, 155, 153, 151, 150, 148, 146, 144, 143, 141, 139, 137, 135, 134, 132, 130, 128, 127, 125, 123, 121, 119, 118, 116, 114, 112, 111, 109, 107, 105, 103, 102, 100, 98, 96, 95, 93, 91, 89, 87, 86, 84, 82, 80, 79, 77, 75, 73, 71, 70, 68, 66, 64, 63, 61, 59, 57, 55, 54, 52, 50, 48, 46, 45, 43, 41, 39, 38, 36, 34, 32, 30, 29, 27, 25, 23, 21, 20, 18, 16, 14, 13, 11, 9, 7, 5, 4, 2, 0, -2, -4, -5, -7, -9, -11, -13, -14, -16, -18, -20, -21, -23, -25, -27, -29, -30, -32, -34, -36, -38, -39, -41, -43, -45, -46, -48, -50, -52, -54, -55, -57, -59, -61, -63, -64, -66, -68, -70, -71, -73, -75, -77, -79, -80, -82, -84, -86, -87, -89, -91, -93, -95, -96, -98, -100, -102, -103, -105, -107, -109, -111, -112, -114, -116, -118, -119, -121, -123, -125, -127, -128, -130, -132, -134, -135, -137, -139, -141, -143, -144, -146, -148, -150, -151, -153, -155, -157, -158, -160, -162, -164, -165, -167, -169, -171, -173, -174, -176, -178, -180, -181, -183, -185, -187, -188, -190, -192, -194, -195, -197, -199, -201, -202, -204, -206, -208, -209, -211, -213, -215, -216, -218, -220, -222, -223, -225, -227, -229, -230, -232, -234, -236, -237, -239, -241, -243, -244, -246, -248, -249, -251, -253, -255, -256, -258, -260, -262, -263, -265, -267, -268, -270, -272, -274, -275, -277, -279, -281, -282, -284, -286, -287, -289, -291, -293, -294, -296, -298, -299, -301, -303, -305, -306, -308, -310, -311, -313, -315, -316, -318, -320, -322, -323, -325, -327, -328, -330, -332, -333, -335, -337, -338, -340, -342, -344, -345, -347, -349, -350, -352, -354, -355, -357, -359, -360, -362, -364, -365, -367, -369, -370, -372, -374, -375, -377, -379, -380, -382, -384, -385, -387, -389, -390, -392, -394, -395, -397, -398, -400, -402, -403, -405, -407, -408, -410, -412, -413, -415, -416, -418, -420, -421, -423, -425, -426, -428, -430, -431, -433, -434, -436, -438, -439, -441, -442, -444, -446, -447, -449, -450, -452, -454, -455, -457, -459, -460, -462, -463, -465, -466, -468, -470, -471, -473, -474, -476, -478, -479, -481, -482, -484, -485, -487, -489, -490, -492, -493, -495, -496, -498, -500, -501, -503, -504, -506, -507, -509, -510, -512, -514, -515, -517, -518, -520, -521, -523, -524, -526, -527, -529, -530, -532, -534, -535, -537, -538, -540, -541, -543, -544, -546, -547, -549, -550, -552, -553, -555, -556, -558, -559, -561, -562, -564, -565, -567, -568, -570, -571, -573, -574, -576, -577, -579, -580, -581, -583, -584, -586, -587, -589, -590, -592, -593, -595, -596, -598, -599, -600, -602, -603, -605, -606, -608, -609, -611, -612, -613, -615, -616, -618, -619, -621, -622, -623, -625, -626, -628, -629, -630, -632, -633, -635, -636, -637, -639, -640, -642, -643, -644, -646, -647, -649, -650, -651, -653, -654, -655, -657, -658, -660, -661, -662, -664, -665, -666, -668, -669, -670, -672, -673, -674, -676, -677, -679, -680, -681, -683, -684, -685, -687, -688, -689, -690, -692, -693, -694, -696, -697, -698, -700, -701, -702, -704, -705, -706, -707, -709, -710, -711, -713, -714, -715, -716, -718, -719, -720, -722, -723, -724, -725, -727, -728, -729, -730, -732, -733, -734, -735, -737, -738, -739, -740, -742, -743, -744, -745, -746, -748, -749, -750, -751, -753, -754, -755, -756, -757, -759, -760, -761, -762, -763, -765, -766, -767, -768, -769, -770, -772, -773, -774, -775, -776, -777, -779, -780, -781, -782, -783, -784, -786, -787, -788, -789, -790, -791, -792, -794, -795, -796, -797, -798, -799, -800, -801, -803, -804, -805, -806, -807, -808, -809, -810, -811, -812, -813, -815, -816, -817, -818, -819, -820, -821, -822, -823, -824, -825, -826, -827, -828, -829, -831, -832, -833, -834, -835, -836, -837, -838, -839, -840, -841, -842, -843, -844, -845, -846, -847, -848, -849, -850, -851, -852, -853, -854, -855, -856, -857, -858, -859, -860, -861, -862, -863, -864, -865, -866, -867, -867, -868, -869, -870, -871, -872, -873, -874, -875, -876, -877, -878, -879, -880, -880, -881, -882, -883, -884, -885, -886, -887, -888, -889, -889, -890, -891, -892, -893, -894, -895, -896, -896, -897, -898, -899, -900, -901, -902, -902, -903, -904, -905, -906, -907, -907, -908, -909, -910, -911, -912, -912, -913, -914, -915, -916, -916, -917, -918, -919, -920, -920, -921, -922, -923, -923, -924, -925, -926, -927, -927, -928, -929, -930, -930, -931, -932, -933, -933, -934, -935, -935, -936, -937, -938, -938, -939, -940, -940, -941, -942, -943, -943, -944, -945, -945, -946, -947, -947, -948, -949, -949, -950, -951, -951, -952, -953, -953, -954, -955, -955, -956, -957, -957, -958, -959, -959, -960, -960, -961, -962, -962, -963, -963, -964, -965, -965, -966, -966, -967, -968, -968, -969, -969, -970, -971, -971, -972, -972, -973, -973, -974, -974, -975, -976, -976, -977, -977, -978, -978, -979, -979, -980, -980, -981, -981, -982, -982, -983, -983, -984, -984, -985, -985, -986, -986, -987, -987, -988, -988, -989, -989, -990, -990, -990, -991, -991, -992, -992, -993, -993, -994, -994, -994, -995, -995, -996, -996, -997, -997, -997, -998, -998, -999, -999, -999, -1000, -1000, -1000, -1001, -1001, -1002, -1002, -1002, -1003, -1003, -1003, -1004, -1004, -1004, -1005, -1005, -1006, -1006, -1006, -1007, -1007, -1007, -1007, -1008, -1008, -1008, -1009, -1009, -1009, -1010, -1010, -1010, -1011, -1011, -1011, -1011, -1012, -1012, -1012, -1012, -1013, -1013, -1013, -1014, -1014, -1014, -1014, -1015, -1015, -1015, -1015, -1015, -1016, -1016, -1016, -1016, -1017, -1017, -1017, -1017, -1017, -1018, -1018, -1018, -1018, -1018, -1019, -1019, -1019, -1019, -1019, -1019, -1020, -1020, -1020, -1020, -1020, -1020, -1021, -1021, -1021, -1021, -1021, -1021, -1021, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1024, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1023, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1022, -1021, -1021, -1021, -1021, -1021, -1021, -1021, -1020, -1020, -1020, -1020, -1020, -1020, -1019, -1019, -1019, -1019, -1019, -1019, -1018, -1018, -1018, -1018, -1018, -1017, -1017, -1017, -1017, -1017, -1016, -1016, -1016, -1016, -1015, -1015, -1015, -1015, -1015, -1014, -1014, -1014, -1014, -1013, -1013, -1013, -1012, -1012, -1012, -1012, -1011, -1011, -1011, -1011, -1010, -1010, -1010, -1009, -1009, -1009, -1008, -1008, -1008, -1007, -1007, -1007, -1007, -1006, -1006, -1006, -1005, -1005, -1004, -1004, -1004, -1003, -1003, -1003, -1002, -1002, -1002, -1001, -1001, -1000, -1000, -1000, -999, -999, -999, -998, -998, -997, -997, -997, -996, -996, -995, -995, -994, -994, -994, -993, -993, -992, -992, -991, -991, -990, -990, -990, -989, -989, -988, -988, -987, -987, -986, -986, -985, -985, -984, -984, -983, -983, -982, -982, -981, -981, -980, -980, -979, -979, -978, -978, -977, -977, -976, -976, -975, -974, -974, -973, -973, -972, -972, -971, -971, -970, -969, -969, -968, -968, -967, -966, -966, -965, -965, -964, -963, -963, -962, -962, -961, -960, -960, -959, -959, -958, -957, -957, -956, -955, -955, -954, -953, -953, -952, -951, -951, -950, -949, -949, -948, -947, -947, -946, -945, -945, -944, -943, -943, -942, -941, -940, -940, -939, -938, -938, -937, -936, -935, -935, -934, -933, -933, -932, -931, -930, -930, -929, -928, -927, -927, -926, -925, -924, -923, -923, -922, -921, -920, -920, -919, -918, -917, -916, -916, -915, -914, -913, -912, -912, -911, -910, -909, -908, -907, -907, -906, -905, -904, -903, -902, -902, -901, -900, -899, -898, -897, -896, -896, -895, -894, -893, -892, -891, -890, -889, -889, -888, -887, -886, -885, -884, -883, -882, -881, -880, -880, -879, -878, -877, -876, -875, -874, -873, -872, -871, -870, -869, -868, -867, -867, -866, -865, -864, -863, -862, -861, -860, -859, -858, -857, -856, -855, -854, -853, -852, -851, -850, -849, -848, -847, -846, -845, -844, -843, -842, -841, -840, -839, -838, -837, -836, -835, -834, -833, -832, -831, -829, -828, -827, -826, -825, -824, -823, -822, -821, -820, -819, -818, -817, -816, -815, -813, -812, -811, -810, -809, -808, -807, -806, -805, -804, -803, -801, -800, -799, -798, -797, -796, -795, -794, -792, -791, -790, -789, -788, -787, -786, -784, -783, -782, -781, -780, -779, -777, -776, -775, -774, -773, -772, -770, -769, -768, -767, -766, -765, -763, -762, -761, -760, -759, -757, -756, -755, -754, -753, -751, -750, -749, -748, -746, -745, -744, -743, -742, -740, -739, -738, -737, -735, -734, -733, -732, -730, -729, -728, -727, -725, -724, -723, -722, -720, -719, -718, -716, -715, -714, -713, -711, -710, -709, -707, -706, -705, -704, -702, -701, -700, -698, -697, -696, -694, -693, -692, -690, -689, -688, -687, -685, -684, -683, -681, -680, -679, -677, -676, -674, -673, -672, -670, -669, -668, -666, -665, -664, -662, -661, -660, -658, -657, -655, -654, -653, -651, -650, -649, -647, -646, -644, -643, -642, -640, -639, -637, -636, -635, -633, -632, -630, -629, -628, -626, -625, -623, -622, -621, -619, -618, -616, -615, -613, -612, -611, -609, -608, -606, -605, -603, -602, -600, -599, -598, -596, -595, -593, -592, -590, -589, -587, -586, -584, -583, -581, -580, -579, -577, -576, -574, -573, -571, -570, -568, -567, -565, -564, -562, -561, -559, -558, -556, -555, -553, -552, -550, -549, -547, -546, -544, -543, -541, -540, -538, -537, -535, -534, -532, -530, -529, -527, -526, -524, -523, -521, -520, -518, -517, -515, -514, -512, -510, -509, -507, -506, -504, -503, -501, -500, -498, -496, -495, -493, -492, -490, -489, -487, -485, -484, -482, -481, -479, -478, -476, -474, -473, -471, -470, -468, -466, -465, -463, -462, -460, -459, -457, -455, -454, -452, -450, -449, -447, -446, -444, -442, -441, -439, -438, -436, -434, -433, -431, -430, -428, -426, -425, -423, -421, -420, -418, -416, -415, -413, -412, -410, -408, -407, -405, -403, -402, -400, -398, -397, -395, -394, -392, -390, -389, -387, -385, -384, -382, -380, -379, -377, -375, -374, -372, -370, -369, -367, -365, -364, -362, -360, -359, -357, -355, -354, -352, -350, -349, -347, -345, -344, -342, -340, -338, -337, -335, -333, -332, -330, -328, -327, -325, -323, -322, -320, -318, -316, -315, -313, -311, -310, -308, -306, -305, -303, -301, -299, -298, -296, -294, -293, -291, -289, -287, -286, -284, -282, -281, -279, -277, -275, -274, -272, -270, -268, -267, -265, -263, -262, -260, -258, -256, -255, -253, -251, -249, -248, -246, -244, -243, -241, -239, -237, -236, -234, -232, -230, -229, -227, -225, -223, -222, -220, -218, -216, -215, -213, -211, -209, -208, -206, -204, -202, -201, -199, -197, -195, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173, -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -150, -148, -146, -144, -143, -141, -139, -137, -135, -134, -132, -130, -128, -127, -125, -123, -121, -119, -118, -116, -114, -112, -111, -109, -107, -105, -103, -102, -100, -98, -96, -95, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73, -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -46, -45, -43, -41, -39, -38, -36, -34, -32, -30, -29, -27, -25, -23, -21, -20, -18, -16, -14, -13, -11, -9, -7, -5, -4, -2, 0, 2, 4, 5, 7, 9, 11, 13, 14, 16, 18, 20, 21, 23, 25, 27, 29, 30, 32, 34, 36, 38, 39, 41, 43, 45, 46, 48, 50, 52, 54, 55, 57, 59, 61, 63, 64, 66, 68, 70, 71, 73, 75, 77, 79, 80, 82, 84, 86, 87, 89, 91, 93, 95, 96, 98, 100, 102, 103, 105, 107, 109, 111, 112, 114, 116, 118, 119, 121, 123, 125, 127, 128, 130, 132, 134, 135, 137, 139, 141, 143, 144, 146, 148, 150, 151, 153, 155, 157, 158, 160, 162, 164, 165, 167, 169, 171, 173, 174, 176, 178, 180, 181, 183, 185, 187, 188, 190, 192, 194, 195, 197, 199, 201, 202, 204, 206, 208, 209, 211, 213, 215, 216, 218, 220, 222, 223, 225, 227, 229, 230, 232, 234, 236, 237, 239, 241, 243, 244, 246, 248, 249, 251, 253, 255, 256, 258, 260, 262, 263, 265, 267, 268, 270, 272, 274, 275, 277, 279, 281, 282, 284, 286, 287, 289, 291, 293, 294, 296, 298, 299, 301, 303, 305, 306, 308, 310, 311, 313, 315, 316, 318, 320, 322, 323, 325, 327, 328, 330, 332, 333, 335, 337, 338, 340, 342, 344, 345, 347, 349, 350, 352, 354, 355, 357, 359, 360, 362, 364, 365, 367, 369, 370, 372, 374, 375, 377, 379, 380, 382, 384, 385, 387, 389, 390, 392, 394, 395, 397, 398, 400, 402, 403, 405, 407, 408, 410, 412, 413, 415, 416, 418, 420, 421, 423, 425, 426, 428, 430, 431, 433, 434, 436, 438, 439, 441, 442, 444, 446, 447, 449, 450, 452, 454, 455, 457, 459, 460, 462, 463, 465, 466, 468, 470, 471, 473, 474, 476, 478, 479, 481, 482, 484, 485, 487, 489, 490, 492, 493, 495, 496, 498, 500, 501, 503, 504, 506, 507, 509, 510, 512, 514, 515, 517, 518, 520, 521, 523, 524, 526, 527, 529, 530, 532, 534, 535, 537, 538, 540, 541, 543, 544, 546, 547, 549, 550, 552, 553, 555, 556, 558, 559, 561, 562, 564, 565, 567, 568, 570, 571, 573, 574, 576, 577, 579, 580, 581, 583, 584, 586, 587, 589, 590, 592, 593, 595, 596, 598, 599, 600, 602, 603, 605, 606, 608, 609, 611, 612, 613, 615, 616, 618, 619, 621, 622, 623, 625, 626, 628, 629, 630, 632, 633, 635, 636, 637, 639, 640, 642, 643, 644, 646, 647, 649, 650, 651, 653, 654, 655, 657, 658, 660, 661, 662, 664, 665, 666, 668, 669, 670, 672, 673, 674, 676, 677, 679, 680, 681, 683, 684, 685, 687, 688, 689, 690, 692, 693, 694, 696, 697, 698, 700, 701, 702, 704, 705, 706, 707, 709, 710, 711, 713, 714, 715, 716, 718, 719, 720, 722, 723, 724,
                                          };

//---- Pins -----
const int IN_4 = 6;           //PA20
const int IN_3 = 5;           //PA15
const int VREF_2 = 4;         //PA08
const int VREF_1 = 9;         //PA07
const int IN_2 = 7;           //PA21
const int IN_1 = 8;           //PA06
const int ledPin = 13;        //PA17
const int chipSelectPin = A2; //PB08
const int dir_pin = 0;        //PORT_PA11
const int step_pin = 1;       //Port_PA10
const int ena_pin = 2;        //PORT_PA14


const float M_Pi = 3.1415926535897932384626433832795;

//--- PID autotuning variables ---
bool tune_running = false; //set flag for running indication

