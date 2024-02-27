# Compiler et linker
CC=gcc
CXX=g++
LD=g++

# Options de compilation
CFLAGS=-Isrc -O3 # Utilisé pour la compilation C
CXXFLAGS=-Isrc -fopenmp -O3 -std=c++17 # Utilisé pour la compilation C++
LDFLAGS= # Options du linker, si nécessaire

# Nom de l'exécutable à créer
TARGET=BijectHash

# Définir tous les fichiers objet nécessaires pour le lien final
OBJ_FILES=src/inthash.o src/main.o src/fileReader.o

# La première règle est celle exécutée par défaut ("make")
# Elle dépend de l'exécutable final
$(TARGET): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJ_FILES)

# Règle pour compiler les fichiers source C
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour compiler les fichiers source C++
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle pour compiler le fichier de benchmark, maintenant avec permutations.o inclus
benchmark: src/benchmark.o src/fileReader.o src/inthash.o src/permutations.o
	$(LD) $(LDFLAGS) -o benchmark src/benchmark.o src/fileReader.o src/inthash.o src/permutations.o -O3 -fopenmp

# Règle clean pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) benchmark src/*.o *~

