#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

using namespace std;

struct Tocka {
    double x, y;
};

vector<Tocka> tocke;

// Funkcija krivulje
// Računa vrijednost funkcije za dani x i skup koeficijenata
double f(double x, const vector<double>& koef) {
    return koef[0] / (koef[1] * x * x + koef[2] * x + koef[3]);
}

// Funkcija greške
// Računa ukupnu kvadratnu grešku između stvarnih točaka i predviđanja funkcije f
double funkcijaGreske(const vector<double>& koef) {
    double suma = 0.0;
    for (const auto& t : tocke) {
        double y_pred = f(t.x, koef);
        suma += (y_pred - t.y) * (y_pred - t.y);
    }
    return suma;
}

int main(int argc, char* argv[]) {
    // Provjera broja argumenata i čitanje točaka iz komandne linije
    if (argc != 11) {
        cerr << "Korištenje: " << argv[0] << " x1 y1 x2 y2 x3 y3 x4 y4 x5 y5" << endl;
        return 1;
    }

    for (int i = 1; i < argc; i += 2) {
        tocke.push_back({stod(argv[i]), stod(argv[i + 1])});
    }

    const int POP_SIZE = 100;  // Velicina populacije
    const int DIM = 4;  // Broj koeficijenata funkcije
    const int MAX_GEN = 2000;  // Maksimalan broj generacija
    const double CR = 0.9;  // Vjerojatnost krizanja
    const double F = 0.8;   // Faktor diferencijacije
    const double LB = -10.0, UB = 10.0;  // Granice koeficijenata (sprjecava beskonacni rast zbog skalabilnosti modela)

    vector<vector<double>> populacija(POP_SIZE, vector<double>(DIM));
    vector<double> fitnes(POP_SIZE);
    mt19937 gen(random_device{}());
    uniform_real_distribution<> distr(LB, UB);
    uniform_real_distribution<> crDistr(0, 1);
    uniform_int_distribution<> jDistr(0, DIM - 1);

    // Inicijalizacija populacije nasumičnim koeficijentima
    for (auto& jedinka : populacija) {
        generate(jedinka.begin(), jedinka.end(), [&]() { return distr(gen); });
    }

    // Evaluacija početne populacije
    transform(populacija.begin(), populacija.end(), fitnes.begin(), funkcijaGreske);

    ofstream csvFile("evolucija.csv");
    csvFile << "Iteracija,a,b,c,d,Greska\n";

    // Glavna petlja evolucijskog algoritma
    for (int genNum = 0; genNum < MAX_GEN; ++genNum) {
        for (int i = 0; i < POP_SIZE; ++i) {
            // Selekcija tri nasumična indeksa različita od i za operaciju mutacije
            vector<int> indeksi(POP_SIZE);
            iota(indeksi.begin(), indeksi.end(), 0);
            indeksi.erase(remove(indeksi.begin(), indeksi.end(), i), indeksi.end());
            shuffle(indeksi.begin(), indeksi.end(), gen);

            const auto& a = populacija[indeksi[0]];
            const auto& b = populacija[indeksi[1]];
            const auto& c = populacija[indeksi[2]];

            // Mutacija - stvaranje mutant vektora
            vector<double> mutant(DIM);
            for (int j = 0; j < DIM; ++j) {
                mutant[j] = a[j] + F * (b[j] - c[j]);
            }

            // Krizanje - kombiniranje mutant vektora s trenutnom jedinkom
            // j_rand osigurava da barem jedan gen dolazi iz mutanta
            vector<double> trial(DIM);
            int jRand = jDistr(gen);
            for (int j = 0; j < DIM; ++j) {
                if (crDistr(gen) < CR || j == jRand) {
                    trial[j] = mutant[j];
                } else {
                    trial[j] = populacija[i][j];
                }
                // Popravak granica - gen izvan granica postavlja se na sredinu izmedju roditelja i granice
                if (trial[j] < LB) trial[j] = (LB + populacija[i][j]) / 2;
                if (trial[j] > UB) trial[j] = (UB + populacija[i][j]) / 2;
            }

            // Evaluacija novonastale jedinke
            double trialFit = funkcijaGreske(trial);

            // Selekcija - usporedba i moguća zamjena s trenutnom jedinkom
            if (trialFit < fitnes[i]) {
                populacija[i] = trial;
                fitnes[i] = trialFit;
            }
        }

        // Logiranje najbolje jedinke u trenutnoj generaciji
        double minFit = *min_element(fitnes.begin(), fitnes.end());
        auto best = min_element(fitnes.begin(), fitnes.end()) - fitnes.begin();
        csvFile << genNum + 1 << "," << populacija[best][0] << "," << populacija[best][1] << "," << populacija[best][2] << "," << populacija[best][3] << "," << minFit << "\n";

    }

    csvFile.close();
    return 0;
}