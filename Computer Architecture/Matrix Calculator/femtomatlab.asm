# FemtoMatlab by Nicolas Avrutin, Andrew Apollonsky, and Benjamin Schenker
# ECE151, Prof. Barenbaum, Spring 2012

        	.data
blah:    	.space 1024
symtab:     	.space 2048         # even numbered words are keys
                	            # odd numbered wirds are addrs
symtablen:  	.word 0
line:       	.space 512          # temporary storage for input


Exit:            .asciiz "Exiting Program\n"
Nodata:          .asciiz "no data entered\n"
Toolong:         .asciiz "variable is too many characters long\n" 
Secvar:          .asciiz "second variable not yet defined\n" 
Firvar:          .asciiz "first variable not yet defined\n" 
Newline:         .asciiz "\n"
Tab:             .asciiz "\t"
Printerror:      .asciiz "Error! Invalid Data.\n"
Matrixdimensionerror:     .asciiz "Error! Matrix Dimension Mismatch. \n"
Overflowerror:   .asciiz "Error! Overflow \n"
InvalidNum:      .asciiz "Invalid Number\n"

        .text
        .globl main
# Initializes 3 special variables in symbol table (Id=0,1,2 respectively) and calls parse_line
# The main function Initializes three special variables in sym table that are required for
# matrix multiplication. These special variables are only used internally and cannot be easily
# displayed. (Technically their variable names are the ASCII values of 0x00000001 (three null
# bytes followed by an ASCII one), 0x00000002, and 0x00000003 respectively). It then calls
# parse_line which does all of the line parsing and actually acts as the real Main function.
main:
        #initialize 3 special variables in sym table   (Id=0,1,2 respectively)
        la      $t1, symtab
        la      $t2, symtablen      
        li      $t0, 1           #initializing spec1, 0(1, addr1,0,0)
        sw      $t0, 0($t1)      
        li      $v0, 9
        li      $a0, 256
        syscall
        sw      $v0, 4($t1)
        sw      $0, 8($t1)
        sw      $0, 12($t1)
        li      $t3, 16  
        sw      $t3, 0($t2)

        li      $t0, 2
        sw      $t0, 16($t1)    #initializing spec2, 0(2, addr2,0,0)
        li      $v0, 9
        li      $a0, 256
        syscall
        sw      $v0, 20($t1)
        sw      $0, 24($t1)
        sw      $0, 28($t1)
        addi    $t3, $t3, 16  
        sw      $t3, 0($t2)

        li      $t0, 3  
        sw      $t0, 32($t1) 	#initializing spec3, 0(3, addr3,0,0)
        li      $v0, 9
        li      $a0, 256
        syscall
        sw      $v0, 36($t1)
        sw      $0, 40($t1)
        sw      $0, 44($t1)
        addi    $t3, $t3, 16 
        sw      $t3, 0($t2)

# Main line parsing function.
# parse_line grabs a line from the terminal (entered by the user) and allocates memory for variables
# that are to be assigned.  It then checks to see what kind of assignment is being given to the new
# variable.  Either it is a print call (variable name followed by a space and then a non equal sign,
# so "a print" will work but so will "a p" or "a @"), an explicit assignment (meaning the value of
# the new variable is written explicitly on the line, like in "a = 1, 1, 1"), or an implicit
# assignment (meaning the value of the new variable is the result of an operation on one or two
# other variables stored in memory. Finally, control is handed over to the appropriate function,
# which returns or jumps back to parse_line upon completion.
parse_line:
        addi    $sp, $sp, -8
        sw      $s0, 0($sp) # $s0 will hold the variable name
        sw      $s1, 4($sp) # $s1 will contain the results of findnextspace (meaning it is the length of the variable name)

                            #Read line from user into a temporary variable (line)
        li      $v0, 8
        la      $a0, line
        li      $a1, 512
        syscall
        #begin parsing
        li      $a0, 0
        jal     find_next_space
        li      $t0, 1
        beq     $v1, $t0, parse_line_nodata    #no data entered
        slti    $t0, $v0, 5
        beq     $t0, $0, parse_line_toolong    #first variable too many characters long
        
        #load name of variable (need to load proper amount of characters)
        la      $t1, line
        lw      $t2, 0($t1)
        move    $s1, $v0
        li      $a3, 4            	#$a3 as a temp register
        sub     $t3, $a3, $s1           #number of bytes to shift right  
        sll     $t3, $t3, 3            	#multiply by eight (bits per byte)
        sllv    $s0, $t2, $t3          	#trim off extra bits and now $s0 holds the variable name
        move    $a0, $s0
        jal     find_var               	#check to see if variable exists already
        #continue parsing
        la      $t1, line
        addi    $s1, $s1, 1             # $s1 now refers to the index of the element after the space after the variable name
        add     $t2, $s1, $t1
        lb      $t2, 0($t2)
        li      $a0, 61                 #just using it as spare variable
        beq     $t2, $a0, parse_line_cont    #check for "=" after variable (technically 2 characters after the variable name)
        move    $a0, $v0
        jal     print                   #if there is no equal sign, it must be a print statement so we jump there
     
        lw      $s0, 0($sp)
        lw      $s1, 4($sp)
        addi    $sp, $sp, 8
        j       parse_line              #start all over        
