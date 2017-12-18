/*
 * Joo-Haeng Lee
 * 2007-11-22
 *
 * To test the data structure for Lafortune mapping of MERL BRDFs
 * based on "Exp. Anal. of BRDF Models" by A. Ngan et al (2005.)
 *
 */
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

typedef map<string, float> StrFMap;

void buildMap_Lafortune();
StrFMap param_Lafortune(string name);

void buildMap_CookTorrance();
StrFMap param_CookTorrance(string name);
void printStrVecfMap(StrFMap p);

//////////////////////////////////////////////////////////////////////////
//  [12/2/2008 Sung-Soo Kim] add additional analytic models.
void buildMap_Ward();
StrFMap param_Ward(string name);

void buildMap_WardDuer();
StrFMap param_WardDuer(string name);

void buildMap_BlinnPhong();
StrFMap param_BlinnPhong(string name);

void buildMap_AshikhminShirley();
StrFMap param_AshikhminShirley(string name);

void buildMap_He();
StrFMap param_He(string name);
