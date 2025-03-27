# Basic CPU Info
- 4 registers: (0-2: User-Accessible registers, 3: conventional return register)
- pc register

# Terminology
**reg(a)**: "Value at register (a)"

**pc**: value of program counter

**mem[a]**: Value at memory address (a)

# HIS I (Hydrogen Instruction Set Version 1)
## Format
`|(1)[0]: opcode|(4)[1-4]: x|(4)[5-8]: y|`
## Instructions
### SETID 0
0. **nul**: NULL
1. **add**: reg[1] = reg[1] + reg[2]
2. **flp**: reg[1] = ~reg[1]
3. **or**:  reg[1] = reg[1] || reg[2]
4. **and**: reg[1] = reg[1] && reg[2]
5. **sl**:  reg[1] = reg[1] << reg[2]
6. **sr**:  reg[1] = reg[1] >> reg[2]
7. **lt**: reg[1] = (reg[1] < reg[2])
8. **jmp**: pc = reg[2]
9. **cjp**: if(reg[1]) pc = reg[2]
10. **set**: reg[x] = y
11. **mv**: reg[y] = reg[x]
12. **ld**: reg[1] = mem[x]
13. **str**: mem[x] = reg[1]
14. **hlt**: HALT