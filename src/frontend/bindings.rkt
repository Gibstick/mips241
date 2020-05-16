#lang racket/base

#|
Definitions and bindings for everything needed to interact
with the emulator from Racket.

Basically, the following files need to be "ported":
  - machine/machine.h
  - emulator/emulator.h
  - common/defs.h
|#

;; for windows, lib name does not have the lib-prefix
(define mips241-libname
    (if (eq? (system-type 'os) 'windows) "mips241" "libmips241"))

(require ffi/unsafe
         ffi/unsafe/define
         ffi/unsafe/alloc
         racket/class)

(provide num-registers
         (struct-out emulator-status)
         init-emulator!
         machine%)

;; paths for the shared object/dll should be
;; 1. same dir as the racket binary
;; 2. ../lib
(define get-paths-for-lib
  (lambda ()
   (list (simplify-path (build-path (find-system-path 'run-file) 'up))
         (simplify-path (build-path (find-system-path 'run-file) 'up 'up "lib")))))

(define num-registers 32)
(define-ffi-definer define-mips241
  (ffi-lib mips241-libname
           #:get-lib-dirs get-paths-for-lib))

;; Types from C. These must match!
(define-cstruct _machine
  ([mem _pointer]
   [mem-size _uint32]
   [registers (_array _uint32 num-registers)]
   [pc _uint32]
   [hi _uint32]
   [lo _uint32]))


;; Free the resources associated with a Machine.
(define-mips241 destroy-machine!
  (_fun _machine-pointer -> _void)
  #:wrap (deallocator)
  #:c-id destroy_machine)

;; Returns a pointer to a ready-to-use Machine.
(define-mips241 init-machine
  (_fun _uint32
        -> (ret : _machine-pointer)
        -> (and (not (ptr-equal? ret #f)) ret))
  #:wrap (allocator destroy-machine!)
  #:c-id init_machine)

;; An enum representing the possible conditions
;;   after executing an instruction.
(define _instruction-retcode
  (_enum '(IR_DONE = 0
                   IR_SUCCESS = 1
                   IR_UNALIGNED_MEMORY_ACCESS
                   IR_UNALIGNED_INSTRUCTION_FETCH
                   IR_OUT_OF_RANGE_MEMORY_ACCESS
                   IR_OUT_OF_RANGE_INSTRUCTION_FETCH
                   IR_INVALID_INSTRUCTION
                   )))

(define-cstruct _emulator-status
  ([retcode _instruction-retcode]
   [pc _uint32]))

;; Must call this on startup to initialize dispatch tables
(define-mips241 init-emulator!
  (_fun -> _void)
  #:c-id init_emulator)

;; Run one instruction.
(define-mips241 step-machine!
  (_fun _machine-pointer -> _emulator-status)
  #:c-id step_machine)

;; Run as many instructions as possible until.
(define-mips241 step-machine!/loop
  (_fun _machine-pointer -> _emulator-status)
  #:c-id step_machine_loop)


;; Dump memory to file
(define-mips241 dump-memory/fn
  (_fun _machine-pointer _path -> _void)
  #:c-id dump_memory)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; A class to wrap a Machine
;; This gives us the dynamic capabilities we need for our plugin scripts
(define machine%
  (class object%
    ;; init
    (super-new)
    (init [memory-size 0])

    ;; private fields
    (define m (init-machine memory-size))
    (define mem-size (machine-mem-size m))

    ;; public methods

    ;; return the amount of memory available to the machine
    (define/public (get-mem-size) mem-size)

    ;; return the word at word-address idx
    (define/public (get-mem idx)
      (when (idx . >= . mem-size)
        (raise-argument-error 'get-mem
                              (format "an integer less than ~a" mem-size)
                              idx))
      (ptr-ref (machine-mem m)
               _uint32
               idx))

    ;; set the word at word-address idx
    (define/public (set-mem! idx val)
      (when (idx . >= . mem-size)
        (raise-argument-error 'get-mem
                              (format "an integer less than ~a" mem-size)
                              idx))
      (ptr-set! (machine-mem m)
                _uint32
                idx
                val))

    ;; get/set register n
    (define/public (get-reg n)
      (when (n . >= . num-registers)
        (raise-argument-error 'get-mem
                              (format "an integer less than ~a" num-registers)
                              n))
      (array-ref (machine-registers m) n))
    (define/public (set-reg! n val)
      (when (n . >= . num-registers)
        (raise-argument-error 'get-mem
                              (format "an integer less than ~a" num-registers)
                              n))
      (array-set! (machine-registers m) n val))

    ;; get/set pc
    (define/public (get-pc)
      (machine-pc m))
    (define/public (set-pc! val)
      (set-machine-pc! m val))

    ;; get/set lo
    (define/public (get-lo)
      (machine-lo m))
    (define/public (set-lo! val)
      (set-machine-lo! val))

    ;; get/set hi
    (define/public (get-hi)
      (machine-hi m))
    (define/public (set-hi! val)
      (set-machine-hi! val))

    ;; step once
    (define/public (step!)
      (step-machine! m))

    ;; step until
    (define/public (step!/loop)
      (step-machine!/loop m))

    ;; dump memory
    (define/public (dump-memory path)
      (dump-memory/fn m path))
    ))
