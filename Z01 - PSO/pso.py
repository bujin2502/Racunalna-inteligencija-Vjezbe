import numpy as np  # Uvoz biblioteke numpy za matematičke operacije
import time  # Uvoz vremenske biblioteke za generisanje slučajnog sjemena
from pyswarm import pso

# Definicija ciljne funkcije koju želimo optimizirati
def funkcijaD(x):
    # Definicija funkcije fx
    fx = 0.3 * (x - 3.0) * np.cos(5.0 * x - 15.0) + 1.0
    # Ako je vrijednost fx veća ili jednaka 0.8, koristi drugu funkciju
    if fx >= 0.8:
        fx = np.sin(3.0 / ((x - 3.0) ** 2.0 + 1.0))
    return fx

# Omotač za ciljnu funkciju koji omogućava rad s pojedinačnim vrijednostima i nizovima
def funkcijaD_omot(x):
    if isinstance(x, np.ndarray):  # Provjerava tip varijable (niz ili pojedinačna vrijednost)
        return np.array([funkcijaD(xi) for xi in x])  # Ako je niz, primjenjuje funkciju na svaku vrijednost
    else:
        return funkcijaD(x)  # Ako je pojedinačna vrijednost, primjenjuje funkciju na tu vrijednost

# Funkcija za optimizaciju pomoću PSO s upravljanjem granicama
def pso_upravljanje_granicama(objective, lb, ub):
    # Omotavanje ciljne funkcije kako bi se granice prisilile
    def omot_cilja(x, *args):
        x = np.minimum(np.maximum(x, lb), ub)  # Postavljanje vrijednosti unutar granica
        return objective(x, *args)
    
    np.random.seed(int(time.time()))  # Postavljanje sjemena za generiranje slučajnih brojeva
    
    # Optimizacija pomoću PSO
    x_opt, f_opt = pso(omot_cilja, lb, ub)
    
    return x_opt, f_opt  # Vraćanje optimalnih vrijednosti

# Postavljanje donje i gornje granice za optimizaciju
lb = 2.0  # Donja granica
ub = 6.0  # Gornja granica

# Poziv funkcije za optimizaciju s upravljanjem granicama
x_opt, f_opt = pso_upravljanje_granicama(funkcijaD_omot, [lb], [ub])

# Ispis rezultata optimizacije
print("Optimalna vrijednost x:", x_opt)
print("Optimalna vrijednost funkcije:", f_opt)
