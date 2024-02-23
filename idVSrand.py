import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Charger les données
df = pd.read_csv('benchmark_results.csv')

# Configurer le graphique
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
handles_execution = []
labels_execution = []
for i, dataset in enumerate(datasets):
    subset_identity = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Identity')]
    subset_random = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Random')]
    
    # Utiliser des styles de lignes différents pour distinguer Identity de Random
    ax1.plot(subset_identity['KSize'], subset_identity['ExecutionTime(ms)'], label=f'Identity {dataset}', color=colors_execution[i], linestyle='-')
    ax1.plot(subset_random['KSize'], subset_random['ExecutionTime(ms)'], label=f'Random {dataset}', color=colors_execution[i], linestyle='--')
    
    handles_execution.append(ax1.plot([],[], color=colors_execution[i], linestyle='-')[0])
    handles_execution.append(ax1.plot([],[], color=colors_execution[i], linestyle='--')[0])
    labels_execution.append(f'Identity {dataset}')
    labels_execution.append(f'Random {dataset}')

# Axe secondaire pour la variance
ax2 = ax1.twinx()
color = 'tab:blue'
ax2.set_ylabel('Variance', color=color)
ax2.tick_params(axis='y', labelcolor=color)

# Préparer les couleurs pour la variance
colors_variance = plt.cm.viridis(np.linspace(0, 1, len(datasets)))

# Tracer les lignes pour la variance
handles_variance = []
labels_variance = []
for i, dataset in enumerate(datasets):
    subset_identity = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Identity')]
    subset_random = df[(df['Dataset'] == dataset) & (df['ExecutionType'] == 'Random')]
    
    ax2.plot(subset_identity['KSize'], subset_identity['Variance'], color=colors_variance[i], linestyle='-', alpha=0.5)
    ax2.plot(subset_random['KSize'], subset_random['Variance'], color=colors_variance[i], linestyle='--', alpha=0.5)

    handles_variance.append(ax2.plot([],[], color=colors_variance[i], linestyle='-')[0])
    handles_variance.append(ax2.plot([],[], color=colors_variance[i], linestyle='--')[0])
    labels_variance.append(f'Identity {dataset}')
    labels_variance.append(f'Random {dataset}')

fig.tight_layout()
plt.title('Algorithm Performance: Execution Time and Variance by Execution Type')

# Création d'une légende qui inclut des informations sur le type d'exécution
handles = handles_execution + handles_variance
labels = labels_execution + labels_variance
plt.legend(handles, labels, loc='upper left', fontsize='small', ncol=2)
plt.show()

