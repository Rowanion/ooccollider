set terminal table; set output "thesis.pgf-plot.table"; set format "%.5f"
set samples 200; plot [x=0:8] (1)* (cos((1)*x+(2))) + 0
