#|
A module for guessing whether a file is
machine code or ascii
|#

#lang racket/base

(provide guess-file-type)

;; Guesses the file type of port `in'.
;; Returns either 'ascii or 'binary. If the file is empty, it returns
;; 'binary.'
;; 
;; limit-bytes is the number of bytes used to sample the file.
;; if limit-bytes is larger than the file size, only the entire file
;; is used.

;; threshold is a tight inclusive lower bound on the ratio of
;; printable characters to nonprintable for a file to be considered ascii.
(define (guess-file-type [in (current-input-port)]
                         #:limit-bytes [limit-bytes 32]
                         #:threshold [threshold 0.95])
  (parameterize ([current-input-port in])
    (lambda ()
      (let loop ([byte (read-byte)]
                 [limit-bytes limit-bytes]
                 [printable-bytes 0]
                 [total-bytes 0])
        (cond [(and (zero? total-bytes)
                    (eof-object? byte))
               'binary]
              [(or (zero? limit-bytes)
                   (eof-object? byte))
               (if (and (not (zero? printable-bytes))
                        (>= (/ printable-bytes total-bytes)
                            threshold))
                 'ascii
                 'binary)]
              [(loop (read-byte)
                     (sub1 limit-bytes)
                     (if (printable? byte) (add1 printable-bytes) printable-bytes)
                     (add1 total-bytes))])))))

;; Returns true if the byte is printable ascii
(define (printable? byte)
  (or (and (>= byte 32)
           (<= byte 127))
      (= byte 10)))
