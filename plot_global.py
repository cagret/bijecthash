import pandas as pd
import matplotlib.pyplot as plt

# Lire les données des deux fichiers CSV
df_benchmark = pd.read_csv('benchmark_results.csv')
df_benchmark_hash = pd.read_csv('benchmark_hash_results.csv')

# Fusionner les deux DataFrames en se basant sur les colonnes 'Dataset' et 'KSize'
merged_df = pd.merge(df_benchmark, df_benchmark_hash, on=['Dataset', 'KSize'], suffixes=('_benchmark', '_hash'))

# Sélectionner uniquement les colonnes numériques pour le calcul de la moyenne
cols_to_mean = ['ExecutionTime(ms)_benchmark', 'Variance_benchmark', 'ExecutionTime(ms)_hash', 'Variance_hash']

# Calculer les moyennes pour le temps d'exécution et la variance pour chaque KSize
average_df = merged_df.groupby('KSize')[cols_to_mean].mean().reset_index()

# Création du graphique
plt.figure(figsize=(12, 7))

# Tracer le temps d'exécution moyen pour chaque méthode
plt.plot(average_df['KSize'], average_df['ExecutionTime(ms)_benchmark'], color='red', marker='o', label='Benchmark Execution Time (ms)')
plt.plot(average_df['KSize'], average_df['ExecutionTime(ms)_hash'], color='blue', marker='s', label='Benchmark Hash Execution Time (ms)')

# Ajouter un titre et des étiquettes
plt.title('Comparaison des Performances des Benchmarks')
plt.xlabel('Taille de K (KSize)')
plt.ylabel('Temps d\'Exécution Moyen (ms)')
plt.legend()

# Créer un deuxième axe Y pour la variance
ax2 = plt.twinx()
ax2.plot(average_df['KSize'], average_df['Variance_benchmark'], color='green', marker='x', linestyle='--', label='Benchmark Variance')
ax2.plot(average_df['KSize'], average_df['Variance_hash'], color='purple', marker='+', linestyle='--', label='Benchmark Hash Variance')
ax2.set_ylabel('Variance Moyenne (log scale)')
ax2.set_yscale('log')
ax2.legend(loc='upper left')

# Afficher le graphique
plt.tight_layout()
plt.savefig('benchmark_comparison.pdf', format='pdf')  # Sauvegarder le graphique en format PDF pour la qualité
plt.show()

