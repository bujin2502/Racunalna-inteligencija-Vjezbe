import numpy as np
import sys
import random

class Tocka:
    def __init__(self, x, y):
        self.x = x
        self.y = y

def f(x, koef):
    return koef[0] / (koef[1] * x**2 + koef[2] * x + koef[3])

def funkcija_greske(koef, tocke):
    suma = 0.0
    for t in tocke:
        y_pred = f(t.x, koef)
        suma += (y_pred - t.y)**2
    return suma

def main(argv):
    if len(argv) != 11:
        print(f"Usage: {argv[0]} x1 y1 x2 y2 x3 y3 x4 y4 x5 y5")
        return 1
    
    tocke = [Tocka(float(argv[i]), float(argv[i+1])) for i in range(1, len(argv), 2)]

    POP_SIZE = 100
    DIM = 4
    MAX_GEN = 2000
    CR = 0.9
    F = 0.8
    LB, UB = -10.0, 10.0

    populacija = np.random.uniform(LB, UB, (POP_SIZE, DIM))
    fitnes = np.array([funkcija_greske(ind, tocke) for ind in populacija])

    with open("evolucija.csv", "w") as csv_file:
        csv_file.write("Iteracija,a,b,c,d,Greska\n")

        for gen_num in range(MAX_GEN):
            for i in range(POP_SIZE):
                indeksi = list(range(POP_SIZE))
                indeksi.remove(i)
                random.shuffle(indeksi)
                a, b, c = [populacija[idx] for idx in indeksi[:3]]

                mutant = a + F * (b - c)

                maska = np.random.rand(DIM) < CR
                maska[np.random.randint(DIM)] = True
                trial = np.where(maska, mutant, populacija[i])
                trial = np.where(trial < LB, (LB + populacija[i]) / 2, trial)
                trial = np.where(trial > UB, (UB + populacija[i]) / 2, trial)
                
                trial_fit = funkcija_greske(trial, tocke)
                
                if trial_fit < fitnes[i]:
                    populacija[i] = trial
                    fitnes[i] = trial_fit

            min_fit = np.min(fitnes)
            best = np.argmin(fitnes)
            csv_file.write(f"{gen_num+1},{populacija[best][0]},{populacija[best][1]},{populacija[best][2]},{populacija[best][3]},{min_fit}\n")

if __name__ == "__main__":
    main(sys.argv)
