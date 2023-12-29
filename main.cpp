#include <iostream>
#include <thread>
#include <omp.h>
#include <random>
#include <cmath>
#include <chrono>

using namespace std;

int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    // std::random_device rd;
    std::mt19937 gen(10);
    std::uniform_int_distribution<int> rand_spin(0, 1);
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    
    int N = 100;
    
    std::uniform_int_distribution<int> rand_col(0, N - 1);
    int **S = new int*[N];
    for (int i = 0; i < N; ++i) {
        S[i] = new int[N];
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            S[i][j] = 2 * rand_spin(gen) - 1;
        }
    }

    double T = 2.5;

    int iter = 1000000;
    for (int it = 0; it < iter; ++it) {
        #pragma omp parallel for
        for (int i = 0; i < N; i+=2) {
            int j = rand_col(gen);
            int spin_sum = 0;
            for (int d = 0; d < 4; ++d) {
                spin_sum += S[(i + dx[d] + N) % N][(j + dy[d] + N) % N];
            }
            int dU = 2 * spin_sum * S[i][j];
            if (dU <= 0) S[i][j] *= -1;
            else if (exp(-dU / T) > prob(gen)) S[i][j] *= -1;
        }
        #pragma omp parallel for
        for (int i = 1; i < N; i+=2) {
            int j = rand_col(gen);
            int spin_sum = 0;
            for (int d = 0; d < 4; ++d) {
                spin_sum += S[(i + dx[d] + N) % N][(j + dy[d] + N) % N];
            }
            int dU = 2 * spin_sum * S[i][j];
            if (dU <= 0) S[i][j] *= -1;
            else if (exp(-dU / T) > prob(gen)) S[i][j] *= -1;
        }
    }
    

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Execution time(ms): " << duration << "\n";
}
