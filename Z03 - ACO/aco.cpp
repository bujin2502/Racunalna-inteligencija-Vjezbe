// MAX-MIN Ant System (MMAS) za problem kvadratnog pridruzivanja (QAP)
//
// Prevodenje:  g++ -O2 -o aco aco.cpp
// Pokretanje:  ./aco                         (ugradeni primjer 5x5)
//              ./aco qapdata/had12.dat        (QAPLIB instanca)
//              ./aco qapdata/had12.dat 5 200  (instanca, broj mrava, broj iteracija)
//
// Rjesenje je permutacija: solution[lokacija] = objekt.
// Tijek iteracija zapisuje se u aco.csv.

#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace Constants {
    constexpr uint16_t N_ANTS = 5;
    constexpr uint32_t N_ITER = 200;
    constexpr double RHO = 0.1;            // faktor isparavanja feromona
    constexpr double ALPHA = 1.1;          // utjecaj feromona
    constexpr bool MAX_MIN = true;         // ukljuceno ogranicavanje feromona (MMAS)
    constexpr double PHERO_MAX = 1E8;      // inicijalna vrijednost za tauMax
    constexpr double PHERO_MIN = 1E-3;     // inicijalna vrijednost za tauMin
    constexpr double MIN_MAX_RATIO = 0.08; // tauMin = tauMax * MIN_MAX_RATIO
}

using Matrix = vector<vector<double>>;

/* segment izborne trake - sadrzi indeks objekta i njegovu kumulativnu vjerojatnost */
struct ProbabilitySegment
{
    uint16_t index;
    double cumulative;
};

/* klasa mrava */
class Ant
{
    private:
        mt19937 mersenneGenerator;          // generator slucajnih brojeva, dobiva sjeme iz klase Colony
        uint16_t problemSize = 0;           // broj lokacija i objekata
        Matrix* pheromones = nullptr;       // pokazivac na matricu feromona u klasi Colony

        vector<uint8_t> pickedObjects;      // zastavice odabranih objekata
        vector<ProbabilitySegment> segments;// izborna traka (rulet) medju neodabranim objektima
        void ResetSegments();
        void AddSegment(uint16_t index, double value);
        uint16_t PickSegment();
    public:
        void Init(Matrix* flows, Matrix* pheromones, mt19937 mersenneGenerator);
        void ConstructSolution(vector<uint16_t>& solution);
};

/* klasa kolonije */
class Colony
{
    private:
        mt19937 mersenneGenerator;
        Matrix *distances, *flows;          // pokazivaci na matrice problema
        Matrix pheromones;                  // feromoni[lokacija][objekt]

        Ant ant;                            // jedan mrav koji sukcesivno simulira cijelu koloniju
        uint16_t nAnts, problemSize;
        uint32_t nIterations, bestIteration = 0;
        double tauMax = Constants::PHERO_MAX, tauMin = Constants::PHERO_MIN;
        double bestAntSolutionCost = 0, bestSolutionCost = numeric_limits<double>::max();
        vector<uint16_t> currentSolution, bestAntSolution, bestSolution;

        double CalculateCost(vector<uint16_t> const& solution);
        void PheromoneEvaporate();
        void PheromoneDeposition();
        void PheromoneLimit();
    public:
        Colony(Matrix& distances, Matrix& flows, uint16_t nAnts, uint32_t nIterations);
        void Init();
        void SolveQAP(vector<uint16_t>& solution, uint32_t& iteration, double& cost, ofstream& csv);
};

/* ======================= metode klase Ant ======================= */

void Ant::Init(Matrix* flows, Matrix* pheromones, mt19937 mersenneGenerator)
{
    this->problemSize = static_cast<uint16_t>(flows->size());
    this->pheromones = pheromones;
    this->mersenneGenerator = mersenneGenerator;
    pickedObjects.resize(problemSize, 0);
    segments.reserve(problemSize + 1);
}

