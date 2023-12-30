import matplotlib.pyplot as plt
import sys
import os

path = sys.argv[1]
par_map = dict()
with open(path + '.par', 'r') as inf:
    for ln in inf:
        if ln.count('=') == 1:
            Name, Val = ln.split('=')
            Name = Name.strip()
            Val = Val.strip()
            if Val[0] == "'" or Val[0] == '"':
                par_map[Name] = Val[1:-1]
            else:
                par_map[Name] = float(Val)

if "N" in par_map:
    N = round(par_map["N"])
else:
    N = 100
if "out_every" in par_map:
    out_every = round(par_map["out_every"])
else:
    out_every = 100
if "T" in par_map:
    T = par_map["T"]
else:
    T = 1.0
if "iter" in par_map:
    max_iter = round(par_map["iter"])
else:
    max_iter = 100

os.makedirs(path + 'img', exist_ok=True)

its = [i * out_every for i in range(max_iter // out_every + 1)]


import matplotlib.animation as animation
fig, ax = plt.subplots()
ims = []
for it in its:
    sys.stdin = open(path + 'data/data_it%d.out' % it, 'r')
    N = int(sys.stdin.readline())
    LATTICE = [[*map(int, sys.stdin.readline().split())] for _ in range(N)]
    im = ax.matshow(LATTICE)
    ax.set_title('N=%d, T=%f, iter=%d' % (N, T, it))
    ims.append([im])
    fig.savefig(path + 'img/img_N%d_T%.2f_it%d.png' % (N, T, it), dpi=300)
    # plt.matshow(LATTICE)
    # plt.title('N=%d, T=%f, iter=%d' % (N, T, it))
    # plt.savefig(path + 'img/img_N%d_T%.2f_it%d.png' % (N, T, it), dpi=300)

    print("Save fig it%d / %d" % (it, its[-1]))
# plt.show()
ani = animation.ArtistAnimation(fig, ims, interval=100)
ani.save(path + 'img/anim_N%d_T%.2f.mp4' % (N, T),  dpi=300)

