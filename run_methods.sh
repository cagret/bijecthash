#!/bin/bash

# Définir le chemin du fichier fasta et la longueur à utiliser
fasta_path="resources/SRR072008.fastq"
length="30"

# Liste des méthodes à exécuter
methods=(identity random cyclic inverse zigzag inthash GAB)

rm result.txt

for method in "${methods[@]}"; do
    echo "Exécution avec la méthode : $method"
    ./src/BijectHash "$fasta_path" "$length" "$method" >> result.txt
done

