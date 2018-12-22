;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; HelloWorld.bb;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; By Maneesh Sethi;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This program prints "Hello World to the screen.;;;;;
; There are no input variables required;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;VARIABLES
;The hello string to be printed on the screen
hellostr$ = "Hello World!" 
;END VARIABLES

;MAIN PROGRAM
;Pass "hellostr$" to PrintString function
PrintString(hellostr$) 

;Wait for five seconds before closing
Delay 5000

;END MAIN PROGRAM

;FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Function PrintString(str$);;;;;;;;;;;
;This function prints the variable str$
;Parameters: str$ - the string to be printed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Function PrintString(strng$)
;Print str$ on screen
Print strng$ 
End Function
;END FUNCTIONS