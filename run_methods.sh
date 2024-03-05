#!/bin/bash

# Vérification du nombre d'arguments
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 [-p num_processes]"
    exit 1
fi

# Définir le chemin du fichier fasta et la longueur à utiliser
k1_values=(8 9 10 11 12 13)
# Liste des méthodes à exécuter
methods=(identity random cyclic inverse zigzag inthash GAB)

rm -f result.txt

# Fonction pour exécuter une méthode avec une valeur de k1
run_method() {
    fasta_path="resources/SRR072008.fastq"
    k=30
    method="$1"
    k1="$2"
    temp_file=$(mktemp)
    echo "k= $k"
    echo "k1= $k1"
    ./src/BijectHash "$fasta_path" --quiet --nb-bins 100 --length "$k" --method "$method" --prefix-length "$k1" >> "$temp_file"
    cat "$temp_file" >> result.txt
    rm "$temp_file"
}

export -f run_method

# Vérifier s'il y a une option pour le nombre de processus
if [ "$1" == "-p" ]; then
    if [ "$#" -lt 2 ]; then
        echo "Usage: $0 -p num_processes"
        exit 1
    fi
    num_processes="$2"
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

