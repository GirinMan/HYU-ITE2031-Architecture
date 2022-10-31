    lw 0 1 six      Error: undefined label six
    lw 1 2 3
start add 1 2 1
    beq 0 1 2
    beq 0 0 start
    noop
done halt
five .fill 5
five .fill 5        Error: duplicate label five
1neg .fill -1       Error: a label must start with a letter
stAddress .fill start  Error: label length is to long(> 6)
