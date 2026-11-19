import numpy as np

# Definiranje normalizirane tanh funkcije
def norm_tanh(x):
    return ((np.tanh(x) + 1) / 2)

# Funkcija za izračunavanje kvadratne pogreške
def izracun_greske(predvidjeno, ocekivano):
    return np.mean((predvidjeno - ocekivano) ** 2)

# Definiranje mreže s fiksnim težinama
class NeuralnaMreza:
    def __init__(self):
        # Težine između ulaznog sloja i skrivenog sloja
        self.weights_ulaz_skriveno = np.array([
            [-1.5, -2.0, 2.5],
            [3.0, -3.5, 4.0],
            [4.5, 5.0, 5.5]
        ])

        # Bias za skriveni sloj
        self.bias_skriveno = np.array([-1.0, -1.5, 1.0])

        # Težine između skrivenog sloja i izlaznog sloja
        self.weights_skriveno_izlaz = np.array([
            [1.0, 0.5],
            [1.5, 1.5],
            [2.5, 2.5]
        ])

        # Bias za izlazni sloj
        self.bias_izlaz = np.array([1.3, 2.1])
    
    def feedforward(self, ulaz):
        # Prva propagacija kroz mrežu
        skriveno_layer_ulaz = np.dot(ulaz, self.weights_ulaz_skriveno) + self.bias_skriveno
        skriveno_layer_izlaz = norm_tanh(skriveno_layer_ulaz)

        # Druga propagacija kroz mrežu
        izlaz_layer_ulaz = np.dot(skriveno_layer_izlaz, self.weights_skriveno_izlaz) + self.bias_izlaz
        izlaz_layer_izlaz = izlaz_layer_ulaz
        return izlaz_layer_izlaz

# Ulazne vrijednosti i očekivane izlazne vrijednosti
ulaz = np.array([1.5, 2.0, -2.5])
ocekivano_izlaz = np.array([1.6, 2.6])

# Kreiranje instance mreže
nm = NeuralnaMreza()

# Izračun izlazne vrijednosti
predvidjeno_izlaz = nm.feedforward(ulaz)
print("Izlazne vrijednosti mreže:", predvidjeno_izlaz)

# Izračun pogreške
greska = izracun_greske(predvidjeno_izlaz, ocekivano_izlaz)
print("Pogreška mreže:", greska)
