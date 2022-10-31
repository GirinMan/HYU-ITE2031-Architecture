./assembler/assemble.out assembler/$1.as $1.mc
./simulator/simulate.out $1.mc > output

rm $1.mc

