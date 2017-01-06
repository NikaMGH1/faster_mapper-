#! /bin/bash



gcc-6 seq_2_r.c -o seq_2_r -lm

gcc-6 seq_1_r.c -o seq_1_r -lm
gcc-6 -fopenmp par_2_omp_r.c -o par_2_omp_r -lm
gcc-6 -fopenmp par_1_omp_r.c -o par_1_omp_r -lm
gcc-6 -fopenmp par_2_omp_d.c -o par_2_omp_d -lm
gcc-6 -fopenmp par_1_omp_d.c -o par_1_omp_d -lm
gcc-6 -fopenmp par_1_omp_memeff.c -o par_1_omp_memeff -lm

for i in `seq 1 50`;
do

     echo $i
     ./seq_2_r "b.txt" "pattern2.txt" 1000 >> 2SEQ.txt
    	./seq_1_r "b.txt" "pattern.txt" 2 >> 1SEQ.txt
     echo "seq done"
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  2 >> 2r_2core.txt
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  8 >> 2r_4core.txt
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  64 >> 2r_4core.txt
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  128 >> 2r_8core.txt
     

     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  3 >> 2r_3core.txt
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  60 >> 2r_5core.txt
     ./par_2_omp_r "b.txt" "pattern2.txt" 1000  100 >> 2r_6core.txt
     
     
     echo "par2r done"
     
     
     ./par_1_omp_r "b.txt" "pattern.txt" 2  2 >> 1r_2core.txt
     ./par_1_omp_r "b.txt" "pattern.txt" 2  8 >> 1r_4core.txt
     ./par_1_omp_r "b.txt" "pattern.txt" 2  64 >> 1r_8core.txt
     ./par_1_omp_r "b.txt" "pattern.txt" 2  128 >> 1r_8core.txt

     ./par_1_omp_r "b.txt" "pattern.txt" 2  3 >> 1r_3core.txt
     ./par_1_omp_r "b.txt" "pattern.txt" 2  60 >> 1r_5core.txt
     ./par_1_omp_r "b.txt" "pattern.txt" 2  100 >> 1r_6core.txt
     
      echo "par1r done"


     
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  2 >> 2d_2core.txt
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  8 >> 2d_4core.txt
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  64 >> 2d_8core.txt
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  128 >> 2d_8core.txt
     

     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  3 >> 2d_3core.txt
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  60 >> 2d_5core.txt
     ./par_2_omp_d "b.txt" "pattern2.txt" 1000  100 >> 2d_6core.txt
     
     
     echo "par2d done"

     ./par_1_omp_d "b.txt" "pattern.txt" 2  2 >> 1d_2core.txt
     ./par_1_omp_d "b.txt" "pattern.txt" 2  8 >> 1d_4core.txt
     ./par_1_omp_d "b.txt" "pattern.txt" 2  64 >> 1d_8core.txt
     ./par_1_omp_d "b.txt" "pattern.txt" 2  128 >> 1d_8core.txt
     

     ./par_1_omp_d "b.txt" "pattern.txt" 2  3 >> 1d_3core.txt
     ./par_1_omp_d "b.txt" "pattern.txt" 2  60 >> 1d_5core.txt
     ./par_1_omp_d "b.txt" "pattern.txt" 2  100 >> 1d_6core.txt
     
     echo "par1d done"

      
    
echo
done
