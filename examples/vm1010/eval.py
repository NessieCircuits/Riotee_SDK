import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

a = np.loadtxt("/tmp/samples.txt", delimiter=",")
plt.plot(a)
plt.ylim((-(2**11), 2**11))
plt.show()