parse_line_cont:
        li      $t4, -1                 # with an equal sign after the variable name we must now check to see if
					# the variable exists already (just a branch here, findvar was called earlier)
        bne     $v0, $t4, parse_line_cont2   #if variable exists we skip to more parsing, else we create the
					# variable on the stack and make room in memory
        la      $t4, symtablen
        lw      $t5, 0($t4)
        la      $a3, symtab            	#temp var
        add     $t5, $t5, $a3
        sw      $s0, 0($t5)            	#storing variable id
        li      $v0, 9
        li      $a0, 256
        syscall
        sw      $v0, 4($t5)
        move    $a0, $t5                # the address of this new variable is neccesary below to call the assignment
					# function so it is stored in the proper argument register
        addi    $t5, $t5, 16
        sub     $t5, $t5, $a3
        sw      $t5, 0($t4)             #update endpointer of symble table to include the new entry
parse_line_cont2:
                                        #we continue parsing splitting into explicit and implicit assignment types,
					#explicit is called when the values to be assigned are 
                                        #explicitly written as in a = 1 2 3, 4 5 6, 7 8 9. implicit is called when
					#matrix operations must be performed before assignment like b = a * c 
        addi    $s1, $s1, 2             #s1 now holds the index of the character after the space after the equal sign
        add     $a1, $s1, $t1           #using $a1 as temp register
        lb      $t2, 0($a1)
        move    $a1, $s1                #the other argument for the assignment functions is set here
                                        #checking if the next character is a minus sign or a number (indicating an
					#explicit assignment)
        li      $t4, 45
        beq     $t2, $t4, parse_line_assign_e
        addi    $t4, $0, 47
        slt     $t5, $t4, $t2
        slti    $t6, $t2, 58
        and     $t7, $t5, $t6
        bne     $t7, $0, parse_line_assign_e        
parse_line_assign_i:        
        lw      $s0, 0($sp)
        lw      $s1, 4($sp)
        addi    $sp, $sp, 8
        jal     assign_implicit
        j       parse_line
parse_line_assign_e:
        lw      $s0, 0($sp)
        lw      $s1, 4($sp)
        addi    $sp, $sp, 8
        jal     assign_explicit
        j       parse_line
parse_line_nodata:
        la      $a0, Nodata
        li      $v0, 4
        syscall
        j    	parse_line
parse_line_toolong:
        la      $a0, Toolong
        li      $v0, 4
        syscall
        j     	exit_program


# Assign_explicit is called by parse_line once it has determined that the variable is
# going to be explicitly assigned a value.  It stores each element at the proper location
#in memory corresponding to the variable to be assigned and checks for proper dimensions.  

#Assign_explicit:
#Argument: $a0 - index of the variable, $a1 - current position in line buffer
#initialize registers for m to 1, n and ntemp to 0
#copy address of variable to register 

#This function uses the following algorithm to check for proper dimensions while properly assigning the elements.   
#call find_next_space on current position
#read number of bytes until next space into temp register
#check if last read character is “,”
#	if not 
#call Atoi on read data
#		increment register holding ntemp
#		store int at address of variable
#		increment register holding address by 4
#		check if newline-p 
#			jump to exit if true
#Loop back to call
#	else
#		increment m
#		if m=2 copy ntemp into n
#		else
#			check if ntemp==n and if so, reset ntemp to zero
#				else return “invalid matrix dimensions”
#		call atoi on read data minus last byte
#		store int at address of variable
#		increment register holding address by 4
#		Loop back to call
#exit: check if ntemp==n and if so,
#	else return “invalid matrix dimensions”
#store m and n in symtab

# Arguments:
#   $a0 - address of the variable in symtab
#   $a1 - current position in line buffer (minus sign or beginning of first int)
# Registers:
#   $s0 - address of current position in line
#   $s1 - address of symtab
#   $s2 - address of variable's data
#   $s3 - address of line
#   $t3 - m
#   $t4 - n
#   $t5 - ntemp
#   $t6 - address of variable in symtab
#   
assign_explicit:
        addi    $sp, $sp, -20
        sw      $ra, 0($sp)
        sw      $s0, 4($sp)    		#current pos in line buff
        sw      $s1 8($sp)
        sw      $s2 12($sp)
        sw      $s3 16($sp)
        la      $s3, line
        add     $s0, $s3, $a1
        addi    $t3, $0, 1      	#m
        move    $t4, $0         	#n
        move    $t5, $0         	#ntemp
        move    $t6, $a0      		#address of variable in symtab
        lw      $s2, 4($t6)
        la      $s1, symtab
assign_explicit_loop:
        move    $t7, $a1      		#$t7 is index in line buffer, $t9 is offset (on line) till nextspace
        move    $a0, $t7      		#set argument for find_next_space
        add     $t7, $t7, $s3      	# convert offset to address
        jal     find_next_space
        add     $t9, $v0, $t7  		# t9 holds address of next space
        addi    $a3, $t9, -1   		# address of char before next space (potential comma)
        lb      $t2, 0($a3)
        li      $a2, 44          	#using $a3 as temp register
        addi    $t5, $t5, 1
        beq     $t2, $a2, assign_explicit_comma   # $a3 holds address of comma
        # if no comma, call atoi on data: upper bound = $t9, lower bound  $t7
        move    $a0, $t7
        move    $a1, $t9
