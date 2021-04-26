import sys
import numpy as np
import os
import matplotlib.pyplot as plt


def main(argv):


	file = argv[0].split(".")[:-1]

	fname1 = "outputs3"+os.sep+"H@5-Evaluate-"+file+".txt"
	fname2 = "outputs3"+os.sep+"H@15-Evaluate-"+file+".txt"
	fname3 = "outputs3"+os.sep+"H@25-Evaluate-"+file+".txt"


	f1 = np.loadtxt(fname1)

	#plt.figure(figsize=(10,8))

	#plt.plot_surface()

	print(f1.shape)




if __name__ == "__main__":
    main(sys.argv[1:])