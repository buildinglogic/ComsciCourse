# Uncomment 2 lines below to create a .png file instead of displaying using X
#set terminal png size 600, 600
#set output "Whole_Program.png"
# Set aspect ration, zoom factor
set size square 1.0, 1.0
# Set plot title
set title "Trial results for Whole Program"
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
3.104740 notitle lt 2, \
'-' title 'Burdened Parallelism' with lines lt 1
1 1.000000
2 1.281691
3 1.414509
4 1.491804
5 1.542374
6 1.578036
7 1.604536
8 1.625002
9 1.641284
10 1.654547
11 1.665559
12 1.674848
13 1.682790
14 1.689657
15 1.695654
16 1.700937
e
