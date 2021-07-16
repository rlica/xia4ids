#!/usr/bin/env python3

#

import ROOT
f = ROOT.TF1("f1", "sin(x)/x", 0., 10.)
f.Draw()
