# Uncomment 2 lines below to create a .png file instead of displaying using X
#set terminal png size 600, 600
#set output "Cilk_Parallel_Region(s).png"
# Set aspect ration, zoom factor
set size square 1.0, 1.0
# Set plot title
set title "Trial results for Cilk Parallel Region(s)"
# Set X, Y axis titles
set xlabel "Worker Count"
set ylabel "Speedup"
# Specify where key is to be displayed
set key left top box
# Make points a bit bigger than the defaultset pointsize 1.8
set xrange [0:16]
set yrange [0:16]
# Plot the data
plot x title 'Parallelism' lt 2, \
159.607768 notitle lt 2, \
'-' title 'Burdened Parallelism' with lines lt 1
1 1.000000
2 1.900000
3 2.832621
4 3.674328
5 4.471555
6 5.227737
7 5.945965
8 6.629026
9 7.279441
10 7.899496
11 8.491268
12 9.056650
13 9.597368
14 10.115001
15 10.610997
16 11.086685
e
