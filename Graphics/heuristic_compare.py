import csv
import sys
import numpy as np
import matplotlib.pyplot as plt
import os
import re

solutionfile_w = "data/mse20-incomplete-weighted-best.csv"
solutionfile_uw = "data/mse20-incomplete-unweighted-best.csv"

endings = [
        # Raw
        '_HS_all_CR0.50_F0.50_LSS0.00_WS0.50',
        # ED + GWSAT
        '_HS_all_CR0.50_F0.50_LSS0.01_WS0.50',
        '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS0.50',
        # ED + GSAT
        '_HS_all_CR0.50_F0.50_LSS0.01_WS0.00',
        '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS0.00',
        # ED + Random Walk
        '_HS_all_CR0.50_F0.50_LSS0.01_WS1.00',
        '_HS_better_than_mean_CR0.50_F0.50_LSS0.01_WS1.00',
           ]

labels = ['Raw ED', 
    'ED+GWSAT:all', 'ED+GWSAT:best',
    'ED+GSAT:all', 'ED+GSAT:best',
    'ED+RW:all', 'ED+RW:best']

colors = ['magenta', 
    'orange', 'yellow', 
    'lime', 'forestgreen', 
    'blue', 'cyan']

# endings = [
#         '_HS_all_CR0.50_F0.50_LSS0.01_RW0.00',
#         '_HS_all_CR0.50_F0.50_LSS0.05_RW0.00',
#         '_HS_all_CR0.50_F0.50_LSS0.10_RW0.00',
#            ]


#endings = [
        #'_HS_all_CR0.50_F0.50_LSS0.01_RW0.50',
        #'_HS_all_CR0.50_F0.50_LSS0.05_RW0.50',
        #'_HS_all_CR0.50_F0.50_LSS0.10_RW0.50',
           #]

#labels = ['1%', '5%', '10%']

#colors = ['magenta', 'lime', 'blue']


def getBestSol(filename, w_uw):
    filename = filename.split('/')[-1]
    key = filename + '.gz'
    solutionfile = ""
    if w_uw == "w":
        solutionfile = solutionfile_w
    elif w_uw == "uw":
        solutionfile = solutionfile_uw
    else:
        print(f'Is weighted? Bad argument: {w_uw}')
        sys.exit(0)

    with open(solutionfile) as csvfile:
        rows = csv.reader(csvfile, delimiter=',')
        for row in rows:
            # print(key + ' ---> ' + ''.join(row[0].split('-')[1:]))
            if re.search(key, row[0]):
                # print(row[1])
                return int(row[1])
    return -1

def graph2d(path, txt_path, w_uw, bestsol):

    plt.figure(figsize=(10, 8))

    for end, label, color in zip(endings, labels, colors):
        data = np.zeros((251, 4), dtype=float)
        count = 0
        for filename in sorted(os.listdir(path + txt_path + end)):
            fname = os.path.join(path + txt_path + end, filename)
            tmpdata = np.loadtxt(fname=fname, delimiter=',', dtype=float)
            data[:len(tmpdata)] += tmpdata
            count += 1

        data /= count
        tt = np.arange(0, len(data[:, 0]))
        plt.plot(tt, data[:, 0], "-", color=color, label=f"{label}", markersize=2)
        plt.plot(tt, data[:, 2], ":", color=color, markersize=1)

    plt.axhline(bestsol, linestyle="solid", color='red', label="Best sol found")

    plt.grid(True)
    plt.legend(loc='upper right')

    plt.savefig(f'imgs/lss/compare_GSAT-{txt_path}.png')

    plt.figure(figsize=(10, 8))

    for end, label, color in zip(endings, labels, colors):
        data = np.zeros((251, 4), dtype=float)
        count = 0
        for filename in sorted(os.listdir(path + txt_path + end)):
            fname = os.path.join(path + txt_path + end, filename)
            tmpdata = np.loadtxt(fname=fname, delimiter=',', dtype=float)
            data[:len(tmpdata)] += tmpdata
            count += 1

        data /= count
        plt.plot(data[:,3], data[:, 0], "-", color=color, label=f"{label}", markersize=2)
        plt.plot(data[:,3], data[:, 2], ":", color=color, markersize=1)

    plt.axhline(bestsol, linestyle="solid", color='red', label="Best sol found")


    plt.grid(True)
    plt.legend(loc='upper right')

    plt.savefig(f'imgs/lss/times_GSAT-{txt_path}.png')


def graph3d(path, txt_path, w_uw, bestsol):

    plt.figure(figsize=(10, 8))

    ax = plt.axes(projection='3d')

    for end, label, color in zip(endings, labels, colors):
        data = np.zeros((250, 4), dtype=float)
        count = 0

        for filename in sorted(os.listdir(path + txt_path + end)):
            fname = os.path.join(path + txt_path + end, filename)
            tmpdata = np.loadtxt(fname=fname, delimiter=',', dtype=float)
            data[:len(tmpdata)] += tmpdata
            count += 1

        data /= count
        tt = np.arange(0, len(data[:, 0]))

        ax.bar(left = tt, height = data[:,3], zs = data[:, 0], zdir = 'y', alpha = 0.8, color = color, label = label)
        ax.plot(tt, data[:,0], data[:,3], color = color)
  
    # plt.axhline(bestsol, linestyle="solid", color='red', label="Best sol found")

    ax.set_xlabel('Gens')
    ax.set_ylabel('Score')
    ax.set_zlabel('Time')

    plt.grid(True)
    plt.legend(loc='upper right')

    plt.savefig(f'imgs/{w_uw}/3d_{txt_path}.png')

def main(argv):

    path = argv[0]
    w_uw = argv[1]
    mode = argv[2]

    files = []
    for filename in sorted(os.listdir(path)):
        if filename.endswith(".wcnf"):
            files.append(filename)

    for wcnf_file in files:

        txt_path = wcnf_file[:-5]
        print(wcnf_file)

        bestsol = getBestSol(wcnf_file, w_uw)
        if bestsol < 0:
            print("BEST SOL NOT FOUND")
            sys.exit(0)

        if mode == "2d":
            graph2d(path, txt_path, w_uw, bestsol)
        elif  mode == "3d":
            graph3d(path, txt_path, w_uw, bestsol)


if __name__ == "__main__":
    main(sys.argv[1:])
