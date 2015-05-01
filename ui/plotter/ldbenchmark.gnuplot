set title "Cumulative Dynamic Linking/Loading Time"
set xtics rotate
set ylabel "µs"
set grid
plot 'ldbenchmark.csv' using (column(0)):2:xtic(1) with lines smooth cumulative title "RTLD_LAZY", \
     'ldbenchmark.csv' using (column(0)):3 with lines smooth cumulative title "RTLD_NOW"