assign_explicit_loop_comma_reentry:
        jal     atoi            	#returns int in $v0
        sw      $v0, 0($s2)
        addi    $s2, $s2, 4
        li      $a2, 1
        beq     $v1, $a2, assign_explicit_exit
        addi    $s0, $t9, 1             # set new pos in line pas next space
        la      $a1, line
        sub     $a1, $s0, $a1
        j       assign_explicit_loop
assign_explicit_comma:
        add     $t3, $t3, 1
        li      $a2, 2
        beq     $t3, $a2, assign_explicit_comma2
        beq     $t4, $t5, assign_explicit_comma3
        j       assign_explicit_error
assign_explicit_comma2:
        move    $t4, $t5
assign_explicit_comma3:
        move    $t5, $0
        move    $a0, $t7
        move    $a1, $a3        	# only difference
        j       assign_explicit_loop_comma_reentry

assign_explicit_exit:
        li      $a3, 1
        beq     $t3, $a3, assign_explicit_exit_2   # if m == 1: matrix dim error not possible
        bne     $t5, $t4, assign_explicit_error    # matrix dimension check
assign_explicit_exit_2:
        sw      $t3, 8($t6)    		#store m in symtab
        move    $t4, $t5
        sw      $t4, 12($t6)   		#store n in symtab
        lw      $ra, 0($sp)
        lw      $s0, 4($sp)    		#current pos in line buff
        lw      $s1 8($sp)
        lw      $s2 12($sp)
        lw      $s3 16($sp)
        addi    $sp, $sp, 20
        jal     $ra
assign_explicit_error: 			#print invalid matrix dimensions and exit safely
        la      $a0, Matrixdimensionerror
        li      $v0, 4
        syscall
        j       parse_line

# assign_implicit

#Input arguments: $a0 - symbol table address of variable to be written
#$a1 - current position in line buffer
#Return Values: None
#Local variables: $t0-9, $s0-9, $v0-2, $a0-3

# Assign_implicit is called to determine the proper operation that needs to be done on previously stored
# variables.  It does this simply by checking the ascii value of the operatior and jumping to that
# operation function after doing some checks and setting the proper arguments.  
#Arguments: 
# $a0 - address of the variable in symtab
# $a1 - current position in line buffer
# #cant use $t0-$t2 because findnextspace uses them
assign_implicit:              
        addi    $sp, $sp, -24
        sw      $s2, 0($sp) 		# address of output variable in symtab
        sw      $s3, 4($sp) 		# index in line buffer
        sw      $s4, 8($sp) 		# holds variable name
        sw      $s5, 12($sp) 		# holds 1st variable addrs
        sw      $s6, 16($sp) 		# holds operation
        sw      $s7, 20($sp) 		# tmp

        move    $s2, $a0
        move    $s3, $a1
        move    $a0, $a1
        jal     find_next_space

        #load name of variable (need to load proper number of characters)
    	move    $s4, $0        
    	la      $t0, line
        add     $t6, $t0, $s3           #$t6 contains address of the beginning of the current position in buffer
        # read first var
        li      $a3, 4
        sub     $a3, $a3, $v0
        sll     $a3, $a3, 3
        add     $v0, $v0, $t6           # $v0 contains address of next space
assign_implicit_loop1:
        lb      $s7, 0($t6)
        sllv    $s7, $s7, $a3
        add     $s4, $s4, $s7
        addi    $a3, $a3, 8
        addi    $t6, $t6, 1
        bne     $t6, $v0, assign_implicit_loop1

        li      $a3, 1                  #using $a as a temp register
        beq     $v1, $a3, Assign_implicit_copy

        addi    $v0, $v0, 1             #$v0++
        move    $t6, $v0                #update line buffer index now points to operation
    	sub     $a0, $t6, $t0
    	jal     find_next_space
        
    	li      $a3, 1                	#using $a as a temp register
        beq     $v1, $a3, Assign_implicit_jumptable_unary  # if newline-p, then unary operation
                                                           # operator in $s6, var symtab addr in $s5

        move    $a0, $s4
        jal     find_var                #check to see if first variable exists already
        li      $a3, -1
        beq     $v0, $a3, Assign_implicit_firvar #if 1st var hasn't been assigned, syntax error
        move    $s5, $v0                # address of first var in symtab

        lb      $s6, 0($t6)             #grab operator and stick in in $s6
        addi    $t6, $t6, 2             # $t6 now pointing to first char of second var
        sub     $s3, $t6, $t0           # s3 now contains updated index into line
        move    $a0, $s3
        jal     find_next_space
        # read second var
        move    $a0, $0
        li      $a3, 4
        sub     $a3, $a3, $v0
        sll     $a3, $a3, 3
        add     $v0, $v0, $t6           # $v0 contains address of next space
