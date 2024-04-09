; An AND gate with a counter that counts
; up when AND gate is active or down when not.

; Counts a 16bit number in the main loop.
; Does AND logic in interrupt handler.
; Pins Right and Left triggers interrupts
; if their digital value changes.

; Not a perfect AND gate though, as values
; such as 4 and 8 output 0 instead of 1. because:
; 8:   %1000
; 4:   %0100
; ==========
; AND: %0000
; Feel free to improve the AND gate logic.



number=$10 ;16bit number address


  ; Start of the program (Reset vector)
  .org $E000
start:
  lda #%0001 ; Set Front pin as output
  sta $7040 ; Set pin direction register

  ; Setup hardware pin interrupts
  lda #$50 ; Select interrupt type (digital change)
           ; for pins Right and Left
  sta $7048 ; Set interrupt type register for Right pin
  sta $7049 ; Set interrupt type register for Left pin
  brk ; Manually trigger interrupt on startup
  cli ; Enable interrupts (important to do after BRK)


 ; Main loop that counts up or down a
 ; 16bit number
loop:
  ; Determine how much to increment or decrement
  lda $7002 ; Read the value of the Back pin register
  asl ; Multiply the value by 2
  sta $02 ; Use this value to increment or decrement
          ; the number by (stored in $02)

  ; Determine if the number should be incremented
  ; or decremented
  lda $7000 ; Read the output value of the AND gate
            ; (outputted to the Front pin register)
  beq sub ; Decrement the number if the output is 0

  ; Increment 16bit number by the value in $02
  clc
  lda number
  adc $02
  sta number
  lda number + 1
  adc #0
  sta number + 1
  jmp loop ; Repeat the loop

  ; Decrement 16bit number by the value in $02
sub:
  sec
  lda number
  sbc $02
  sta number
  lda number + 1
  sbc #0
  sta number + 1
  jmp loop ; Repeat the loop



; Interrupt handler that does AND logic
irq:
  pha ; Remember the A register
  ldx #0 ; Use X Register as a digital 0
  ldy #15 ; Use Y Register as a digital 1 (full power)

  ; Read the value of the Right pin
  lda $7001
  sta $00

  ; Read the value of the Left pin
  lda $7003

  ; AND the values of the Right and Left pins
  and $00

  ; AND gate outputs 0 if the result is 0
  bne irq0
  stx $7000
  jmp irq1

  ; AND gate outputs 1 otherwise
irq0:
  sty $7000

  ; Clear the interrupt flags for Right and Left pins
  ; Must be done for interrupt to trigger again
  ; Clear flags by writing a 1 for each pin
irq1:
  lda #%1010
  sta $7068 ; Clear flags in the interrupt status register

  pla ; Restore the A register
  rti ; Return from interrupt
  


; Processor vectors
  .org $FFFC
  .word $E000 ; Reset vector
  .word irq ; IRQ vector (interrupt handler)



























































