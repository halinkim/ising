#include <iostream>
#include <thread>
#include <omp.h>
#include <random>
#include <cmath>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <regex>
#include <cstdlib>
 
std::string ltrim(std::string const &str)
{
    return std::regex_replace(str, std::regex("^\\s+"), "");
}
 
std::string rtrim(std::string const &str)
{
    return std::regex_replace(str, std::regex("\\s+$"), "");
}
 
std::string trim(std::string const &str)
{
   return ltrim(rtrim(str));
}


using namespace std;

int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int **S;
int N;
double T;
int iter;
int out_every;

void parse_parameter(const string &pathname) {
    ifstream fin;
    fin.open(pathname);
    unordered_map<string, double> par_map;
    string ln;
    while (getline(fin, ln)) {
        if (ln.empty() || ln[0] == '#') {
            continue;
        }
        istringstream iss(ln);
        string parName;
        double parVal;
        if (getline(iss, parName, '=') && iss >> parVal) {
            par_map[trim(parName)] = parVal;
        }
    }
    fin.close();
    // for (const auto & pair: par_map) {
    //     cout << pair.first << " : " << pair.second << "\n";
    // }

    if (par_map.find("N") != par_map.end()) {
        N = round(par_map["N"]);
    }
    else {
        N = 100;
    }
    if (par_map.find("out_every") != par_map.end()) {
        out_every = round(par_map["out_every"]);
    }
    else {
        out_every = 100;
    }
    if (par_map.find("T") != par_map.end()) {
        T = par_map["T"];
    }
    else {
        T = 1.0;
    }
    if (par_map.find("iter") != par_map.end()) {
        iter = round(par_map["iter"]);
    }
    else {
        iter = 100;
    }
}

void print_lattice(int it) {
    ofstream fout;
    fout.open("data/ising_it" + to_string(it) + ".out");
    fout << N << "\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            fout << S[i][j] << " ";
        }
        fout << "\n";
    }
    fout.close();
}
int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    parse_parameter("par.par");

    // std::random_device rd;
    std::mt19937 gen(10);
    std::uniform_int_distribution<int> rand_spin(0, 1);
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> rand_col(0, N - 1);

    S = new int*[N];
    for (int i = 0; i < N; ++i) {
        S[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            S[i][j] = 2 * rand_spin(gen) - 1;
        }
    }
    
    print_lattice(0);
    for (int it = 1; it <= iter; ++it) {
        for (int _it = N * N; _it > 0; --_it) {
            int I = rand_col(gen);
            int j = rand_col(gen);
            int spin_sum = 0;
            for (int d = 0; d < 4; ++d) {
                spin_sum += S[(I + dx[d] + N) % N][(j + dy[d] + N) % N];
            }
            int dU = 2 * spin_sum * S[I][j];
            if (dU <= 0) S[I][j] *= -1;
            else if (exp(-dU / T) > prob(gen)) S[I][j] *= -1;
        }
        // for (int _it = 0; _it < N; ++_it) {
        //     #pragma omp parallel for
        //     for (int i = 0; i < (N + 1) / 2; ++i) {
        //         int I = 2 * i;
        //         int j = rand_col(gen);
        //         int spin_sum = 0;
        //         for (int d = 0; d < 4; ++d) {
        //             spin_sum += S[(I + dx[d] + N) % N][(j + dy[d] + N) % N];
        //         }
        //         int dU = 2 * spin_sum * S[I][j];
        //         if (dU <= 0) S[I][j] *= -1;
        //         else if (exp(-dU / T) > prob(gen)) S[I][j] *= -1;
        //     }
        //     #pragma omp parallel for
        //     for (int i = 0; i < N / 2; ++i) {
        //         int I = 2 * i + 1;
        //         int j = rand_col(gen);
        //         int spin_sum = 0;
        //         for (int d = 0; d < 4; ++d) {
        //             spin_sum += S[(I + dx[d] + N) % N][(j + dy[d] + N) % N];
        //         }
        //         int dU = 2 * spin_sum * S[I][j];
        //         if (dU <= 0) S[I][j] *= -1;
        //         else if (exp(-dU / T) > prob(gen)) S[I][j] *= -1;
        //     }
        // }
        if (it % out_every == 0) {
            print_lattice(it);
        }
    }
    

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Execution time(ms): " << duration << "\n";
    string cmd = "python3 visual.py";
    system(cmd.c_str());
}