assign_implicit_loop2:
        lb      $s7, 0($t6)
        sllv    $s7, $s7, $a3
        add     $a0, $a0, $s7           # accumulator for var2 name
        addi    $a3, $a3, 8
        addi    $t6, $t6, 1
        bne     $t6, $v0, assign_implicit_loop2

        jal     find_var
        li      $a3, -1                 #using $a3 as temp register
        beq     $v0, $a3, Assign_implicit_secvar #if second var hasn't been assigned, syntax error

        move    $a1, $v0               	#$a0 is the address in symtable of 1st operand,
                                    	#$a1 is the address in symtable of 2nd operand
                                    	#$a2 has address of the output variable in symtable
        move    $a0, $s5 
        move    $a2, $s2

        move    $t1, $s6        	#saving operation so can restore s registers

        lw      $s2, 0($sp) 		# address of output variable in symtab
        lw      $s3, 4($sp) 		# index in line buffer
        lw      $s4, 8($sp) 		# holds variable name
        lw      $s5, 12($sp) 		# holds 1st variable addrs
        lw      $s6, 16($sp) 		#holds operation
        lw      $s7, 20($sp) 		# tmp
        addi    $sp, $sp, 24
Assign_implicit_jumptable:
        li      $t0, 42
        beq     $t1,$t0, multiply
        
        li      $t0, 43
        beq     $t1,$t0, addition        
        
        li      $t0, 45
        beq     $t1,$t0, subtract
        
        j       parse_line            	#invalid operation       
Assign_implicit_jumptable_unary:

   	move    $t1, $s6        	#saving operation so can restore s registers

        lw      $s2, 0($sp) 		# address of output variable in symtab
        lw      $s3, 4($sp) 		# index in line buffer
        lw      $s4, 8($sp) 		# holds variable name
        lw      $s5, 12($sp) 		# holds 1st variable addrs
        lw      $s6, 16($sp) 		# holds operation
        lw      $s7, 20($sp) 		# holds operation
        addi    $sp, $sp, 24


        li      $t0, 39
        beq     $t1,$t0, transpose
        
        j       parse_line            	#invalid operation        
Assign_implicit_copy:
        lw      $t0, 4($s2)           	#$t0 has addres of output variable in memory
        lw      $t1, 4($s5)           	#$t1 has addres of input variable in memory
        lw      $t2, 8($s5)             #$t2 has dim m of input variable
        sw      $t2, 8($s2)
        lw      $t3, 12($s5)            #$t3 has dim n of input variable
        sw      $t3, 12($s2)        
        mult    $t2, $t3
        mflo    $t4                     #total #of words to copy
        
        move    $t5, $0
Assign_implicit_copy_loop:
        sll     $t7, $t5,4              #multiply loop var by 4
        add     $t8, $t1, $t7           #$t8 is address to load from
        add     $t9, $t2, $t7           #$t9 is address to store
        lw      $t6, 0($t8)
        sw      $t6, 0($t9)
        addi    $t5, $t5, 1             # $t5++
        bne     $t5, $t4, Assign_implicit_copy_loop
        
        j       parse_line
Assign_implicit_firvar:
        la      $a0, Firvar
        li      $v0, 4
        syscall
    	j       parse_line
Assign_implicit_secvar:
        la      $a0, Secvar
        li      $v0, 4
        syscall
    	j     parse_line

################################
##   Find Next Space
######
# Acccepts an index into line and returns the index of the next space or newline
# Arguments:
#     $a0 - current position in the string
# Return Values: 
#     $v0 - offset to next space
#     $v1 - newline -p (1 if it is a newline, not a space)
# Local variables: $s0-2 and $a1-3 
# find_next_space iterates through characters starting at the given beginning position,
# comparing each to the ascii values for space and newline. Once it has found an appropriate
# character, the position offset between this and $a0 is calculated and returned in $v0,
# along with either 0 or 1 in $v1 that is determined by the actual character found. Old
# $s0-2’s are saved before the function does anything to them onto the stack, and retrieved
# afterwards. 

find_next_space:
        addi    $sp, $sp, -12
        sw      $s0, 0($sp)
        sw      $s1, 4($sp)
        sw      $s2, 8($sp)

        la      $a3, line
        move    $s2, $a0
        move    $v0, $0
        move    $v1, $0
find_next_space_loop:
        add     $s0, $a3, $a0
        lb      $s1, 0($s0)
        li      $a2, 32   	        #$a2 used as temp register
        beq     $s1, $a2, find_next_space_found
        li      $a2, 10               	#$a2 used as temp register
        beq     $s1, $a2, find_next_space_newline

        addi    $a0, $a0, 1
        j       find_next_space_loop
find_next_space_found:                  #might not need this but keeping for now
        sub     $v0, $a0, $s2
        j    find_next_space_exit
find_next_space_newline:
        addi    $v1, $v1, 1
        sub     $v0, $a0, $s2
find_next_space_exit:
        lw      $s0, 0($sp)
        lw      $s1, 4($sp)
        lw      $s2, 8($sp)
        addi    $sp, $sp, 12
        jr      $ra        
        
################################
##   Print Matrix
######
# Input arguments: $a0 - address in the symbol table of the variable that will be displayed.
# Return Values: None. 
# Local variables: $a1, $t2-8

