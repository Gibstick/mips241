#lang racket
#|
Stuff that is common to all frontends.
|#

(require racket/contract
         (only-in ffi/unsafe
                  array-ref
                  ptr-set!
                  _uint32
                  array-set!)
         "bindings.rkt")

(provide load-program!
         default-post-fn
         num-registers
         start
         (all-from-out "bindings.rkt"))

(define version "0.1")
(define return-address #x8123456c)


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
  (unless m
    (error "Null pointer??? Bye."))
  (let loop
    ([word (read-bytes 4 in)]
     [idx (/ offset 4)])
    (cond
      [(eof-object? word) (void)]
      [(idx . >= . (send m get-mem-size))
       (error "Your program is too big. Exiting.")]
      [(idx . >= . return-address)
       (error "Your program goes past the return address. Exiting.")]
      [else
       (send m set-mem! idx (integer-bytes->integer word false true))
       (loop (read-bytes 4 in) (add1 idx))]))
  (send m set-pc! offset)
  (send m set-reg! 31 return-address))

(define (default-post-fn m status)
  (print-registers m)
  (displayln (emulator-status-retcode status)))

;; Params for start
(define display-version (make-parameter false))
(define load-address (make-parameter 0))

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
    (list* 'ps "" "Frontend description:" help-strings))

  (define flaglist
    (list* 'once-each
           `[("--version")
             ,(lambda (f) (display-version true))
             ("Display the version number")]
           `[("-l" "--load-at")
             ,(lambda (f addr) (load-address (string->number addr)))
             ("Load the program at <address>" "address")]
           once-each))

  (define filename
    (parse-command-line
     (find-system-path 'run-file)
     (current-command-line-arguments)
     `(,flaglist
       ,ps-list)
     (lambda (flag-accum [filename false]) filename)
     '("filename")))

  (when (display-version)
    (displayln version)
    (exit))

  (unless filename
    (displayln "Invalid usage. Try --help." (current-error-port))
    (displayln "For input from standard input, give '-' as the filename." (current-error-port))
    (exit 1))

  (init-fn m)

  (if (equal? filename "-")
      (load-program! m (load-address))
      (with-input-from-file
          filename
        (thunk (load-program! m (load-address)))))

  (define status (send m step!/loop))

  (post-fn m status))

(module+ main
  (start #:help-strings
         '("The barebones frontend that loads the program and runs it.")))
