#!/bin/bash
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

# Vérification du nombre d'arguments
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <fasta_path> [-p num_processes] [-c]"
    exit 1
fi

# Définir le chemin du fichier fasta et la longueur à utiliser
fasta_path="$1"
export fasta_path=$fasta_path

k1_values=(8 9 10 11 12 13)
# Liste des méthodes à exécuter
if [[ "$@" == *"-c"* ]]; then
    methods=('composition=(random_nucl*canonical)' 'composition=(lyndon*canonical)' 'composition=(random_bits*canonical)' 'composition=(bwt*canonical)' 'composition=(zigzag*canonical)' 'composition=(inthash*canonical)' 'composition=(minimizer*canonical)' 'composition=(GAB*canonical)' 'composition=(identity*canonical)' 'composition=(inverse*canonical)' 'composition=(cyclic*canonical)')
else
    methods=(identity random_bits random_nucl cyclic lyndon bwt inverse zigzag inthash minimizer GAB)
fi

rm -f result.txt

# Fonction pour exécuter une méthode avec une valeur de k1
run_method() {
    k=30
    method="$1"
    k1="$2"
    temp_file=$(mktemp)
    echo "k= $k"
    echo "k1= $k1"
    ./src/BijectHash "$fasta_path" --quiet --nb-bins 5 --length "$k" --method "$method" --prefix-length "$k1" >> "$temp_file"
    cat "$temp_file" >> result.txt
    rm "$temp_file"
}

export -f run_method

# Vérifier s'il y a une option pour le nombre de processus
if [[ "$@" == *"-p"* ]]; then
  num_processes=$(echo "$@" | sed -n 's/.*-p \([0-9]*\).*/\1/p')
  for method in "${methods[@]}"; do
    echo "Exécution avec la méthode : $method"
      parallel -j "$num_processes" run_method ::: "$method" ::: "${k1_values[@]}"
    done
else
    for method in "${methods[@]}"; do
        echo "Exécution avec la méthode : $method"
        for k1 in "${k1_values[@]}"; do
            run_method "$method" "$k1"
        done
    done
fi

# Fermer le fichier result.txt
exec 3>&-