# print takes the address in the symbol table of $a0, uses it to go to the area in memory where
# the actual matrix is stored (because the symbol table stores an address of the data, not the
# data itself), and iterates through the data. It prints out individual numbers in a row with
# tabs between them, printing a newline character whenever an internal counter signals that the
# displayed row length is equal to the actual row length.    
 
print:
        li      $a1, -1        		#$a1 as temp register        
        beq     $a0, $a1, print_error
        move    $t1, $a0    		#$t1 has address of variable in symtab
        lw      $t7, 4($t1)    		#$t7 has address of variable in memory
        lw      $t2, 8($t1)    		#$t2 is m
        lw      $t3, 12($t1)    	#$t3 is n


        li      $t4, 0      		# Iterator along rows
        li      $t6, 0        		# $t6 is offset
        li      $t5, 0      		# Iterator along columns

print_loop:    				#Iterates through columns and rows
        add     $t8, $t6, $t7
        lw      $a0, 0($t8) 		# $a0 contains value to be printed
        li      $v0, 1
        syscall
        la      $a0, Tab    		#prints tab
        li      $v0, 4
        syscall

        addi    $t5, $t5, 1        	#increments column iterator
        addi    $t6, $t6, 4

        bne     $t5, $t3, print_loop 

        la      $a0, Newline            #prints newline
        li      $v0, 4
        syscall

        addi    $t4, $t4, 1        	#increments row iterator
        move    $t5, $0
        bne     $t4, $t2, print_loop

        jr      $ra       

print_error:
        la      $a0, Printerror
        li      $v0, 4
        syscall
        li      $v0, -1
        jr      $ra
        
################################
##   Find Variable
######
# $v0 returns index of word $a0 in symtab
# Arguments:
#     $a0 - id in symtab (name of variable)
# Return Values: 
#     $v0 - address of the symtab entry of variable with name $a0, or -1 if the variable cannot
#           be found. 
# Local variables: $s0-2 and $a1-3
#
# find_var starts at the beginning of the symbol table, and iterates through, searching for the
# proper variable. Getting to the next variable requires an offset of 16 (4 for each name,
# address, m-dimension, n-dimension). Old $s0-2’s are saved before the function does anything
# to them onto the stack, and retrieved afterwards. 

find_var:
        addi    $sp, $sp, -12
        sw      $s0, 0($sp)
        sw      $s1, 4($sp)
        sw      $s2, 8($sp)
        la      $a2, symtab
        la      $a3, symtablen
        lw      $a3, 0($a3)
        add     $a3, $a3, $a2
        move    $s2, $a0
        move    $s1, $a2        	#iterator registor
find_var_loop:
        lw      $s0, 0($a2)          	# $s0 contains the id of the variable being examined
        beq     $s2, $s0, find_var_found
        beq     $a2, $a3, find_var_notfound
        addi    $a2, $a2, 16
        j       find_var_loop
find_var_found:
        move    $v0, $a2
        j       find_var_exit
find_var_notfound:
        li    	$v0, -1
find_var_exit:
        lw      $s0, 0($sp)
        lw      $s1, 4($sp)
        lw      $s2, 8($sp)
        addi    $sp, $sp, 12
        jr    	$ra
        
################################
##   String to Integer
######
#    Argument: $a0 - address to start, $a1- address of next non-numeric character      
# Registers:
#       $s7 - tmp
atoi:
        addi    $sp, $sp, -8
        sw      $s1, 0($sp)
        sw      $s7, 4($sp)
        move    $v0, $0              	# init ret val to 0  
        li      $a3, 1            	#sign register   
        lb      $s1, 0($a0)            	# fetch first byte
        li      $a2, 45
        li      $s7, 10
        beq     $s1, $a2, atoi_neg    	#assuming positive from here on
        j       atoi_loop2
atoi_loop:
        addi    $a0, $a0, 1
atoi_loop2:
        beq     $a0, $a1, atoi_exit
        lb      $s1, 0($a0)            	# fetch current byte
        blt     $s1, 48, atoi_error    	#not a valid number
        bgt     $s1, 57, atoi_error
        mult    $v0, $s7
        mflo    $v0
        addi    $s1, $s1, -48
        add     $v0, $v0, $s1
        j       atoi_loop
atoi_neg:
        li      $a3, -1
        j       atoi_loop
atoi_error:
        la      $a0, InvalidNum
        li      $v0, 4
        syscall

atoi_exit:
        mult    $v0, $a3
        mflo    $v0
        lw      $s1, 0($sp)
        lw      $s7, 4($sp)
        addi    $sp, $sp, 8
        jr      $ra


################################
##   Adding Matrices
######
# Input arguments:
#     $a0 - symbol table address of operand one
#     $a1 - symbol table address of operand two
#     $a2 - symbol address of the variable that the answer will be written to. 
# Return Values: None
# Local variables: $t0-9, $s0-3, $s5
#
# The addition function looks at the dimensions of the two matrices, and returns an
# error if there is a mismatch. From there on, we loop through all the positions in
# the output matrix $a2 and insert the sum of the two corresponding values in $a0 and
# $a1. Upon completion, the program jumps to parse_line to continue parsing.

