import numpy as np
from scipy.signal import hilbert
import matplotlib.pyplot as plt

# 1. Citirea semnalului dintr-un fișier .txt
input_file = "waveData.txt"  # Numele fișierului de intrare
signal = np.loadtxt(input_file)  # Citire vector semnal

# 2. Crearea unui vector de timp pe baza lungimii semnalului (opțional)
fs = 1000  # Frecvența de eșantionare presupusă (poți ajusta dacă e cunoscută)
t = np.linspace(0, len(signal) / fs, len(signal), endpoint=False)  # Vector timp

# 3. Calculul transformatei Hilbert și al anvelopei
analytic_signal = hilbert(signal)  # Semnalul analitic
amplitude_envelope = np.abs(analytic_signal)  # Anvelopa semnalului

# 4. Salvarea rezultatelor într-un fișier
output_data = np.column_stack((t, signal, np.real(analytic_signal),
                               np.imag(analytic_signal), amplitude_envelope))
np.savetxt("hilbert_results.txt", amplitude_envelope, delimiter="\n", fmt="%.6f")

# 5. Salvarea numărului de elemente într-un fișier separat
num_elements = len(amplitude_envelope)  # Calculul numărului de elemente
with open("hilbert_count.txt", "w") as count_file:
    count_file.write(f"{num_elements}\n")  # Scrierea numărului în fișier