void Ant::ConstructSolution(vector<uint16_t>& solution)
{
    solution.clear();
    fill(pickedObjects.begin(), pickedObjects.end(), 0);

    for (uint16_t location = 0; location < problemSize; ++location)
    {
        ResetSegments();
        for (uint16_t object = 0; object < problemSize; ++object)
        {
            if (pickedObjects[object])
                continue;
            double tau = (*pheromones)[location][object];
            AddSegment(object, pow(tau, Constants::ALPHA)); // kod QAP ne koristimo heuristicki dio
        }
        uint16_t segmentIndex = PickSegment();
        uint16_t object = segments[segmentIndex].index;
        solution.push_back(object);
        pickedObjects[object] = 1;
    }
}

void Ant::ResetSegments()
{
    segments.clear();
    segments.push_back({0, 0.0}); // nulti segment koji se ne koristi
}

void Ant::AddSegment(uint16_t index, double value)
{
    segments.push_back({index, segments.back().cumulative + value});
}

uint16_t Ant::PickSegment()
{
    if (segments.size() < 2)
        return 0;
    if (segments.size() == 2)
        return 1;
    // slucajno "ubadanje" trake i binarno pretrazivanje segmenta
    uniform_real_distribution<double> distribution(0.0, segments.back().cumulative);
    double p = distribution(mersenneGenerator);
    uint16_t l = 0, r = static_cast<uint16_t>(segments.size() - 1);
    while (r - l > 1)
    {
        uint16_t m = (l + r) / 2;
        if (p > segments[m].cumulative)
            l = m;
        else
            r = m;
    }
    return r; // prvi segment cija je kumulativna vjerojatnost >= p
}

/* ======================= metode klase Colony ======================= */

Colony::Colony(Matrix& distances, Matrix& flows, uint16_t nAnts, uint32_t nIterations)
    : distances(&distances), flows(&flows), nAnts(nAnts),
      problemSize(static_cast<uint16_t>(distances.size())), nIterations(nIterations)
{
    auto seed = static_cast<mt19937::result_type>(
        chrono::high_resolution_clock::now().time_since_epoch().count());
    mersenneGenerator = mt19937(seed);

    pheromones.assign(problemSize, vector<double>(problemSize, 0.0));
    currentSolution.reserve(problemSize);
    bestAntSolution.reserve(problemSize);
    bestSolution.reserve(problemSize);

    ant.Init(this->flows, &pheromones, mt19937(mersenneGenerator()));
}

void Colony::Init()
{
    // kod MAX-MIN feromoni pocinju od maksimalne vrijednosti
    tauMax = Constants::PHERO_MAX;
    tauMin = Constants::PHERO_MIN;
    for (auto& row : pheromones)
        fill(row.begin(), row.end(), Constants::MAX_MIN ? tauMax : 1.0);
    bestSolutionCost = numeric_limits<double>::max();
    bestIteration = 0;
}

void Colony::SolveQAP(vector<uint16_t>& solution, uint32_t& iteration, double& cost, ofstream& csv)
{
    csv << "Iteracija;NajboljiMrav;NajboljeUkupno;Rjesenje\n";

    for (uint32_t iter = 0; iter < nIterations; ++iter)
    {
        bestAntSolutionCost = numeric_limits<double>::max();
        for (uint16_t a = 0; a < nAnts; ++a)
        {
            ant.ConstructSolution(currentSolution);
            double c = CalculateCost(currentSolution);
            if (c < bestAntSolutionCost)
            {
                bestAntSolutionCost = c;
                bestAntSolution = currentSolution;
            }
        }

        if (bestAntSolutionCost < bestSolutionCost)
        {
            bestSolutionCost = bestAntSolutionCost;
            bestSolution = bestAntSolution;
            bestIteration = iter + 1;
            if (Constants::MAX_MIN)
            {
                tauMax = 1.0 / ((1.0 - Constants::RHO) * bestSolutionCost);
                tauMin = tauMax * Constants::MIN_MAX_RATIO;
            }
        }

        PheromoneEvaporate();
        PheromoneDeposition();
        if (Constants::MAX_MIN)
            PheromoneLimit();

        csv << iter + 1 << ";" << bestAntSolutionCost << ";" << bestSolutionCost << ";";
        for (size_t i = 0; i < bestSolution.size(); ++i)
            csv << (i ? " " : "") << bestSolution[i];
        csv << "\n";
    }

    solution = bestSolution;
    iteration = bestIteration;
    cost = bestSolutionCost;
}

