/*
 * JOOS is Copyright (C) 1997 Laurie Hendren & Michael I. Schwartzbach
 *
 * Reproduction of all or part of this software is permitted for
 * educational or research use on condition that this copyright notice is
 * included in any copy. This software comes with no warranty of any
 * kind. In no event will the authors be liable for any damages resulting from
 * use of this software.
 *
 * email: hendren@cs.mcgill.ca, mis@brics.dk
 */

/* 

   ldc_int 6
   ldc_int 6
   --------->
   ldc_int 6
   dup

   src: https://frippery.org/jopt/opt2.html
*/
int simplify_duplicate_intconstants(CODE **c)
{ int x, y;
  if (is_ldc_int(*c, &x) &&
      is_ldc_int(next(*c), &y) && x == y) {
    return replace(c, 2,makeCODEldc_int(x, makeCODEdup(NULL)));
  }
  return 0;
}

/* iload x        iload x        iload x
 * ldc 0          ldc 1          ldc 2
 * imul           imul           imul
 * ------>        ------>        ------>
 * ldc 0          iload x        iload x
 *                               dup
 *                               iadd
 */

int simplify_multiplication_right(CODE **c)
{ int x,k;
  if (is_iload(*c,&x) && 
      is_ldc_int(next(*c),&k) && 
      is_imul(next(next(*c)))) {
     if (k==0) return replace(c,3,makeCODEldc_int(0,NULL));
     else if (k==1) return replace(c,3,makeCODEiload(x,NULL));
     else if (k==2) return replace(c,3,makeCODEiload(x,
                                       makeCODEdup(
                                       makeCODEiadd(NULL))));
     return 0;
  }
  return 0;
}

/* dup
 * astore x
 * pop
 * -------->
 * astore x
 */
int simplify_astore(CODE **c)
{ int x;
  if (is_dup(*c) &&
      is_astore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
     return replace(c,3,makeCODEastore(x,NULL));
  }
  return 0;
}

/* iload x
 * ldc k   (0<=k<=127)
 * iadd
 * istore x
 * --------->
 * iinc x k
 */ 
int positive_increment(CODE **c)
{ int x,y,k;
  if (is_iload(*c,&x) &&
      is_ldc_int(next(*c),&k) &&
      is_iadd(next(next(*c))) &&
      is_istore(next(next(next(*c))),&y) &&
      x==y && 0<=k && k<=127) {
     return replace(c,4,makeCODEiinc(x,k,NULL));
  }
  return 0;
}

/* goto L1
 * ...
 * L1:
 * goto L2
 * ...
 * L2:
 * --------->
 * goto L2
 * ...
 * L1:    (reference count reduced by 1)
 * goto L2
 * ...
 * L2:    (reference count increased by 1)  
 */
int simplify_goto_goto(CODE **c)
{ int l1,l2;
  if (is_goto(*c,&l1) && is_goto(next(destination(l1)),&l2) && l1>l2) {
     droplabel(l1);
     copylabel(l2);
     return replace(c,1,makeCODEgoto(l2,NULL));
  }
  return 0;
}

/*
 *
 *  Old style - still works, but better to use new style.
 *
#define OPTS 5

OPTI optimization[OPTS] = {simplify_multiplication_right,
                           simplify_astore,
                           positive_increment,
                           simplify_goto_goto,
                           simplify_duplicate_intconstants};

* new style for giving patterns
* *
*/

/*
 * iload x
 * pop
 * ----->
 * nothing
 */
int simplify_pop_after_load(CODE **c) 
{
	int x;
	if (is_iload(*c, &x) && 
		is_pop(next(*c)))
		return replace(c, 2, NULL);
	return 0;
}

/*
 * TODO not so sure about this nop...
 * nop
 * ----->
 * nothing
 */
int simplify_nop(CODE **c) 
{
  	if (is_nop(*c))
  		return replace(c, 1, NULL);
	return 0;
}

/*
 * aload_0
 * getfield test/b Z
 * iconst_1
 * if_icmpne l
 * ---------->
 *
 * aload_0
 * getfield test/b Z
 * ifne l
 */

int simplify_equals_true_field(CODE **c)
{
	int x, y, l;
	char *arg;

	if( is_aload(*c, &x) &&
		is_getfield(next(*c), &arg) && 
		is_ldc_int(next(next(*c)), &y) && y == 1 &&
		is_if_icmpeq(next(next(next(*c))), &l) )
			return replace(c, 4, makeCODEaload(x, makeCODEgetfield(arg, makeCODEifne(l, NULL))));
	return 0;
}

int init_patterns()
{ 	
	ADD_PATTERN(simplify_multiplication_right);
	ADD_PATTERN(simplify_astore);
	ADD_PATTERN(positive_increment);
	ADD_PATTERN(simplify_goto_goto);
	ADD_PATTERN(simplify_duplicate_intconstants);
	ADD_PATTERN(simplify_pop_after_load);
	ADD_PATTERN(simplify_nop);
	ADD_PATTERN(simplify_equals_true_field);
	return 1;
}

/* 
 * bipush 9
 * istore 4
 * iload 4
 * istore_1	
 * -------->
 * bipush 9
 * dup
 * istore 4
 * istore_1
 *
 * source: https://frippery.org/jopt/opt5.html
 */

/* 
 * is_bipush() not present 
int simplify_store_followed_by_fetch(CODE **c)
{ 
	return 0;
}
*/


