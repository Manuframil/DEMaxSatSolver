import csv
import sys
import numpy as np
import matplotlib.pyplot as plt
import os

solutionfile = "data/mse20-incomplete-unweighted-best.csv"


def getBestSol(filename):
    key = filename + '.gz'
    with open(solutionfile) as csvfile:
        rows = csv.reader(csvfile, delimiter=',')
        for row in rows:
            if row[0] == key:
                return row[1]
    return -1


def main(argv):
    endings = ['_CR0.50_F0.50_LSS0.01_WS0.50', '_CR0.50_F0.50_LSS0.01_WS0.00', '_CR0.50_F0.50_LSS0.01_WS1.00',
               '_CR0.50_F0.50_LSS0.00_WS0.50']
    labels = ['DE+GWSat', 'DE+GSat', 'DE+WalkSat', 'DE Raw']
    colors = ['aquamarine', 'salmon', 'gold', 'lime']

    txt_path = argv[0][:-5]

    bestsol = getBestSol(argv[0])

    wcnf_file = txt_path.split('/')[-1]

    plt.figure(figsize=(10, 8))

    plt.axhline(bestsol, linestyle="solid", color='red', label="Best sol found")

    for end, label, color in zip(endings, labels, colors):
        data = np.zeros((250, 4), dtype=float)
        count = 0
        for filename in sorted(os.listdir(txt_path + end)):
            fname = os.path.join(txt_path + end, filename)
            tmpdata = np.loadtxt(fname=fname, delimiter=',', dtype=float)
            data += tmpdata
            count += 1

        data /= count
        tt = np.arange(0, len(data[:, 0]))
        plt.plot(tt, data[:, 0], "o-", color=color, label=f"Best {label}", markersize=2)
        plt.plot(tt, data[:, 1], "-", color=color, alpha=0.25, markersize=1)
        plt.plot(tt, data[:, 2], "-", color=color, label=f"Mean {label}", markersize=1)

    plt.grid(True)
    plt.legend(loc='upper right')

    plt.savefig(f'imgs/compare_{wcnf_file}.png')


if __name__ == "__main__":
    main(sys.argv[1:])
