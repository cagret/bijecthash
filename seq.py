import random

# Définition des lettres représentant les bases d'ADN
bases = ['A', 'T', 'C', 'G']

# Fonction pour générer une séquence d'ADN aléatoire
def generate_random_sequence(length):
    return ''.join(random.choice(bases) for _ in range(length))

# Nombre de séquences à générer
num_sequences = 5

# Longueur de chaque séquence
sequence_length = 50

# Générer les séquences aléatoires
sequences = [generate_random_sequence(sequence_length) for _ in range(num_sequences)]

# Créer le fichier FASTA
with open('sequences.fasta', 'w') as file:
    for i, seq in enumerate(sequences, start=1):
        file.write(f'>Sequence_{i}\n')
        file.write(f'{seq}\n')

