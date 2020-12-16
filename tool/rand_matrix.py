import numpy as np

m = list(np.random.rand(20, 20) * 100 + 100)
mstr = "\n".join([",".join([str(round(float(e), 2)) for e in r]) for r in m])

with open('input_20.txt', 'w') as input_file:
    input_file.write(mstr)
