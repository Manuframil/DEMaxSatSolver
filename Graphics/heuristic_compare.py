import csv
import sys
import numpy as np
import matplotlib.pyplot as plt
import os

db = {
	'ram_k4_n18.ra1': (3064081, 532),
	'ram_k4_n19.ra1': (3883075, 1260),
	'ram_k4_n20.ra1': (4844949, 1988),
	'scpcyc10_maxsat': (16640, 2716),
	'scpcyc11_maxsat': (39424, 3444),
	'scpclr13_maxsat': (4810, 248),

}

def main(argv):

	files = ['', '']


	x1, y1, t1 = [],[],[]
	x2, y2, t2 = [],[],[]
	x3, y3, t3 = [],[],[]

	path = argv[0][:-5]

	with open(path+'-GWSAT.txt') as csvfile:
		plots = csv.reader(csvfile, delimiter=',')
		for row in plots:
			x1.append(float(row[0]))
			y1.append(float(row[1]))
			t1.append(float(row[2]))
	
	with open(path+'-H-CR0.5-F0.5.txt') as csvfile:
		plots = csv.reader(csvfile, delimiter=',')
		for row in plots:
			x2.append(float(row[0]))
			y2.append(float(row[1]))
			t2.append(float(row[2]))

	with open(path+'-H-CR0.9-F0.9.txt') as csvfile:
		plots = csv.reader(csvfile, delimiter=',')
		for row in plots:
			x3.append(float(row[0]))
			y3.append(float(row[1]))
			t3.append(float(row[2]))


	tt = np.arange(0, len(x1))

	################################
	# Grafica SCORE - GENERACIONES #
	################################
	plt.figure(figsize=(10,8))

	cost, sol = db[path]

	plt.axhline(cost-sol, linestyle="solid", color='red', label="Solver")

	plt.plot(tt, x1, "o-",color="aquamarine",  label="Best GWSAT", markersize=2)
	plt.plot(tt, y1, "-",color="aquamarine", label="Mean GWSAT", markersize=2)

	plt.plot(tt, x2, "o-",color="salmon",  label="Best H-CR0.5-F0.5", markersize=2)
	plt.plot(tt, y2, "-",color="salmon", label="Mean -H-CR0.5-F0.5", markersize=2)

	plt.plot(tt3, x3, "o-",color="gold",  label="Best H-CR0.9-F0.9", markersize=2)
	plt.plot(tt3, y3, "-",color="gold", label="Mean H-CR0.9-F0.9", markersize=2)
	
	plt.grid(True)
	plt.legend(loc='lower right')

	plt.savefig('compare_GENS_'+argv[0] + '.png')

	#plt.show()

if __name__ == "__main__":
    main(sys.argv[1:])