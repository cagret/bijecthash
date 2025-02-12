#!/usr/bin/env python3
###############################################################################
#                                                                             #
#  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     #
#                           (Laboratoire d'Informatique, de Robotique et de   #
#                           Microélectronique de Montpellier /                #
#                           Centre National de la Recherche Scientifique /    #
#                           Université de Montpellier)                        #
#                           CRIStAL/CNRS/UL                                   #
#                           (Centre de Recherche en Informatique, Signal et   #
#                           Automatique de Lille /                            #
#                           Centre National de la Recherche Scientifique /    #
#                           Université de Lille)                              #
#                                                                             #
#  Auteurs/Authors:                                                           #
#                   Clément AGRET      <cagret@mailo.com>                     #
#                   Annie   CHATEAU    <annie.chateau@lirmm.fr>               #
#                   Antoine LIMASSET   <antoine.limasset@univ-lille.fr>       #
#                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             #
#                   Camille MARCHET    <camille.marchet@univ-lille.fr>        #
#                                                                             #
#  Programmeurs/Programmers:                                                  #
#                   Clément AGRET      <cagret@mailo.com>                     #
#                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  This file is part of BijectHash.                                           #
#                                                                             #
#  BijectHash is free software: you can redistribute it and/or modify it      #
#  under the terms of the GNU General Public License as published by the      #
#  Free Software Foundation, either version 3 of the License, or (at your     #
#  option) any later version.                                                 #
#                                                                             #
#  BijectHash is distributed in the hope that it will be useful, but WITHOUT  #
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      #
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   #
#  more details.                                                              #
#                                                                             #
#  You should have received a copy of the GNU General Public License along    #
#  with BijectHash. If not, see <https://www.gnu.org/licenses/>.              #
#                                                                             #
###############################################################################

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Chemins vers les fichiers CSV (assurez-vous que ces chemins sont corrects et que les fichiers sont accessibles)
file_paths = {
    "Cyclic": "cyclic_data.csv",
    "Identity": "identity_data.csv",
    "Inverse": "inverse_data.csv",
    "Random": "random_data.csv",
    "Zigzag": "zigzag_data.csv"
}

# Chargement des données depuis les fichiers CSV dans des DataFrames et les trier
dfs = {name: pd.read_csv(path).sort_values(by='SuffixCount', ascending=True).reset_index(drop=True) for name, path in file_paths.items()}
#for name, df in dfs.items():
#    df.sort_values(by='SuffixCount', inplace=True)

# Calcul de la somme des 'SuffixCount' pour chaque décile de chaque méthode
decile_sums = {name: [] for name in dfs.keys()}

for name, df in dfs.items():
    total_rows = len(df)
    for decile in range(10):
        start_idx = int(np.floor(decile / 10.0 * total_rows))
        end_idx = int(np.floor((decile + 1) / 10.0 * total_rows))
        decile_sum = df.iloc[start_idx:end_idx]['SuffixCount'].sum()
        decile_sums[name].append(decile_sum)

# Préparation pour le tracé de l'histogramme
bar_width = 0.15
decile_indices = np.arange(10)  # Indices pour les déciles

plt.figure(figsize=(15, 10))

# Tracé de l'histogramme avec des barres groupées pour chaque méthode
colors = ['blue', 'orange', 'green', 'red', 'purple']
for i, (name, sums) in enumerate(decile_sums.items()):
    plt.bar(decile_indices + i * bar_width, sums, width=bar_width, label=name, color=colors[i],alpha=0.5)

# Configuration des étiquettes et du titre
plt.xlabel('Déciles')
plt.ylabel('Somme de SuffixCount')
plt.title('Somme de SuffixCount par décile pour chaque méthode')
plt.xticks(decile_indices + bar_width * 2, [f'{10 * i}-{10 * (i+1)}%' for i in range(10)])
plt.legend()

# Affichage de l'histogramme
plt.tight_layout()
plt.show()
