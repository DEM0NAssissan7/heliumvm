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
1. **add**: reg[0] = reg[0] + reg[1]
2. **sub**: reg[0] = reg[0] - reg[1]
3. **or**:  reg[0] = reg[0] || reg[1]
4. **and**: reg[0] = reg[0] && reg[1]
5. **sl**:  reg[0] = reg[0] << reg[1]
6. **sr**:  reg[0] = reg[0] >> reg[1]
7. **lt**: reg[0] = (reg[0] < reg[1])
8. **jmp**: pc = reg[1]
9. **cjp**: if(reg[0]) pc = reg[1]
10. **set**: reg[x] = y
11. **mv**: reg[y] = reg[x]
12. **ld**: reg[y] = mem[x]
13. **str**: mem[x] = reg[0]
14. **hlt**: HALT