addition:
        lw      $s0, 4($a0)            	#address of input1 in mem
        lw      $s1, 4($a1)            	#address of input2 in mem 
        lw      $s2, 4($a2)            	#address of output in mem
        lw      $t4, 8($a0)             #m1
        lw      $t5, 12($a0)            #n1
        lw      $t6, 8($a1)             #m2
        lw      $t7, 12($a1)            #n2
        
        beq     $t4, $t6, addition_check1       # If m match
        j       addition_dimension_error
        
addition_check1:                                # If n match
        beq     $t5, $t7, addition_process
        j       addition_dimension_error

addition_dimension_error:                       # Dimension Mismatch
        la      $a0, Matrixdimensionerror
        li      $v0, 4
        syscall
        j 	addition_done
        
addition_process:                       #Setup for adding
        sw      $t4, 8($a2)           	#Store m in $a2
        sw      $t5, 12($a2)          	#store n in $a2
        move    $t8, $0                 #doubles iterator            
        mult    $t4, $t5
        mflo    $t6                     #Length of matrix
        li      $t0, 0
        j       addition_loop
        
addition_loop:          
        add     $t1, $s0, $t8           #A0 + offset
        add     $t2, $s1, $t8           #A1 + offset
        lw      $s4, 0($t1)           	#Load value 1
        lw      $s5, 0($t2)           	#Load value2
        add     $t9, $s4, $s5           #Element addition, stored in $t9
        add     $s3, $t8, $s2           #A2 + Offset
        sw      $t9, 0($s3)             #Store 
        addi    $t8, $t8, 4
        addi    $t0, $t0, 1
        beq     $t6, $t0, addition_done
        j       addition_loop

addition_done:
        j       parse_line
 
################################
##   Subtracting Matrices
######
# Input arguments:
#     $a0 - symbol table address of operand one
#     $a1 - the symbol table address of operand two
#     $a2 - the symbol address of the variable that the answer will be written to
# Return Values: None
# Local variables: $t0-9,  $s0-3, $s5
#
# The subtract function looks at the dimensions of the two matrices, and returns an error
# if there is a mismatch. From there on, we loop through all the positions in the output
# matrix $a2 and insert the difference of the corresponding values in $a0 and $a1 ($a0 - $a1).
# Upon completion, the program jumps to parse_line to continue parsing.

subtract:
        lw      $s0, 4($a0)            	#address of input1 in mem
        lw      $s1, 4($a1)            	#address of input2 in mem 
        lw      $s2, 4($a2)            	#address of output in mem
        lw      $t4, 8($a0)             #m1
        lw      $t5, 12($a0)            #n1
        lw      $t6, 8($a1)             #m2
        lw      $t7, 12($a1)            #n2
        
        beq     $t4, $t6, subtract_check1       # If m match
        j       subtract_dimension_error
        
subtract_check1:                                # If n match
        beq     $t5, $t7, subtract_process
        j       subtract_dimension_error

subtract_dimension_error:                       # Dimension Mismatch
        la      $a0, Matrixdimensionerror
        li      $v0, 4
        syscall
        j subtract_done
        
subtract_process:                       #Setup for adding
        sw      $t4, 8($a2)           	#Store m in $a2
        sw      $t5, 12($a2)          	#store n in $a2
        move    $t8, $0                 #doubles iterator            
        mult    $t4, $t5
        mflo    $t6                     #Length of matrix
        li      $t0, 0
        j       subtract_loop
        
subtract_loop:          
        add     $t1, $s0, $t8           #A0 + offset
        add     $t2, $s1, $t8           #A1 + offset
        lw      $s4, 0($t1)           	#Load value 1
        lw      $s5, 0($t2)           	#Load value2
        sub     $t9, $s4, $s5           #Element subtraction, stored in $t9
        add     $s3, $t8, $s2           #A2 + Offset
        sw      $t9, 0($s3)             #Store 
        addi    $t8, $t8, 4
        addi    $t0, $t0, 1
        beq     $t6, $t0, subtract_done
        j       subtract_loop

subtract_done:
        j       parse_line
        
        

################################
##   Multiplication Catagorization
######
# Input arguments: $a0 - symbol table address of operand one
# $a1 - symbol table address of operand two
# $a2 - holds the symbol address of the variable that the answer will be written to. 
# Return Values: None
# Local variables: $t0-9, $s0-9, $v0-2, $a0-3. 

# The multiply function is subdivided into several parts. The initial part looks at
# the dimensions of the two matrices, and decides whether to do a 1x1 * [mxn] multiply against
# a scalar or a [m1xn1] * [m2xn2] of two matrices. The scalar x matrix case runs through the
# matrix, and multiplies every value by the scalar. The matrix x matrix case runs through the
# dimensions of the output matrix, and for every single one, it uses a dot multiply subroutine
# to calculate to the proper number to insert. $s0-5 are used by the dot product function which
# is utilized by the matrix multiplication function.

 
multiply:   
        lw      $t4, 8($a0)                      #m1
        lw      $t5, 12($a0)                     #n1
        lw      $t6, 8($a1)                      #m2
        lw      $t7, 12($a1)                     #n2
        li      $t2, 1
        li      $t8, 0
        li      $t9, 0
        add     $t8, $t4, $t5                    #a=n1 + m1
        beq     $t8, 2, multiply_ais2            #If a0 is a 1x1 matrix, jump
        move    $t8, $0
