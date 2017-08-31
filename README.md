# Overview
Scallop is an accurate reference-based transcript assembler. Scallop features
its high accuracy in assembling multi-exon transcripts as well as lowly
expressed transcripts. Scallop achieves this improvement through a novel
algorithm that can be proved preserving all phasing paths from reads and paired-end reads,
while also achieves both transcripts parsimony and coverage deviation minimization.

**Pre-print** of Scallop is available at [bioRxiv](http://biorxiv.org/content/early/2017/04/03/123612).
The datasets and scripts used in this paper to compare the performance of Scallop
and other assemblers are available at [**scalloptest**](https://github.com/Kingsford-Group/scalloptest).

Please also checkout the **podcast** about Scallop (thanks [Roman Cheplyaka](https://ro-che.info/) for the interview).
It is available at both [the bioinformatics chat](https://bioinformatics.chat/scallop) and
[iTunes](https://itunes.apple.com/us/podcast/the-bioinformatics-chat/id1227281398). 

# Release
Latest release, including both binary and source code, is [here](https://github.com/Kingsford-Group/scallop/releases/tag/v0.10.1).

# Installation
Scallop uses additional libraries of 
Boost, htslib and Clp. 
If they have not been installed in your system, you first
need to download and install them.  You can either install
them to the default system directories (for example, `/usr/local`),
in which case you need to use `sudo` to install,
or you can install them to your home directories, in which case you need to
specify `--prefix=/path/to/your/home/directory` when you run `configure`.
After install these dependencies, you then compile the source code of Scallop.
If some of the dependencies are installed to your home directories, then
they are required to be specified when run `configure` of Scallop.

The compilation process of both dependencies and Scallop requires `automake` and `autoconf` packages.
If they have not been installed, on linux platform, do the following:
```
sudo apt-get install autoconf
sudo apt-get install automake
```

## Download Boost
If Boost has not been downloaded/installed, download Boost
[(license)](http://www.boost.org/LICENSE_1_0.txt) from (http://www.boost.org).
Uncompress it somewhere (compiling and installing are not necessary).

## Install htslib
If htslib has not been installed, download htslib 
[(license)](https://github.com/samtools/htslib/blob/develop/LICENSE)
from (http://www.htslib.org/) with version 1.5 or higher.
Note that htslib relies on zlib. So if zlib has not been installed in your system,
you need to install zlib first. To do so, download zlib
[(license)](https://zlib.net/zlib_license.html) at (https://zlib.net/)
Use the following commands to install zlib:
```
./configure
make
make install
```
After installing zlib, use the following commands to build htslib
(provide `--prefix=/path/to/your/local/directory` to `./configure`
if you want to install htslib to your home directory.):
```
autoconf
./configure --disable-bz2 --disable-lzma --disable-gcs --disable-s3 --enable-libcurl=no
make
make install
```

## Install Clp
If Clp has not been installed in your system, 
download Clp [(license)](https://opensource.org/licenses/eclipse-1.0)
from (https://projects.coin-or.org/Clp). 
Use the following to install Clp
(provide `--prefix=/path/to/your/local/directory` to `./configure`
if you want to install Clp to your home directory):
```
./configure --disable-bzlib --disable-zlib
make
make install
```

## Compile Scallop

Use the following to compile Scallop:
```
cd src
./configure
make
```
Notice that if htslib and/or Clp are installed to your home directories,
you need to provide additional arguments to `configure`:
```
./configure --with-clp=/path/to/your/Clp/path --with-htslib=/path/to/your/htslib/path`
```
The executable file `scallop` will appear at `src/src/scallop`.


# Usage

The usage of `scallop` is:
```
./scallop -i <input.bam> -o <output.gtf> [options]
```

The `input.bam` is the read alignment file generated by some RNA-seq aligner, (for example, TopHat2, STAR, or HISAT2).
Make sure that it is sorted; otherwise run `samtools` to sort it:
```
samtools sort input.bam > input.sort.bam
```

The reconstructed transcripts shall be written as gtf format into `output.gtf`.

Scallop support the following parameters. Please also refer
to the additional explanation below the table.

 Parameters | Default Value | Description
 ------------------------- | ------------- | ----------
 --help  | | print usage of Scallop and exit
 --version | | print version of Scallop and exit
 --preview | | show the inferred `library_type` and exit
 --verbose | 1 | chosen from {0, 1, 2}
 --library_type               | empty | chosen from {empty, unstranded, first, second}
 --min_transcript_coverage    | 1 | the minimum coverage required to output a multi-exon transcript
 --min_single_exon_coverage   | 20 | the minimum coverage required to output a single-exon transcript
 --min_transcript_length_base      |150 | the minimum base length of a transcript
 --min_transcript_length_increase  | 50 | the minimum increased length of a transcript with each additional exon
 --min_mapping_quality        | 1 | ignore reads with mapping quality less than this value
 --min_bundle_gap             | 50 | the minimum distances required to start a new bundle
 --min_num_hits_in_bundle     | 20 | the minimum number of reads required in a bundle
 --min_flank_length           | 3 | the minimum match length required in each side for a spliced read
 --min_splice_bundary_hits    | 1 | the minimum number of spliced reads required to support a junction

1. For `--verbose`, 0: quiet; 1: one line for each splice graph; 2: details of graph decomposition.

2. `--library_type` is highly recommended to provide. The `unstranded`, `first`, and `second`
correspond to `fr-unstranded`, `fr-firststrand`, and `fr-secondstrand` used in standard Illumina
sequencing libraries. If none of them is given, i.e., it is `empty` by default, then Scallop
will try to infer the `library_type` by itself (see `--preview`). Notice that such inference is based
on the `XS` tag stored in the input `bam` file. If the input `bam` file do not contain `XS` tag,
then it is essential to provide the `library_type` to Scallop. You can try `--preview` to see
the inferred `library_type`.

3. `--min_transcript_coverage` is used to filter lowly expressed transcripts: Scallop will filter
out transcripts whose (predicted) raw counts (number of moleculars) is less than this number.

4. `--min_transcript_length_base` and `--min_transcript_length_increase` is combined to filter
short transcripts: the minimum length of a transcript is given by `--min_transcript_length_base`
\+ `--min_transcript_length_increase` * num-of-exons-in-this-transcript. Transcripts that are less
than this number will be filtered out.
