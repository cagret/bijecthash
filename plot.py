import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Lire les données
df = pd.read_csv('benchmark_results.csv')

# Configurer le graphique
fig, ax1 = plt.subplots()

# Axe pour le temps d'exécution
color = 'tab:red'
ax1.set_xlabel('KSize')
ax1.set_ylabel('Execution Time (ms)', color=color)
ax1.tick_params(axis='y', labelcolor=color)

# Générer des lignes pour chaque taille de dataset
datasets = df['Dataset'].unique()
colors = plt.cm.viridis(np.linspace(0, 1, len(datasets)))

for i, dataset in enumerate(datasets):
    subset = df[df['Dataset'] == dataset]
    ax1.plot(subset['KSize'], subset['ExecutionTime(ms)'], label=f'{dataset}', color=colors[i])

# Axe secondaire pour la variance
ax2 = ax1.twinx()
color = 'tab:blue'
ax2.set_ylabel('Variance', color=color)
ax2.tick_params(axis='y', labelcolor=color)

for i, dataset in enumerate(datasets):
    subset = df[df['Dataset'] == dataset]
    ax2.plot(subset['KSize'], subset['Variance'], '--', color=colors[i], label=f'Variance {dataset}')

fig.tight_layout()
plt.title('Algorithm Performance: Execution Time and Variance in Suffix Distribution')
ax1.legend(loc='upper left')
ax2.legend(loc='upper right')
plt.show()

