#!/usr/bin/env racket
#lang racket/base
#|
Stuff that is common to all frontends.
|#

(require racket/string ; string-join
         racket/format ; ~a
         racket/class  ; send
         racket/list   ; range, list, etc
         racket/cmdline
         racket/port
         (only-in ffi/unsafe
                  array-ref
                  ptr-set!
                  _uint32
                  array-set!)
         "bindings.rkt"
         "guess-file-type.rkt")

(provide load-program!
         default-post-fn
         num-registers
         start
         (all-from-out "bindings.rkt"))

(define version "0.1")
(define return-address #x8123456c)
(define stack-address #x01000000)


;; Prints all registers of a Machine.
;; Effects: Prints to (current-output-port)
(define (print-registers m)
  (for ([i (in-range 1 num-registers 4)])
    (displayln
     (string-join
      (map (lambda (reg)
             (format "$~a = 0x~a"
                     (~a #:min-width 2 #:left-pad-string "0" #:align 'right
                         reg)
                     (~r #:base 16 #:min-width 8 #:pad-string "0"
                         (send m get-reg reg))))
           (range i (min (+ i 4) num-registers)))
      "  ")
     (current-error-port))))


;; Load a program into a machine at offset,
;;   reading input from the port in
;; Requires: Non-null Machine pointer
(define (load-program! m offset [in (current-input-port)])
  (define end-address
    (let loop
      ([word (read-bytes 4 in)]
       [idx (/ offset 4)])
      (cond
        [(eof-object? word) (* 4 idx)]
        [(idx . >= . (send m get-mem-size))
         (error "Your program is too big. Exiting.")]
        [(idx . >= . return-address)
         (error "Your program goes past the return address. Exiting.")]
        [else
         (send m set-mem! idx (integer-bytes->integer word #f #t))
         (loop (read-bytes 4 in) (add1 idx))])))
  (send m set-pc! offset)
  (send m set-reg! 30 (max end-address stack-address))
  (send m set-reg! 31 return-address))

(define (default-post-fn m status)
  (print-registers m)
  (displayln (emulator-status-retcode status)))


;; Workaround: parse-command-line does not
;; work with ps-strings, so we define our own
;; help function closure to print the ps-strings
(define (make-help-fn ps-list)
  (lambda (s)
    (display s)
    (for ([line (in-list ps-list)])
      (displayln line))
    (exit 0)))

;; Params for start
(define display-version (make-parameter #f))
(define load-address (make-parameter 0))
(define file-type (make-parameter #f))
(define assembler (make-parameter "java cs241.binasm"))

;; Main function. Frontends will call this function to actually do stuff.
;;   init-fn is a (machine% -> Void). It does setup,
;;   for example reading in two integers and placing them in $1 and $2.
;;
;;   post-fn is a (machine% emulator-status -> Void). It does stuff once the
;;   program has finished. By default, it prints out all the registers.
;;
;;   help-strings is a list of strings to display when the help flag is given
;;   on the command line. Each string will be printed on its own line.
;;
;;   once-each: DO NOT USE YET
(define (start #:init-fn [init-fn (lambda (m) (void))]
               #:post-fn [post-fn default-post-fn]
               #:help-strings [help-strings empty]
               #:once-each [once-each empty])
  (init-emulator!)
  (define m (new machine%))

  (define ps-list
    (list* "" "Frontend description:" help-strings))

  (define flaglist
    (list* 'once-each
           `[("--version")
             ,(lambda (f) (display-version #t))
             ("Display the version number")]
           `[("-l" "--load-at")
             ,(lambda (f addr) (load-address (string->number addr)))
             ("Load the program at <address>" "address")]
           `[("-t" "--type")
             ,(lambda (f type) (file-type (string->symbol type)))
             ("Manually specify the file type as binary or ascii" "type")]
           `[("-a" "--assembler")
             ,(lambda (f as) (assembler as))
             ("Program and arguments to be invoked for assembling" "assembler")]
           once-each))

  (define filename
    (parse-command-line
     (find-system-path 'run-file)
     (current-command-line-arguments)
     `(,flaglist)
     (lambda (flag-accum [filename #f]) filename)
     '("filename")
     (make-help-fn ps-list)
     ))



  ;; short-circuit and display version if -v
  (when (display-version)
    (displayln version)
    (exit))

  (unless filename
    (displayln "Invalid usage. Try --help." (current-error-port))
    (displayln "For input from standard input, give '-' as the filename." (current-error-port))
    (exit 1))

  ;; set up ports
  (define in-port
    (if (not (equal? filename "-"))
      (open-input-file filename #:mode 'binary)
      (current-input-port)))

  (define stdin? (equal? filename "-"))

  ;; guess file type if necessary
  (unless stdin?
    (with-input-from-file filename
      (lambda () (file-type (guess-file-type)))))

  ;; assume stdin is machine code

  (define-values (subproc
                  proc-out
                  proc-in
                  proc-err)
    (cond
      [(or stdin? (equal? (file-type) 'binary))
        (values #f in-port #f #f)]
      [else
        ;; check for empty string
        (when (string=? (assembler) "")
          (raise-user-error 'start "Invalid path to assembler ~s"
                            (assembler)))

        (define split-path (string-split (assembler)))
        (define as-path (find-executable-path (first split-path)))
        (define as-args (string-join (rest split-path)))

        (unless as-path
          (raise-user-error 'start
                            "Assembler ~s not found in PATH"
                            (first split-path)))

        (subprocess #f in-port #f as-path as-args)]))

  (load-program! m (load-address) proc-out)

  (and subproc (subprocess-wait subproc))

  (unless (or (not subproc)
              (zero? (subprocess-status subproc)))
    (raise-user-error 'start
                      "Assembler returned with nonzero status ~a\n~a"
                      (port->string proc-err)
                      (subprocess-status subproc)))

  (init-fn m)

  (define status (send m step!/loop))

  (post-fn m status)

  ;; close ports
  (and proc-out (close-input-port proc-out))
  (and proc-in (close-output-port proc-in))
  (and proc-err (close-input-port proc-err))
  (close-input-port in-port)
  (void)
  )

(module+ main
  (start #:help-strings
         '("The barebones frontend that loads the program and runs it.")))
