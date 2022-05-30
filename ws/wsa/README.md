# The Lime Whitespace assembly language


## Keywords
push (n)         - Pushes number to top of the stack

dupe / dup       - Duplicates the number on the top of the stack

copy (n)         - copies the number 'n' positions below the top of the stack and pushes it to the top of the stack

swap             - Swaps the top 2 elements on the stack

drop             - Discards the top element on the stack

slide (n)        - Discards n numbers of elements from the top of the stack, except the very top of the stack

add              - Pops the two elements on the top of the stack and pushes the sum

sub              - Pops the two elements on the top of the stack and pushes the difference (Order: pop a, pop b, push b+a)

mul              - Pops the two elements on the top of the stack and pushes the product

div              - Pops the two elements on the top of the stack and pushes the quotient (Order: pop a, pop b, push b/a)

mod              - Pops the two elements on the top of the stack and pushes the modulus (Order: pop a, pop b, push b/a)

store            - Stores the value at the top of the stack in the heap position provided by the value at the second top of the stack (pop value, pop index, heap[index] = value)

fetch / retrieve - Fetches the number at heap position provided by the top of the stack and puts it on the top of the stack

call (label)     - Jumps to the label and stores the instruction pointer and jumps back in the case of encountering the ret (return) keyword

jmp (label)      - Jumps to the label unconditionally, does not store instruction pointer

jz (label)       - Jumps to the label if the value at the top of the stack is zero

jn (label)       - Jumps to the label if the value at the top of the stack is negative

ret              - Returns to the last call address

end              - Exits the program

printc           - Prints the value at the top of the stack as its ascii character

printi           - Prints the value at the top of the stack as an integer

readc            - Reads the stdin as characters and puts its ascii value at the heap index provided by the top of the stack

readi            - Reads the stdin as an integer and puts its value at the heap index provided by the top of the STACK


## Labels
To define a label you have to start with a . (dot / period) and then its name, followed by a : (colon). (Example: .example:)

To jump to a label you have to call, jmp, jz, or jn followed by a . (dot / period) and the labelname. (Example: jmp .example)


## Numbers

### Decimal
Examples:
>13

>42069

### Hexadecimal
Prefix with '0x'.
Examples:
>0x0d   -> (Decimal) 13

>0x4a55 -> (Decimal) 42069

### Character representation
Enclose with '
Examples:
>'A' -> (Decimal) 65

>'z' -> (Decimal) 122

#### Escape characters
Escape with '\\'
Examples that are implemented:
>'\t' -> (Decimal) 9

>'\n' -> (Decimal) 10

>'\\'' -> (Decimal) 39

## Macros
Start a macro declaration with the keword 'MACRO'.
Directly after declare the macro identifyer.
Declare macro contents between brackets '[ (Contents) ]'

Use the identifyer to "paste" the macro

Example:
```code
macro say_hi 
[ 
    push 'H' printc
    push 'i' printc
    push '!' printc
]

say_hi
end
```

## Comments
### Singleline comments
```code
// This is a comment on a single line (Ignores until newline)
;  This is another comment on a single line (Ignores until newline)
```

### Multiline comment
```code
/*
    This whole comment will not
    be parsed as Whitespace assembly
*/
```
