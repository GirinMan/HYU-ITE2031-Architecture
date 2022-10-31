    lw 0 1 five
    lw 1 2 65536    Error: offsetField doesn't fit in 16bits
start add 1 2 1
    beq 0 1 9       Error: invalid register value 9       
    beq 0 0 start
    nop             Error: unrecognized opcode "nop"
done halt
five .fill 5
neg1 .fill -1
stAddr .fill start
