# bijecthash


BijectHash is a tool designed for generating permutations of k-mers from DNA sequences and encoding them into unique integers using bijective hash functions.

## Getting Started

To get started with BijectHash, follow these steps:

1. Clone the repository:

`git clone https://github.com/cagret/bijecthash`

2. Make sure you have GCC installed on your system.

3. Navigate to the `bijecthash` directory:

`cd bijecthash`



4. Compile the project using the provided Makefile:

`make`



## Usage

Once the project is compiled, you can use BijectHash as follows:

`Usage: ./BijectHash -f <filename> <k>`



Replace `<filename>` with the path to your FASTA file containing DNA sequences, and `<k>` with the length of k-mers you want to generate permutations for.



## Generating sequences.fasta

You can generate `sequences.fasta` using the provided Python script `seq.py`. Run the following command:

`python3 seq.py`



## Benchmarking

This repository includes two benchmarking tools: benchmark and benchmark_hash. These utilities are designed to evaluate the performance of our k-mer hashing algorithms across various genomic sequence files and k-mer sizes.
### Compilation

To compile the benchmarks, use the following g++ commands which will produce the executable files benchmark and benchmark_hash:

```bash
g++ -o benchmark benchmark.cpp fileReader.cpp -std=c++11 -O3 -fopenmp
g++ -o benchmark_hash benchmark_hash.cpp fileReader.cpp inthash.c -std=c++11 -O3 -fopenmp
```

Ensure you have a C++11 compatible compiler and the necessary files: benchmark.cpp, benchmark_hash.cpp, fileReader.cpp, and inthash.c.
#### Running the Benchmarks

After compilation, you can run the benchmarks with a list of FASTA files and a range of k-mer sizes. The files should be semicolon-separated, and the k-mer sizes should be space-separated as shown below:


```bash
./benchmark -f "sequences_100000.fa;sequences_400000.fa;sequences_700000.fa;sequences_1000000.fa" 18 20 22 24 26 28 30
./benchmark_hash -f "sequences_100000.fa;sequences_400000.fa;sequences_700000.fa;sequences_1000000.fa" 18 20 22 24 26 28 30
```

The above commands will execute the benchmarks for the specified files and k-mer sizes. The results will be outputted detailing the execution time and variance for each dataset and k-mer size combination.
### Output

The benchmarks generate a CSV file with columns for Dataset, KSize, ExecutionTime(ms), and Variance. These files will be named benchmark_results.csv and benchmark_hash_results.csv for benchmark and benchmark_hash utilities, respectively.




## Example

Here's an example of how to use BijectHash:

`./BijectHash -f sequences.fasta 30`



This command will generate permutations of 30-mers from the sequences in the `sequences.fasta` file.

## License
This project is licensed under the [GNU General Public License v3.0](LICENSE).
