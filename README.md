# faster_mapper-
for semester project

https://github.com/xhongyi/MS_internship/blob/master/split_sam/split_sam.cc

g++ f2.cc -O3 --std=c++11 -fopenmp -o a.out

./a.out /big_data/hxin/NA12878/NA12878_GRCh37.sam

2TB SSD
2TB HDD
time ./a.out /big_data/hxin/NA12878/NA12878_GRCh37.sam

ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/10Xgenomics_ChromiumGenome_LongRanger2.0_06202016/NA12878_GRCh37.bam
samtool view NA12878_GRCh37.bam > NA12878_GRCh37 .sam  
./a.out NA12878_GRCh37.sam  split_barcode_folder
