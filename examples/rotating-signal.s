; Rotating signal program
; The output rotates clockwise

; It uses a LUT to get the half sine of Y
; and the half cosine of X.
; Input values for LUT are 0-255.

; Only uses a LUT and the registers for
; fast execution. RAM is not used.
; LUT table looks like this:
;
;15       __      
;|      /    \
;|    /        \
;|  /            \
;| /              \______________
;0-------------------------------255


; Start of the program (Reset vector)
  .org $E000
start:
  lda #%1111 ; Set all pins as output
  sta $7040 ; Set Pin Direction Register
  ldy #0  ; Y Register for calculating sin
  ldx #64 ; X Register 90 degrees shifted for
          ; calculating cos



; Main loop
loop:
  ; Increase Y by ~14 degrees
  tya
  clc
  adc #10
  tay

  ; Look up the half sine of Y
  lda halfsine, y
  sta $7000 ; Set Front Pin Register

  ; Look up half sine of Y 180 degrees out
  ; of phase
  lda halfsine180, y
  sta $7002 ; Set Back Pin Register

  ; Increase X by ~14 degrees
  txa
  clc
  adc #10
  tax

  ; Look up the half cosine of X
  lda halfsine, x
  sta $7003 ; Set Left Pin Register

  ; Look up half cosine of X 180 degrees out
  ; of phase
  lda halfsine180, x
  sta $7001 ; Set Right Pin Register
  tya
  
  jmp loop ; Repeat the loop
 


;LUT for a half sine wave
halfsine:
  .byte $00, $00, $01, $01, $01, $02, $02, $03
  .byte $03, $03, $04, $04, $04, $05, $05, $05
  .byte $06, $06, $06, $07, $07, $07, $08, $08
  .byte $08, $09, $09, $09, $0a, $0a, $0a, $0a
  .byte $0b, $0b, $0b, $0b, $0c, $0c, $0c, $0c
  .byte $0c, $0d, $0d, $0d, $0d, $0d, $0e, $0e
  .byte $0e, $0e, $0e, $0e, $0e, $0e, $0f, $0f
  .byte $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f
  .byte $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f
  .byte $0f, $0f, $0f, $0e, $0e, $0e, $0e, $0e
  .byte $0e, $0e, $0e, $0d, $0d, $0d, $0d, $0d
  .byte $0c, $0c, $0c, $0c, $0c, $0b, $0b, $0b
  .byte $0b, $0a, $0a, $0a, $0a, $09, $09, $09
  .byte $08, $08, $08, $07, $07, $07, $06, $06
  .byte $06, $05, $05, $05, $04, $04, $04, $03
  .byte $03, $03, $02, $02, $01, $01, $01, $00
halfsine180: ;LUT for a half sine wave 180
             ;out of phase.
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $00, $00, $00, $00, $00, $00
  .byte $00, $00, $01, $01, $01, $02, $02, $03
  .byte $03, $03, $04, $04, $04, $05, $05, $05
  .byte $06, $06, $06, $07, $07, $07, $08, $08
  .byte $08, $09, $09, $09, $0a, $0a, $0a, $0a
  .byte $0b, $0b, $0b, $0b, $0c, $0c, $0c, $0c
  .byte $0c, $0d, $0d, $0d, $0d, $0d, $0e, $0e
  .byte $0e, $0e, $0e, $0e, $0e, $0e, $0f, $0f
  .byte $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f
  .byte $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f
  .byte $0f, $0f, $0f, $0e, $0e, $0e, $0e, $0e
  .byte $0e, $0e, $0e, $0d, $0d, $0d, $0d, $0d
  .byte $0c, $0c, $0c, $0c, $0c, $0b, $0b, $0b
  .byte $0b, $0a, $0a, $0a, $0a, $09, $09, $09
  .byte $08, $08, $08, $07, $07, $07, $06, $06
  .byte $06, $05, $05, $05, $04, $04, $04, $03
  .byte $03, $03, $02, $02, $01, $01, $01, $00
  

  
  ; Processor vectors
  .org $FFFC
  .word start ; Reset vector
  .word $0000 ; IRQ vector (not used)































































