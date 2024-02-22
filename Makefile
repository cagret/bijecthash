# Compiler et linker
CC=gcc
CXX=g++
LD=g++

# Options de compilation
CFLAGS=-I. -O2
CXXFLAGS=-I. -O2

# Nom de l'exécutable à créer
TARGET=BijectHash

# Définir tous les fichiers objet nécessaires pour le lien final
OBJ_FILES=inthash.o main.o fileReader.o

# La première règle est celle exécutée par défaut ("make")
# Elle dépend de l'exécutable final
$(TARGET): $(OBJ_FILES)
	$(LD) -o $(TARGET) $(OBJ_FILES)

# Règle pour compiler les fichiers source C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour compiler les fichiers source C++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle clean pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET) *.o

