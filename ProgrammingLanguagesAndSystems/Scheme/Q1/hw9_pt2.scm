;takes two lists and returns a single merged list
(define merge
  (lambda (L1 L2)
	(cond
	  ((null? L1) L2) ;if L1 is empty return L2
	  ((null? L2) L1) ;if L2 is empty return L1

	  ;if car L1 < car L2 
	  ((< (car L1) (car L2))

	   ;get the first element of L1
	   ;then merge the rest of L1 with L2
	   (cons (car L1) 
			 (merge (cdr L1) L2))
	  )

	  ;if car L1 > car L2
      (else

		;get the first element of L2
		;merge the rest of L2 with L1
		(cons (car L2) (merge L1 (cdr L2)))
	  )
	 )
  )
)

;divides a list L into two separate, equal-sized lists L1 and L2
;whichlist? indicates which list the next element of L should be added to
(define mergesortHelper
  (lambda (L L1 L2 whichlist?)
	(cond

	  ;if L is empty merge start merging the two lists together
	  ((null? L)
	   (merge (mergesort L1) (mergesort L2))
	  )

	  ;if the next element is from L1
	  ((eq? whichlist? 1)
	   ;call to mergesortHelper
	   ;mergesortHelper L       L1                L2 whichlist?
	   (mergesortHelper (cdr L) (cons (car L) L1) L2 (+ whichlist? 1))
	  )

	  ;if the next element is from L2
	  ((eq? whichlist? 2)
	   ;call to mergesortHelper
	   ;mergesortHelper L       L1 L2                whichlist?
	   (mergesortHelper (cdr L) L1 (cons (car L) L2) (- whichlist? 1))
	  )
	)
  )
)

;mergesort a given list
;calls mergesortHelper and merge
(define mergesort
  (lambda (L)
	(cond
	  ;if the list is empty
	  ((null? L) '())

	  ;check that the rest of the list is not empty
	  ((null? (cdr L)) L)
	  (else (mergesortHelper L '() '() 1))
	)
   )
)


;as recursive calls pop
;it should build up a list that
;passed the expression
(define filter
  (lambda (expr L)
	(cond

	  ;if the list is empty return empty list
	  ((null? L) '())

	  ;use the given predicate on the first
	  ;element of the list
	  ((expr (car L))
	   ;if it passes
	   ;make a cons of the first element
	   ;and call it on the rest of the list
	   (cons (car L) (filter expr (cdr L)))
	  )
	  ;otherwise call filter on the rest of the list
	  (else (filter expr (cdr L)))
	)
  )
)