multiply_continue1:
        add     $t9, $t6, $t7                    #b = m2 + m2
        beq     $t9, 2, multiply_bis2            #If a1 is a 1x1 matrix, jump
        move    $t9, $0
multiply_continue2:
        xor     $t3, $t8, $t9                    #For calling dot multiply
        beq     $t3, $t2, dot_multiply           #If the xor returns 1, multiply the two matrices elementwise.
        beq     $t5, $t6, mat_multiply           #If n1 = m2, matrix multiplication
        la      $a0, Matrixdimensionerror
        li      $v0, 4
        syscall
        j       parse_line
        
multiply_ais2:
        li      $a3, 0
        li      $t8, 1
        j       multiply_continue1
multiply_bis2:
        li      $a3, 1
        li      $t9, 1
        j       multiply_continue2

################################
##   Matrix Multiplying
######

# mat_multply
# Arguments:
#        $a0 - address of symtab entry of operand 1 (A)
#        $a1 - address of symtab entry of operand 2 (B)
#        $a2 - address of symtab entry of output var (C)
# Return Values:
# Registers used:
#        $s0 - $s5 used in dot_product
#        $a0
mat_multiply:
    	# setting dimensions
        lw      $t0, 8($a0)      	# load m1 into $t0
        lw      $t1, 12($a0)     	# load n1 into $t1
        lw      $t2, 8($a1)      	# load m2 into $t2
        lw      $t3, 12($a1)     	# load n2 into $t3
        sw      $t0, 8($a2)      	# store m1 as m of output var
        sw      $t3, 12($a2)     	# store n2 as n of output var


        li      $t4, 0          	# counting var j (cols of B)
        li      $t5, 0           	# counting var i (rows of A)
        
    	lw      $s6, 4($a0)    		#$s6 holds address of var 1 in mem
    	lw      $s7, 4($a1)    		#$s7 holds address of var 2 in mem
    	lw      $s8, 4($a2)    		#$s8 holds address of var 3 in mem

	lw      $v1, 4($a2)    		#Linear iterator through return matrix
   	la      $a3, symtab
mat_multiply_loop_thru_rows:    
        move    $t4, $0
mat_multiply_loop_thru_columns:
    	j       mat_multiply_row_generate
mat_multiply_row_generated:    
  	j       mat_multiply_column_generate
mat_multiply_column_generated:
#setting arguments for dot_product sending in actual memory addresses of spec1, spec2 and spec3
	lw      $s4, 4($a3)
    	lw      $s5, 20($a3)
    	lw      $a2, 36($a3)
    	jal     dot_product            	#Perform the dot product
        lw 	$s2, 0($a2)
        mult    $t5, $t3
        mflo    $s3
        add     $s3, $s3, $t4
        sll     $s3, $s3, 2
        add     $s3, $s3, $s8
        sw      $s2, 0($s3)
        addi    $v1, $v1, 4
    	addi    $t4, $t4, 1
   	bne     $t4, $t3, mat_multiply_loop_thru_columns
        addi 	$t5, $t5, 1
        bne 	$t5, $t0, mat_multiply_loop_thru_rows

        j 	parse_line

mat_multiply_row_generate:          	#Breaks and uses $t6, $t7, $v0
    	mult   	$t5, $t1            	# Row iterator x Value/row in 1
    	mflo   	$t6                	# Offset in $a0 for row.
    	sll    	$t6, $t6, 2
    	add    	$t6, $s6, $t6    	# $t6 is address of current element in memory
    	li    	$t7, 0            	# Iterator through row to add in.
    	lw    	$t8, 4($a3)        	#$t8 has address of spec1 in memory
mat_multiply_row_generate_loop:
    	lw    	$v0, 0($t6)        	#kth element of 1st var in memory
    	sw    	$v0, 0($t8)

   	addi   	$t6, $t6, 4        	#Iterator through a0 address shift
    	addi   	$t8, $t8, 4

    	addi    $t7, $t7, 1          	#Iterator for finish-comparison shift
    	beq    	$t7, $t1, mat_multiply_row_generated
    	j    	mat_multiply_row_generate_loop

mat_multiply_column_generate:    	#Breaks and uses $t6, $t7, $v0, $a3
    	sll    	$t6, $t3, 2             #Offset for next value in a1. Iterates.
    	sll    	$t7, $t4, 2     	#Horizontal offset for first value
    	add    	$t7, $t7, $s7    	#Address for 1st value
    	li    	$a1, 0
    	lw    	$t9, 20($a3)        	#$t9 is addres of spec2 in mem    
mat_multiply_column_generate_loop:
    	lw    	$v0, 0($t7)        	#Store stuff
    	sw    	$v0, 0($t9)
    	add   	$t7, $t7, $t6    	# increment address of next element in variable 2
    	addi   	$t9, $t9, 4    		# increment address of next element in spec2

    	addi   	$a1, $a1, 1        	#Iterator for finish-comparison shift
    	beq    	$a1, $t2, mat_multiply_column_generated
    	j   	mat_multiply_column_generate_loop
        

