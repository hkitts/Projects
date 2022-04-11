(define swap
  (lambda (a b)
	(cond ((< a b) (cons a b))
		  (else (cons b a))
		  )))

(define avg (lambda (L)
		  (letrec (
			(avgFinal (lambda (s len)
						(cond
						  ((< len 1) 0)
						  (else (/ s (+ len 0.0))))))

			
				   (sumHelper (lambda (L sumThusFar n)
			(cond
			  ((null? L) (avgFinal sumThusFar n))
			  (#t (sumHelper (cdr L)
							 (+ (car L) sumThusFar) (+ 1 n)))))))
			(sumHelper L 0 0)
			

			)))

(define min (lambda (L)
	(letrec ((minHelper (lambda (L minThusFar)
				(cond
				  ((null? L) '())
				  ((null? (cdr L)) (car L))
				  (#t (let ((a (car L))
							(b (min (cdr L))))
						(if (< b a) b a)))))))
			 (minHelper L '())
			 )))
