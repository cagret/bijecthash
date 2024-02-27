import csv
import matplotlib.pyplot as plt
import numpy as np

# Lire le fichier CSV
filename = "benchmark2_results.csv"

# Initialiser un dictionnaire pour stocker les données
data = {}

with open(filename, newline="") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        method = row['Method']
        deciles = [int(row[f'Decile{i}']) for i in range(1, 11)]
        data[method] = deciles


# Préparer les données pour le graphique
deciles = [f'{i*10}%' for i in range(1, 11)]
methods = list(data.keys())
values = np.array(list(data.values()))

# Nombre de déciles et nombre de méthodes
num_deciles = len(deciles)
num_methods = len(methods)

# Largeur des barres
bar_width = 0.8 / num_methods

# Configurer les positions des barres sur l'axe X
indices = np.arange(num_deciles)
positions = [indices + bar_width*i for i in range(num_methods)]

# Créer le graphique
plt.figure(figsize=(12, 8))

for pos, method, val in zip(positions, methods, values):
    plt.bar(pos, val, bar_width, label=method, alpha=0.5)

plt.xlabel('Décile')
plt.ylabel('Nombre de suffixes')
plt.title('Nombre de suffixes par décile pour différentes méthodes')
plt.xticks(indices + bar_width * (num_methods - 1) / 2, deciles)
plt.legend(title="Méthodes")
plt.grid(axis='y', linestyle='--')

plt.show()

# Créer des graphiques pour chaque méthode
for method, deciles in data.items():
    plt.figure(figsize=(10, 6))
    plt.bar(range(1, 11), deciles, tick_label=[f'{i*10}%' for i in range(1, 11)])
    plt.title(f'Répartition des nombres de suffixes par déciles pour {method}')
    plt.xlabel('Décile')
    plt.ylabel('Nombre de suffixes')
    plt.grid(axis='y', linestyle='--')
    plt.show()

    # Afficher des statistiques de base pour chaque méthode
    print(f'Méthode: {method}')
    print(f'  Minimum: {min(deciles)}')
    print(f'  Maximum: {max(deciles)}')
    print(f'  Moyenne: {np.mean(deciles):.2f}')
    print(f'  Médiane: {np.median(deciles)}')
    print()

