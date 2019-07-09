set title "Cumulative Dynamic Linking/Loading Time" textcolor rgb "@TEXTCOLOR@"
set xlabel textcolor rgb "@TEXTCOLOR@"
set xtics rotate
set ylabel "µs" textcolor rgb "@TEXTCOLOR@"
set yrange [0:]
set grid
set style data lines
set border linecolor rgb "@TEXTCOLOR@"
set key textcolor rgb "@TEXTCOLOR@"

lazySum=0
nowSum=0

sumLazy(x) = (lazySum = lazySum + x, lazySum)
sumNow(x)  = (nowSum = nowSum + x, nowSum)

plot 'ldbenchmark.csv' using 0:(sumLazy($2)):($2-$3) with yerrorbars notitle linecolor rgb "#808080" pointsize 0, \
     'ldbenchmark.csv' using 0:2:xticlabels(1) with lines smooth cumulative title "RTLD_LAZY" linecolor rgb "#bf0303", \
     'ldbenchmark.csv' using 0:(sumNow($5)):($5-$6) with yerrorbars notitle linecolor rgb "#808080" pointsize 0, \
     'ldbenchmark.csv' using 0:5 with lines smooth cumulative title "RTLD_NOW" linecolor rgb "#2C72C7"
