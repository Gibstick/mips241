#lang racket/base

;; Example of a Racket frontend
;; Gets an int and places it in $1

(require "frontend.rkt"
         racket/class
         threading)


(define (init-fn machine)
  ;; https://docs.racket-lang.org/threading/index.html
  (display "Enter value for register 1: ")
  (define result
    (and~> (read-line)
           ((lambda (x) (and (not (eof-object? x)) x)))
           string->number
           (send machine set-reg! 1 _)))
  (unless result
    (raise-user-error 'init
                      "Invalid number.")))

(start #:init-fn init-fn)
