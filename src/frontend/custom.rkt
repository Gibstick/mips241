#lang racket/base

(require "frontend.rkt")

(define custom-frontend-file (make-parameter #f))

(define-namespace-anchor a)

(define (init-fn machine)
  (parameterize ([current-namespace (namespace-anchor->namespace a)])
    ((dynamic-require (custom-frontend-file) 'init-fn) machine)))

(start
 #:init-fn init-fn
 #:once-each
 `([("-f" "--frontend")
    ,(lambda (f filename) (custom-frontend-file filename))
    ("Specify a custom frontend as a racket script" "filename")])
 #:help-strings
 '("Allows loading custom frontends."))