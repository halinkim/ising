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
#include <filesystem>

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

bool isNumeric(const std::string& s) {
    std::regex nonNumericNonDotRegex("[^0-9.]");
    return !std::regex_search(s, nonNumericNonDotRegex);
}
bool isString(const std::string& s) {
    std::regex quotedStringRegex(R"((["'])(.*?)\1)");
    std::smatch match;
    return std::regex_match(s, match, quotedStringRegex);
}



using namespace std;

int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int **S;
int N;
double T;
int iter;
int out_every;
string out_path = "simulation/ising/";

void parse_parameter(const string &pathname) {
    ifstream fin;
    fin.open(pathname);
    unordered_map<string, double> par_map;
    unordered_map<string, string> par_map_string;
    string ln;
    while (getline(fin, ln)) {
        if (ln.empty() || ln[0] == '#') {
            continue;
        }
        istringstream iss(ln);
        string parName;
        string parVal;
        if (getline(iss, parName, '=') && iss >> parVal) {
            parName = trim(parName);
            parVal = trim(parVal);
            if (isString(parVal)) {
                par_map_string[parName] = parVal.substr(1, parVal.size() - 2);
            }
            else if (isNumeric(parVal)) {
                par_map[parName] = stod(parVal);
            }
            else {
                cout << "Failed to parse: " << parName << "\n";
            }
        }
    }
    fin.close();

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
    // if (par_map_string.find("par_path") != par_map_string.end()) {
    //     par_path = par_map_string["par_path"];
    // }
    if (par_map_string.find("out_path") != par_map_string.end()) {
        out_path = par_map_string["out_path"];
    }
    if (!filesystem::exists(out_path + "data/")) {
        if (!filesystem::create_directories(out_path + "data/")) {
            std::cerr << "Failed to Create Directory in out_path" << std::endl;
            return;
        }
    }
    ofstream fout;
    fout.open(out_path + ".par");
    fout << "N = " << N << "\n";
    fout << "T = " << T << "\n";
    fout << "iter = " << iter << "\n";
    fout << "out_path = '" << out_path << "'\n";
    fout << "out_every = " << out_every << "\n";
    fout.close();
}

void print_lattice(int it) {
    ofstream fout;
    fout.open(out_path + "data/data_it" + to_string(it) + ".out");
    fout << N << "\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            fout << S[i][j] << " ";
        }
        fout << "\n";
    }
    fout.close();
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <par_path>" << std::endl;
        return 1;
    }
    parse_parameter(argv[1]);

    auto start_time = std::chrono::high_resolution_clock::now();
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
        if (it % out_every == 0) {
            print_lattice(it);
        }
    }
    

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Execution time(ms): " << duration << "\n";
    string cmd = "python3 visual.py " + out_path;
    system(cmd.c_str());
}
