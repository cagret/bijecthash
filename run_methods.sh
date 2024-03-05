#!/bin/bash

# Définir le chemin du fichier fasta et la longueur à utiliser
fasta_path="resources/SRR072008.fastq"
length="30"
k1_values=(8 9 10 11 12 13)
# Liste des méthodes à exécuter
methods=(identity random cyclic inverse zigzag inthash GAB)

rm result.txt

# Boucle pour chaque méthode
for method in "${methods[@]}"; do
    echo "Exécution avec la méthode : $method"

    # Boucle pour chaque valeur de k1
    for k1 in "${k1_values[@]}"; do
        echo "k1 = $k1"
        ./src/BijectHash "$fasta_path" "$length" "$method" "$k1" >> result.txt
    done

done
