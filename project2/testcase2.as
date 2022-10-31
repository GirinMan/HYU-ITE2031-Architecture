    lw 0 1 three load 3 to reg 1
    lw 0 2 five load 5 to reg 2
    beq 0 0 l1 jump to l1
    noop
    noop
    noop
    add 1 2 4 store 3 + 5 to reg 4 (not taken)
l1    add 1 2 3 store 3 + 5 to reg 3
    halt
three .fill 3
five .fill 5
six .fill 6
