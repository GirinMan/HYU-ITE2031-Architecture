    lw 0 1 six     load reg1 with 6 (symbolic address)
    lw 1 2 2        load reg2 with -2 (numeric address)
start add 1 2 1     decrement reg1 by 2
    beq 0 1 2       if reg1 == 0 goto done 
    noop
    beq 0 0 start   go back to the loop
done halt
six .fill 6
minus .fill -2
addr1 .fill start
