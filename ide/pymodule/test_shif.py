import sys
sys.path.append('/Users/hehao/work/lmice/ide')

import pymodule
import pymodule.shif_convert as cov
s=cov.FutureSHIF()


s.load("/Users/hehao/Desktop/future_171203.csv")

s.test()

print "done"
