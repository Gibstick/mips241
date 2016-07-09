#lang racket/base

(require "frontend.rkt"
         racket/list
         racket/format
         racket/class)

;; Prints all registers in a legacy format
(define (print-registers/legacy m)
  (for ([i (in-range num-registers)])
    (eprintf "register ~a: 0x~a\n"
             (~a #:min-width 2 #:align 'right i)
             (~r #:base 16 #:min-width 8 #:pad-string "0"
                 (send m get-reg i)))))

(define (emulator-status->string status)
  (case (emulator-status-retcode status)
    [(IR_DONE) "Program completed successfully."]
    [(IR_SUCCESS) "Program execution paused."]
    [(IR_UNALIGNED_MEMORY_ACCESS) "Program attempted to read/write an unaligned address."]
    [(IR_UNALIGNED_INSTRUCTION_FETCH) "Program counter contains an unaligned address."]
    [(IR_OUT_OF_RANGE_MEMORY_ACCESS) "Program attempted to read/write memory that was out of bounds."]
    [(IR_OUT_OF_RANGE_INSTRUCTION_FETCH) "Program counter contains an out-of-bounds address."]
    [(IR_INVALID_INSTRUCTION) "An invalid instruction was encountered."]
    [else "Unknown error!"]))

    
(define (post-fn m status)
  (print-registers/legacy m)
  (displayln (emulator-status->string status)))

(start #:post-fn post-fn
       #:help-strings '("Legacy frontend for compatiblity with the C version."))
