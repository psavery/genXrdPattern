/*  Generate powder diffraction using ObjCryst++
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

#include <cstdlib>
#include <sstream>

#include "ObjCryst/ObjCryst/DiffractionDataSingleCrystal.h"
#include "ObjCryst/ObjCryst/CIF.h"
#include "ObjCryst/Quirks/VFNStreamFormat.h"

using namespace ObjCryst;
using namespace std;

// Case insensitive startsWith check
bool iStartsWith(const std::string& s, const std::string& start)
{
  if (start.size() > s.size())
    return false;

  for (size_t i = 0; i < start.size(); ++i) {
    if (tolower(start[i]) != tolower(s[i]))
      return false;
  }
  return true;
}

// If there is an '=' sign, get the part to the righ of that
std::string getValue(const std::string& s)
{
  size_t pos = s.find("=");
  if (pos == string::npos || pos + 1 >= s.size())
    return " ";

  return s.substr(pos + 1);
}

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
  if (argc < 2) {
    cerr << "Usage: <exe> <cifFile> [<options>]\n";
    cerr << "To use stdin instead, replace <cifFile> with "
         << "'--read-from-stdin'\n";
    cerr << "Use --help for more info.\n";
    return 1;
  }

  if (iStartsWith(argv[1], "--help")) {
    cerr << "Usage: <exe> <cifFile> [<options>]\n";
    cerr << "To use stdin instead, replace <cifFile> with "
         << "'--read-from-stdin'\n";
    cerr << "Options:\n"
         << "  --wavelength=<num>: set the wavelength (Angstroms)\n"
         << "  --peakwidth=<num>:  set the peak width (degrees)\n"
         << "  --numpoints=<num>:  set the number of points\n"
         << "  --max2theta=<num>:  set the max 2*theta (degrees)\n";
    return 0;
  }

  bool readFromStdin = false;

  if (iStartsWith(argv[1], "--read-from-stdin"))
    readFromStdin = true;

  // Defaults
  double wavelength = 1.54056;
  double peakWidth = 0.01;
  long   numPoints = 1000;
  double max2Theta = M_PI * .9;

  // Process the arguments
  for (int i = 2; i < argc; ++i) {
    if (iStartsWith(argv[i], "--wavelength="))
      wavelength = atof(getValue(argv[i]).c_str());
    else if (iStartsWith(argv[i], "--peakwidth="))
      peakWidth = atof(getValue(argv[i]).c_str()) * DEG2RAD;
    else if (iStartsWith(argv[i], "--numpoints="))
      numPoints = atoi(getValue(argv[i]).c_str());
    else if (iStartsWith(argv[i], "--max2theta="))
      max2Theta = atof(getValue(argv[i]).c_str()) * DEG2RAD;
    else
      cerr << "Warning: Unrecognized option: " << argv[i] << "\n";
  }

  if (wavelength < 1.e-6)
    cerr << "Warning: Wavelength is too small. A crash may be eminent.\n";

  string filename(readFromStdin ? "" : argv[1]);

  cerr << "# Loading: " << (readFromStdin ? "stdin" : filename) << endl;

  // Read either from the file or stdin
  Crystal* pCryst;
  if (readFromStdin) {
    pCryst = loadCIF(cin);
  }
  else {
    ifstream in(filename.c_str());
    pCryst = loadCIF(in);
  }

  PowderPatternDiffraction* diffData = new PowderPatternDiffraction();
  diffData->SetCrystal(*pCryst);
  diffData->SetReflectionProfilePar(PROFILE_PSEUDO_VOIGT,
                                    peakWidth * peakWidth);
  diffData->GetCrystal().SetUseDynPopCorr(true);

  PowderPattern data;
  data.SetRadiationType(RAD_XRAY);
  data.SetWavelength(wavelength);
  data.SetPowderPatternPar(0, max2Theta / numPoints,
                           numPoints);
  // add CaF2 as a Crystalline phase
  data.SetMaxSinThetaOvLambda(50.0);
  data.AddPowderPatternComponent(*diffData);
  // we don't have data, so just simulate (0->Pi/2)..
  // give a constant 'obs pattern of unit intensity
  CrystVector_REAL obs(numPoints);
  obs = 1;
  data.SetPowderPatternObs(obs);
  data.Prepare();

  cerr << "# Auto-simulating powder pattern:" << endl
       << "#    Crystal: " << pCryst->GetName() << endl
       << "#    Wavelength: " << wavelength << endl
       << "#    2theta: 0->" << max2Theta * RAD2DEG << "?("
       << numPoints << " points)" << endl
       << "#    peak width: " << peakWidth * RAD2DEG << "?" << endl
       << "#    to stdout" << endl;
  CrystVector_REAL ttheta, icalc;
  icalc = data.GetPowderPatternCalc();
  icalc *= 100 / icalc.max();
  ttheta = data.GetPowderPatternX();
  if (data.GetRadiation().GetWavelengthType() != WAVELENGTH_TOF)
    ttheta *= RAD2DEG;
  cout << "# Simulated data for crystal:" << pCryst->GetName()
       << endl;
  cout << "#    2Theta/TOF    ICalc" << endl;
  cout << FormatVertVector<REAL>(ttheta,icalc,12,4);
}
