        lw 0 2 var1
        lw 0 3 var2
        lw 0 4 mask
        lw 0 5 max
        noop
loop    nor 2 4 7       extract LSB from var1
        beq 0 7 calc    if LSB == 0 then go to calc
        beq 0 0 proc    else go to proc
calc    add 1 3 1       %1 = %1 + %3
proc    lw 0 7 minus    load -1 to reg 7 (symbolic)
        add 5 7 5       %5 = %5 - 1
        beq 0 5 done    if %5 == 0 then break
        add 3 3 3       %3 = %3 + %3
        add 4 4 4       %4 = %4 + %4
        lw 0 7 one      load 1 to reg 7 (symbolic)
        add 4 7 4       increment reg 4 by 1
        beq 0 0 loop    go back to loop
done    halt
var1    .fill   16384
var2    .fill   1111
mask    .fill   -2      for mask bit
one     .fill   1
minus   .fill   -1
max     .fill   15      maximum 15 bit calculation