double Colony::CalculateCost(vector<uint16_t> const& solution)
{
    double result = 0.0;
    for (size_t i = 0; i < problemSize; ++i)
        for (size_t j = 0; j < problemSize; ++j)
            if (i != j)
                result += (*distances)[j][i] * (*flows)[solution[j]][solution[i]];
    return result;
}

void Colony::PheromoneEvaporate()
{
    for (auto& row : pheromones)
        for (auto& tau : row)
            tau *= (1.0 - Constants::RHO);
}

void Colony::PheromoneDeposition()
{
    double phero = 1.0 / bestAntSolutionCost;
    for (size_t location = 0; location < problemSize; ++location)
        pheromones[location][bestAntSolution[location]] += phero;
}

void Colony::PheromoneLimit()
{
    for (auto& row : pheromones)
        for (auto& tau : row)
        {
            if (tau > tauMax) tau = tauMax;
            else if (tau < tauMin) tau = tauMin;
        }
}

/* ======================= ulaz / izlaz ======================= */

// QAPLIB format: n, zatim matrica A (n x n), zatim matrica B (n x n)
bool LoadQAPLIB(const string& path, Matrix& flows, Matrix& distances)
{
    ifstream in(path);
    if (!in)
        return false;
    size_t n;
    if (!(in >> n) || n == 0 || n > 65535)
        return false;
    auto readMatrix = [&](Matrix& m) {
        m.assign(n, vector<double>(n));
        for (auto& row : m)
            for (auto& v : row)
                if (!(in >> v)) return false;
        return true;
    };
    return readMatrix(flows) && readMatrix(distances);
}

void printSolution(const vector<uint16_t>& solution, uint32_t iteration, double cost)
{
    cout << "Najbolje rjesenje pronadeno u iteraciji " << iteration << endl;
    cout << "Cijena: " << cost << endl;
    cout << "Rjesenje (lokacija -> objekt): ";
    for (auto s : solution)
        cout << s << " ";
    cout << endl;
}

int main(int argc, char* argv[])
{
    Matrix flows = {
        {0.0, 1.0, 1.5, 1.8, 0.9},
        {1.01, 0.0, 0.7, 1.1, 1.25},
        {1.55, 0.75, 0.0, 1.9, 0.5},
        {1.89, 1.115, 1.95, 0.0, 1.65},
        {0.91, 1.25, 0.5, 1.66, 0.0}};
    Matrix distances = {
        {0.0, 11.1, 10.05, 16.6, 17.99},
        {11.22, 0.0, 15.56, 8.2, 15.17},
        {10.01, 15.2, 0.0, 14.7, 9.76},
        {16.3, 8.52, 14.77, 0.0, 9.71},
        {17.77, 15.52, 9.01, 9.3, 0.0}};

    if (argc > 1 && !LoadQAPLIB(argv[1], flows, distances))
    {
        cerr << "Ne mogu ucitati QAPLIB datoteku: " << argv[1] << endl;
        return 1;
    }
    uint16_t nAnts = argc > 2 ? static_cast<uint16_t>(stoi(argv[2])) : Constants::N_ANTS;
    uint32_t nIter = argc > 3 ? static_cast<uint32_t>(stoul(argv[3])) : Constants::N_ITER;

    ofstream csv("aco.csv");
    Colony colony(distances, flows, nAnts, nIter);
    colony.Init();

    vector<uint16_t> solution;
    uint32_t iteration = 0;
    double cost = 0;
    colony.SolveQAP(solution, iteration, cost, csv);

    printSolution(solution, iteration, cost);
    return 0;
}
