#!/usr/bin/env python3

import argparse
import matplotlib.pyplot as plt
import sys

import subprocess
from subprocess import PIPE

if len(sys.argv) < 2:
  print("Usage: <exe> <cif> [options]")
  sys.exit("Type 'Use --help' for help on options")

parser = argparse.ArgumentParser(description='Options for powder diffraction')
parser.add_argument('cif', type=str,
                    help='The CIF file to use')
parser.add_argument('--wavelength', type=float, default=1.54056,
                    help='The wavelength in Angstroms (Default: 1.54056)')
parser.add_argument('--peakwidth', type=float, default=0.572958,
                    help='The peak width in degrees (Default: 0.572958)')
parser.add_argument('--numpoints', type=int, default=1000,
                    help='The number of points (Default: 1000)')
parser.add_argument('--max2theta', type=float, default=162,
                    help='The maximum 2*theta in degrees (Default: 162)')

args = vars(parser.parse_args())

fileName = args['cif']

with open(fileName, 'r') as rf:
  cifData = rf.read().encode('utf-8')

cmd = ['./build/genPowderDiffraction', '--read-from-stdin',
       '--wavelength=' + str(args['wavelength']),
       '--peakwidth='  + str(args['peakwidth']),
       '--numpoints='  + str(args['numpoints']),
       '--max2theta='  + str(args['max2theta'])]
p = subprocess.Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=PIPE)

output, err = p.communicate(cifData)
rc = p.returncode

output = output.decode('utf-8')

#print('rc is:', rc)
print('output is:', output)
print(err.decode('utf-8'))

x = []
y = []
lines = output.split('\n')
strBeforeData = 'Changed powder pattern'
startedData = False
# If we count 5 lines in a row with 2 columns, assume we have found the data
for i, line in enumerate(lines):
  line = line.split('#')[0].strip()
  if not line:
    continue

  if startedData:
    x.append(float(line.split()[0]))
    y.append(float(line.split()[1]))

  elif strBeforeData in line:
    startedData = True

plt.xlabel(r'2$\theta$ (degrees)')
plt.ylabel('Intensity')
plt.title('Powder Pattern')

plt.plot(x, y, 'r')
plt.show()
