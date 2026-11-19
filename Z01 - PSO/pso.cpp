#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>

using namespace std;

// POČETAK - OVDJE UBACI (copy - paste) kod iz svog zadatka
//
//

double funkcijaD(double x)
{
    double fx = 0.3 * (x - 3.0) * cos(5.0 * x - 15.0) + 1.0;

    if (fx >= 0.8)
        fx = sin(3.0 / (pow(x - 3.0, 2.0) + 1.0));

    return fx;
}

//
//
// KRAJ FUNKCIJE

int main()
{
    unsigned long long seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937_64 rng(seed);
    double Xlb = 1.0;
    double Xub = 6.0;
    double xNB;
    double fxNB;
    unsigned swarmSize = 5;
    vector<double> xPB;
    vector<double> x;
    vector<double> v;

    long nIterations = 50;
    double w = 0.6;
    double C1 = 1.2;
    double C2 = 1.8;

    uniform_real_distribution<double> Rx(Xlb, Xub);
    uniform_real_distribution<double> r(0.0, 1.0);
    ofstream MyFile("PSO_01.csv");

    x.resize(swarmSize);
    v.resize(swarmSize);
    xPB.resize(swarmSize);
    for (unsigned i = 0; i < swarmSize; i++)
    {
        x.at(i) = Rx(rng);
        v.at(i) = 0.0;
        xPB.at(i) = x.at(i);
        if (i == 0 || funkcijaD(xPB.at(i)) < fxNB)
        {
            xNB = xPB.at(i);
            fxNB = funkcijaD(xNB);
        }
    }

    MyFile << "najbolji_X ; funkc(najbolji_X)" << endl;

    for (long iter = 0; iter < nIterations; iter++)
    {
        for (unsigned i = 0; i < swarmSize; i++)
        {
            v[i] = w * v[i] + C1 * r(rng) * (xPB[i] - x[i]) + C2 * r(rng) * (xNB - x[i]);
            x[i] = x[i] + v[i];

//
//
// POČETAK - OVDJE UBACI GILJEVIĆEV DIO

            if (x[i] < Xlb || x[i] > Xub)
                x[i] = Rx(rng);

// KRAJ GILJEVIĆEVOG DIJELA
//
//
            
            if (funkcijaD(x[i]) < funkcijaD(xPB[i]))
            {
                xPB[i] = x[i];
            }
            if (funkcijaD(xPB[i]) < fxNB)
            {
                xNB = xPB[i];
                fxNB = funkcijaD(xNB);
            }
        }
        MyFile << xNB << ";" << fxNB << endl;
    }

    cout << "Najbolje pronadeno rjesenje: xNB = " << xNB << "  f(xNB) = " << fxNB << endl;

    MyFile.close();
    return 0;
}
