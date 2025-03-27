#!/bin/bash

# Convertir .s a .x
cd inputs
./asm2hex test_lsl_lsr.s

# Volver a raíz del proyecto
cd ..

# Correr simulador propio
echo "Corriendo simulador propio..."
./src/sim inputs/test_lsl_lsr.x <<EOF
run 10
rdump
quit
EOF > sim_output_lsl_lsr.txt

# Correr simulador de referencia
echo "Corriendo simulador de referencia..."
./ref_sim inputs/test_lsl_lsr.x <<EOF
run 10
rdump
quit
EOF > ref_output_lsl_lsr.txt

# Comparar resultados
echo "Comparando salidas..."
diff -u ref_output_lsl_lsr.txt sim_output_lsl_lsr.txt || echo "⚠️  Las salidas difieren."

echo "✅ Comparación terminada."