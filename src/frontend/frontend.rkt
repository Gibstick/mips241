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

(provide (contract-out [print-registers (-> machine? any)])
         load-program!
         default-post-fn
         num-registers
         start)

(define version "0.1")
(define return-address #x8123456c)


;; Prints all registers of a Machine.
;; Requires: non-null Machine pointer
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
                         (array-ref (machine-registers m) reg))))
           (range i (min (+ i 4) num-registers)))
      "  "))))


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
      [(idx . >= . (machine-mem-size m))
       (error "Your program is too big. Exiting.")]
      [(idx . >= . return-address)
       (error "Your program goes past the return address. Exiting.")]
      [else
       (ptr-set! (machine-mem m)
                 _uint32
                 idx
                 (integer-bytes->integer word false true))
       (loop (read-bytes 4 in) (add1 idx))]))
  (set-machine-pc! m offset)
  (array-set! (machine-registers m) 31 return-address))

(define (default-post-fn m status)
  (print-registers m)
  (displayln (emulator-status-retcode status)))

;; Params for start
(define display-version (make-parameter false))
(define load-address (make-parameter 0))

;; Main function. Frontends will call this function to actually do stuff.
;;   initializer-fn is a (_Machine -> Void). It does setup,
;;   for example reading in two integers and placing them in $1 and $2.
;;
;;   post-fn is a (_Machine emualtor-status -> Void). It does stuff once the
;;   program has finished. By default, it prints out all the registers.
;;
;;   help-strings is a list of strings to display when the help flag is given
;;   on the command line. Each string will be printed on its own line.
(define (start initializer-fn
               #:post-fn [post-fn default-post-fn]
               #:help-strings [help-strings empty])
  (init-emulator!)
  (define m (init-machine 0))
  
  (define ps-list
    (list* 'ps "" "Frontend description:" help-strings))
  
  (define filename
    (parse-command-line
     (find-system-path 'run-file)
     (current-command-line-arguments)
     `((once-each
        [("--version")
         ,(lambda (f) (display-version true))
         ("Display the version number")]
        [("-l" "--load-at")
         ,(lambda (f addr) (load-address (string->number addr)))
         ("Load the program at <address>" "address")])
       ,ps-list)
     (lambda (flag-accum [filename false]) filename)
     '("filename")))
  
  (when (display-version)
    (displayln version)
    (exit))
  
  (unless filename
    (displayln "Invalid usage. Try --help." (current-error-port))
    (exit 1))
  
  (with-input-from-file
      filename
    (thunk (load-program! m (load-address))))
  
  (define status (step-machine!/loop m))
  
  (post-fn m status))

(module+ main
  (start (lambda (m) (void))
         #:help-strings
         '("The barebones frontend that loads the program and runs it.")))