################################
##   Dot Product
######
# Internal Function. Used by matrix multiplication. Can be externally used by calling matrix
# multiplication on an 1 x m and a m x 1 matrix.
#
# Arguments:
#     $a0 - holds the symtab index of operand 1
#     $a1 - holds symtab index of operand 2
#     $a2 - symtab index of output variable
dot_product:    
        li      $s2, 0                  #accumulator
	li      $s3, 0                	#4x counter for offset

dot_product_loop:
        add     $s1, $s4, $s3           #Get address for 1st vector
        lw      $s1, 0($s1)             #Load value for 1st vector
        add     $s0, $s5, $s3           #Get address for 2nd vector
        lw      $s0, 0($s0)             #Load value for 2nd vector
        mult    $s1, $s0                #Multiplying values
    	mfhi    $s1
   	bne     $s1, $0, overflow_error       
    	mflo    $s1                     #Saving them into s1
          
    	add     $s2, $s2, $s1           #Add up the value into the sum
    	addi    $s3, $s3, 4
    
    	srl     $s1, $s3, 2             #Counter / 4 for comparing with length
        beq     $s1, $t1, dot_product_end
        j       dot_product_loop            

dot_product_end:
        sw      $s2, 0($a2)             #Save value
        jr      $ra

################################
##   Dot Multiplying
######

#a0 and a1 are inputs, a2 is output, a3 specifies which is the scalar
#t4 is m1, t5 is n1, t6 is m2, t7 is n2

dot_multiply:
    	li      $t0, 0
    	li      $t6, 0                          #Loop iterator
    	beq     $a3, $t0, dot_multiply_first    # Split up depending on which is the vector, find dimensions
    	j       dot_multiply_second
        
dot_multiply_first:                            	#If a0 = 0, first is scalar
    	lw      $t1, 4($a0)                    
    	lw      $t2, 4($a1)
    	lw      $t3, 8($a1)                    	#m   
    	lw      $t4, 12($a1)                	#n
    	j       dot_multiply_2
    
dot_multiply_second:
    	lw      $t1, 4($a1)
    	lw      $t2, 4($a0)
    	lw      $t3, 8($a0)                    	#m
    	lw      $t4, 12($a0)                	#n
    	j       dot_multiply_2
    
    #At this point, t1 is pointer to scalar, t2 is pointer to matrix, t3 is m of matrix, t4 is n of matrix
dot_multiply_2:
    	sw      $t3, 8($a2)                    	#store m
    	sw      $t4, 12($a2)                	#store n
    	mult    $t3, $t4            
    	mflo    $t5                            	#Total Dimensions             
    	lw      $t0, 4($a2)                    	#Memory address of output
    	li      $t3, 0                        	#Iterator for loop
    	lw      $t7, 0($t1)                    	#Scalar value
    	j 	dot_multiply_loop

dot_multiply_loop:
    	lw      $t8, 0($t2)                    	# Value in matrix
    	mult    $t8, $t7                    	# Multiply
    	mflo    $t9            
    	sw      $t9, 0($t0)                    	# Save stuff.
    	addi    $t3, 1
    	addi    $t0, 4                        	#Increment output position
    	addi    $t2, 4                        	#Increment input matrix position
    	bne     $t3, $t5, dot_multiply_loop     # Check if continue loop
    	j       dot_multiply_fin

dot_multiply_fin:
    	j       parse_line
    

################################
##   Matrix Transpose
##### CURRENTLY NOT FUNCTIONAL#
#a0 is input, $a2 is 'output'

transpose:
    	lw      $t0, 8($a0)                    	#number of rows
    	lw      $t1, 12($a0)                	#number of columns
    	lw      $t2, 4($a0)                    	#Address of the data in a0
    	lw      $t8, 4($a2)                    	#Address of the data in a1
    	sw      $t0, 12($a2)
    	sw      $t1, 8($a2)
    	li      $t3, -1                    	#Iterates through rows
    
transpose_loop_thru_rows:
    	li      $t4, 0                        	#Iterates through columns
   	addi    $t3, 1
transpose_loop_thru_columns:
    	mult    $t4, $t0                    
    	mflo    $t7                         	#Multiplication of new index
    	add     $t7, $t7, $t3                	#Addition of new index
    	mult    $t3, $t1
    	mflo    $s2                            	#Multiplication of old index
    	add     $s2, $s2, $t4                	#Addition of old index
    	sll     $s0, $t7, 2                    	#new matrix offset         
    	sll     $s2, $s2, 2                 	#old matrix offset
    	add     $s0, $s0, $t8                	#Get address for new insertion
    	add     $s2, $s2, $t2                	#Get address for old getting    
    	lw      $t9, 0($s2)
    	sw      $t9, 0($s0)
   	addi    $t4, $t4, 1
   	beq     $t3, $t0, transpose_fin        	#If the iterations done is right, skip to finish
    	beq     $t1, $t4, transpose_loop_thru_rows
    	j 	transpose_loop_thru_columns
    
tranpose_fin:
    	j   	parse_line
    

################################
##   Overflow Error
######        

overflow_error:
  	li    	$v0, 4
    	la      $a0, Overflowerror
    	syscall

# exit_program: safely exits program
exit_program:

        la      $a0, Exit
        li      $v0, 4
        syscall
    
        li    	$v0, 10
       	syscall

