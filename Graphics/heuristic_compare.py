import csv
import sys
import numpy as np
import matplotlib.pyplot as plt
import os
import re

solutionfile = "data/mse20-incomplete-unweighted-best.csv"


def getBestSol(filename):
    filename = filename.split('/')[-1]
    key = filename + '.gz'
    with open(solutionfile) as csvfile:
        rows = csv.reader(csvfile, delimiter=',')
        for row in rows:
            # print(key + ' ---> ' + ''.join(row[0].split('-')[1:]))
            if re.search(key, row[0]):
                # print(row[1])
                return int(row[1])
    return -1


def main(argv):
    endings = [
            # '_HS_all_CR0.50_F0.50_LSS0.01_WS0.50',
            #    '_HS_all_CR0.50_F0.50_LSS0.01_WS0.00',
            #    '_HS_all_CR0.50_F0.50_LSS0.01_WS1.00',
               # '_HS_all_CR0.50_F0.50_LSS0.00_WS0.50',
               '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS0.50',
               # '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS0.00',
               # '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS1.00',
               ]

    labels = ['DE+GWSat:all', 'DE+GSat:all', 'DE+WalkSat:all', 'DE Raw',
              'DE+GWSat:best', 'DE+GSat:best', 'DE+WalkSat:best']

    colors = ['aquamarine', 'salmon', 'gold', 'lime', 'orange', 'blue', 'green', 'yellow']

    txt_path = argv[0][:-5]

    bestsol = getBestSol(argv[0])
    if bestsol < 0:
        print("BEST SOL NOT FOUND")
        sys.exit(0)

    wcnf_file = txt_path.split('/')[-1]

    plt.figure(figsize=(10, 8))

    plt.axhline(bestsol, linestyle="solid", color='red', label="Best sol found")


    for end, label, color in zip(endings, labels, colors):
        data = np.zeros((1000, 4), dtype=float)
        count = 0
        for filename in sorted(os.listdir(txt_path + end)):
            fname = os.path.join(txt_path + end, filename)
            tmpdata = np.loadtxt(fname=fname, delimiter=',', dtype=float)
            data[:len(tmpdata)] += tmpdata
            count += 1

        data /= count
        # print(data[:20])
        tt = np.arange(0, len(data[:, 0]))
        plt.plot(tt, data[:, 0], "o-", color=color, label=f"{label}", markersize=2)
        plt.plot(tt, data[:, 1], "-", color=color, alpha=0.25, markersize=1)
        plt.plot(tt, data[:, 2], "-", color=color, markersize=1)

    plt.grid(True)
    plt.legend(loc='upper right')

    plt.savefig(f'imgs/w/compare_{wcnf_file}.png')


if __name__ == "__main__":
    main(sys.argv[1:])
