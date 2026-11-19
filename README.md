# Računalna inteligencija – zadaci

Implementacije osnovnih algoritama računalne inteligencije u **C++** i **Pythonu**.
Svaki zadatak je u zasebnoj mapi i može se pokrenuti zasebno.

## Sadržaj

| Mapa | Algoritam | Problem | Jezik |
|------|-----------|---------|-------|
| `Z01 - PSO` | Optimizacija rojem čestica (Particle Swarm Optimization) | Traženje minimuma jednodimenzionalne funkcije na intervalu | C++, Python |
| `Z02 - DE` | Diferencijalna evolucija (Differential Evolution) | Prilagodba krivulje `f(x) = a / (b·x² + c·x + d)` zadanim točkama | C++, Python |
| `Z03 - ACO` | MAX-MIN Ant System (MMAS) | Problem kvadratnog pridruživanja (QAP), instance iz QAPLIB-a | C++ |
| `Z04 - NN` | Unaprijedna neuronska mreža (feedforward) | Izračun izlaza i pogreške mreže s fiksnim težinama | Python |

## Preduvjeti

- C++ prevoditelj s podrškom za C++17 (npr. `g++`)
- Python 3 s paketima:
  ```bash
  pip install numpy pyswarm
  ```

## Pokretanje

### Z01 – PSO
```bash
cd "Z01 - PSO"
g++ -O2 -o pso pso.cpp && ./pso      # rezultati iteracija u PSO_01.csv
python3 pso.py                        # verzija s bibliotekom pyswarm
```

### Z02 – DE
Program prima 5 točaka `(x, y)` kroz koje se prilagođava krivulja:
```bash
cd "Z02 - DE"
g++ -O2 -o de de.cpp
./de x1 y1 x2 y2 x3 y3 x4 y4 x5 y5
python3 de.py x1 y1 x2 y2 x3 y3 x4 y4 x5 y5   # tijek evolucije u evolucija.csv
```
`de-komentari.py` je ista implementacija s detaljnim komentarima.

### Z03 – ACO
```bash
cd "Z03 - ACO"
g++ -O2 -o aco aco.cpp
./aco                           # ugrađeni primjer 5x5
./aco qapdata/had12.dat         # QAPLIB instanca
./aco qapdata/had12.dat 5 200   # instanca, broj mrava, broj iteracija
```
Tijek iteracija zapisuje se u `aco.csv`. Mapa `qapdata/` sadrži testne instance iz
[QAPLIB](https://coral.ise.lehigh.edu/data-sets/qaplib/) i nasumično generirane instance.

### Z04 – NN
```bash
cd "Z04 - NN"
python3 neuralna_mreza.py
```

## Napomene

Kod je nastao kao rješenje zadataka iz kolegija *Računalna inteligencija* i služi
u edukativne svrhe.
