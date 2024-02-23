import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Charger les données
df = pd.read_csv('benchmark_results.csv')

# Configurer le graphique pour le temps d'exécution
fig, ax1 = plt.subplots()

# Axe pour le temps d'exécution
color = 'tab:red'
ax1.set_xlabel('KSize')
ax1.set_ylabel('Execution Time (ms)', color=color)
ax1.tick_params(axis='y', labelcolor=color)

# Préparer les couleurs pour chaque dataset
datasets = df['Dataset'].unique()
colors_execution = plt.cm.tab10(np.linspace(0, 1, len(datasets)))

# Tracer les lignes pour le temps d'exécution
for i, dataset in enumerate(datasets):
    subset_identity = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Identity')]
    subset_random = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Random')]
    
    # Utiliser des styles de lignes différents pour distinguer Identity de Random
    ax1.plot(subset_identity['KSize'], subset_identity['ExecutionTime(ms)'], label=f'Identity {dataset}', color=colors_execution[i], linestyle='-')
    ax1.plot(subset_random['KSize'], subset_random['ExecutionTime(ms)'], label=f'Random {dataset}', color=colors_execution[i], linestyle='--')

# Afficher le graphique pour le temps d'exécution
fig.tight_layout()
plt.title('Algorithm Performance: Execution Time by Execution Type')
plt.legend(loc='upper left', fontsize='small', ncol=2)
plt.show()


# Configurer le graphique pour la variance
fig, ax2 = plt.subplots()

# Axe pour la variance
color = 'tab:blue'
ax2.set_xlabel('KSize')
ax2.set_ylabel('Variance', color=color)
ax2.tick_params(axis='y', labelcolor=color)

# Préparer les couleurs pour chaque dataset
colors_variance = plt.cm.viridis(np.linspace(0, 1, len(datasets)))

# Tracer les lignes pour la variance
for i, dataset in enumerate(datasets):
    subset_identity = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Identity')]
    subset_random = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Random')]
    
    ax2.plot(subset_identity['KSize'], subset_identity['Variance'], label=f'Identity {dataset}', color=colors_variance[i], linestyle='-')
    ax2.plot(subset_random['KSize'], subset_random['Variance'], label=f'Random {dataset}', color=colors_variance[i], linestyle='--')

# Afficher le graphique pour la variance
fig.tight_layout()
plt.title('Algorithm Performance: Variance by Execution Type')
plt.legend(loc='upper left', fontsize='small', ncol=2)
plt.show()

