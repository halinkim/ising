#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <omp.h>
#include <fstream>
#include <cfloat>
#include <chrono>
using namespace std;

int main()
{
    const int N = 1000000;
    vector<vector<double>> c;
    default_random_engine g(0);
    uniform_real_distribution<double> d(0.0f, nextafter(1.0f, DBL_MAX));
    c.reserve(N);

    for (int i = 0; i < N; i++) {
        const unsigned size = pow(10, i % 4);
        vector<double> a;
        a.reserve(size);

        for (int j = 0; j < size; j++) {
            const double number = d(g);
            a.push_back(number);
        }

        c.push_back(std::move(a));
    }

    double sum = 0.0;
    vector<double> b(N);

    #pragma omp parallel num_threads(4) firstprivate(N) shared(b,c,sum)
    {
        #pragma omp for reduction(+:sum)
        for (int i = 0; i < N ; i++) {
            double sumLocal = 0.0;

            for (int j = 0; j < c[i].size();j++) {
                sumLocal += pow(c[i][j], 2);
            }

            const double n = sqrt(sumLocal);
            b[i] = n;

            sum += sumLocal;
        }
    }
}