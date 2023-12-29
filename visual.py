import matplotlib.pyplot as plt
import sys

# print(sys.argv[1:])
par_map = dict()
with open('par.par', 'r') as inf:
    for ln in inf:
        if ln.count('=') == 1:
            Name, Val = ln.split('=')
            Name = Name.strip()
            Val = Val.strip()
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


its = [i * out_every for i in range(max_iter // out_every + 1)]

for it in its:
    sys.stdin = open('data/ising_it%d.out' % it, 'r')
    N = int(sys.stdin.readline())
    LATTICE = [[*map(int, sys.stdin.readline().split())] for _ in range(N)]
    plt.matshow(LATTICE)
    plt.title('iter=%d, T=%f' % (it, T))
    plt.savefig('img/ising_N%d_T%.2f_it%d.png' % (N, T, it), dpi=300)
# plt.show()


