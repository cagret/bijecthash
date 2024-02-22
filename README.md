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

## Example

Here's an example of how to use BijectHash:

`./BijectHash -f sequences.fasta 30`



This command will generate permutations of 30-mers from the sequences in the `sequences.fasta` file.

## License
This project is licensed under the [GNU General Public License v3.0](LICENSE).
