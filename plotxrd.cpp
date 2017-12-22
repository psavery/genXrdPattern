/*  PlotXRD using ObjCryst++
    (c) 2017 Patrick Avery psavery@buffalo.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define HAVE_SSE_MATHFUN

#include <sstream>

#include "ObjCryst/ObjCryst/DiffractionDataSingleCrystal.h"
#include "ObjCryst/ObjCryst/CIF.h"
#include "ObjCryst/Quirks/VFNStreamFormat.h"

const double cif2patternWavelength = 1.54056;
const double cif2patternPeakWidth = 0.01;
const long   cif2patternNbPoint = 1000;
const double cif2patternMax2Theta = M_PI * .9;

using namespace ObjCryst;
using namespace std;

// Load the cif and return a crystal
Crystal* loadCIF(std::istream& in)
{
  ObjCryst::CIF cif(in,true,true);
  bool oneScatteringPowerPerElement=true, connectAtoms=true;
  Crystal* pCryst = CreateCrystalFromCIF(cif, true, true, false, false);
  pCryst->MergeEqualScatteringPowers(oneScatteringPowerPerElement);
  if (connectAtoms)
    pCryst->ConnectAtoms();
  CreatePowderPatternFromCIF(cif);
  CreateSingleCrystalDataFromCIF(cif);

  return pCryst;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    cerr << "Usage: <exe> <cifFile>\n";
    return 1;
  }

  string filename(argv[1]);

  cerr << "Loading: " << filename << endl;
  ifstream in(filename.c_str());

  Crystal* pCryst = loadCIF(in);

  PowderPatternDiffraction* diffData = new PowderPatternDiffraction();
  diffData->SetCrystal(*pCryst);
  diffData->SetReflectionProfilePar(PROFILE_PSEUDO_VOIGT,
                                  cif2patternPeakWidth * cif2patternPeakWidth);
  diffData->GetCrystal().SetUseDynPopCorr(true);

  PowderPattern data;
  data.SetRadiationType(RAD_XRAY);
  data.SetWavelength(cif2patternWavelength);
  data.SetPowderPatternPar(0, cif2patternMax2Theta / cif2patternNbPoint,
                           cif2patternNbPoint);
  // add CaF2 as a Crystalline phase
  data.SetMaxSinThetaOvLambda(50.0);
  data.AddPowderPatternComponent(*diffData);
  // we don't have data, so just simulate (0->Pi/2)..
  // give a constant 'obs pattern of unit intensity
  CrystVector_REAL obs(cif2patternNbPoint);
  obs = 1;
  data.SetPowderPatternObs(obs);
  data.Prepare();

  cerr << "Auto-simulating powder pattern:" << endl
       << "   Crystal: " << pCryst->GetName()
       << endl
       << "   Wavelength: " << cif2patternWavelength << endl
       << "   2theta: 0->" << cif2patternMax2Theta * RAD2DEG << "?("
       << cif2patternNbPoint << " points)" << endl
       << "   peak width: " << cif2patternPeakWidth * RAD2DEG << "?" << endl
       << "   to stdout" << endl;
  CrystVector_REAL ttheta, icalc;
  icalc = data.GetPowderPatternCalc();
  icalc *= 100 / icalc.max();
  ttheta = data.GetPowderPatternX();
  if (data.GetRadiation().GetWavelengthType() != WAVELENGTH_TOF)
    ttheta *= RAD2DEG;
  cout << "#Simulated data for crystal:" << pCryst->GetName()
       << endl;
  cout << "#    2Theta/TOF    ICalc" << endl;
  cout << FormatVertVector<REAL>(ttheta,icalc,12,4);
}
