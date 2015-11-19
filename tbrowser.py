
import os, sys
import ROOT

# enable running from another directory than the one where demo.py resides
workdir = os.path.dirname( sys.argv[0] )
if workdir:
   os.chdir( workdir )

br = ROOT.TBrowser()




