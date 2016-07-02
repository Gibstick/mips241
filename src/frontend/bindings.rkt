#lang racket

#|
Definitions and bindings for everything needed to interact
with the emulator from Racket.

Basically, the following files need to be "ported":
  - machine/machine.h
  - emulator/emulator.h
  - common/defs.h
|#

(require ffi/unsafe
         ffi/unsafe/define
         ffi/unsafe/alloc)

(provide num-registers
         (struct-out machine)
         destroy-machine!
         init-machine
         (struct-out emulator-status)
         init-emulator!
         step-machine!
         step-machine!/loop
         dump-memory)

(define get-paths-for-lib
  (const
   (list (simplify-path (build-path (find-system-path 'run-file) 'up))
         (simplify-path (build-path (find-system-path 'run-file) 'up 'up "lib")))))

(define num-registers 32)
(define-ffi-definer define-mips241
  (ffi-lib "libmips241"
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
(define-mips241 dump-memory
  (_fun _machine-pointer _path -> _void)
  #:c-id dump_memory)
