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
 iload x
 aload y
 swap
 ----->
 aload y
 iload x
 */

int simplify_aload_iload_swap(CODE **c)
{ int x, y;
  if (is_iload(*c, &x) && is_aload(next(*c), &y) && is_swap(next(next(*c)))) {
    return replace(c, 3, makeCODEaload(y, makeCODEiload(x, NULL)));
  }
  return 0;
}

int simplify_aload_aload_swap(CODE **c)
{ int x, y;
  if (is_aload(*c, &x) && is_aload(next(*c), &y) && is_swap(next(next(*c)))) {
    return replace(c, 3, makeCODEaload(y, makeCODEaload(x, NULL)));
  }
  return 0;
}

/*
  goto f1
  .
  .
  .
  f1:
  f2:
  ------>
  goto f2
  .
  .
  .
  f1:
  f2:

*/
int simplify_dup_labels(CODE **c)
{ int f1, f2;
  if (is_goto(*c, &f1)) {
    if ( is_label(next(destination(f1)), &f2) ) {
      copylabel(f2);
      droplabel(f1);
      return replace(c, 1, makeCODEgoto(f2, NULL));
    }
  }
  return 0;
}

/*
  if_acmpne uncryptic_16
  goto stop_12
  uncryptic_16:
  -------------->
  if_acmpeq stop_12
  (uncryptic: if not unique)
*/

int simplify_if_goto_label(CODE **c)
{ int l1, l2, l3;
  CODE *l;
  l = NULL;
  if ( is_goto(next(*c), &l2) &&
      is_label(next(next(*c)), &l3)){
    if (is_if_acmpne(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_acmpeq(l2, l));
    } else if (is_if_acmpeq(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_acmpne(l2, l));
    } else if (is_if_icmpeq(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmpne(l2, l));
    } else if (is_if_icmpne(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmpeq(l2, l));
    } else if (is_if_icmpgt(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmple(l2, l));
    } else if (is_if_icmplt(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmpge(l2, l));
    } else if (is_if_icmpge(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmplt(l2, l));
    } else if (is_if_icmple(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEif_icmpgt(l2, l));
    } else if (is_ifeq(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEifne(l2, l));
    } else if (is_ifne(*c, &l1) && l1 == l3) {
      if (!uniquelabel(l1)) {
        l = makeCODElabel(l1, NULL);
      }
      droplabel(l1);
      return replace(c, 3, makeCODEifeq(l2, l));
    }
  }
  return 0;

}

/*
  if_icmplt uncryptic_17
  goto true_1
  uncryptic_17:
  goto stop_0
  true_1:
  ------->
  if_icmplt stop_0
  true_1:
  */
int simplify_continuous_labels(CODE **c)
{ int u17, t1, u17l, s0, t1l;
  if (is_goto(next(*c), &t1) &&
      is_label(next(next(*c)), &u17l) &&
      is_goto(next(next(next(*c))), &s0) &&
      is_label(next(next(next(next(*c)))), &t1l) && t1==t1l
     ) {
    if (uniquelabel(u17l)) {
      if ( is_if_icmplt(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmplt(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_icmple(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmple(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_icmpge(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmpge(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_icmpne(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmpne(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_icmpgt(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmpgt(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_icmpeq(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_icmpeq(s0, makeCODElabel(t1, NULL)));
      } else if ( is_ifnonnull(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEifnonnull(s0, makeCODElabel(t1, NULL)));
      } else if ( is_ifnull(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEifnull(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_acmpne(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_acmpne(s0, makeCODElabel(t1, NULL)));
      } else if ( is_if_acmpeq(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEif_acmpeq(s0, makeCODElabel(t1, NULL)));
      } else if ( is_ifeq(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEifeq(s0, makeCODElabel(t1, NULL)));
      } else if ( is_ifne(*c, &u17) && u17 == u17l) {
        droplabel(u17l);
        return replace(c, 5, makeCODEifne(s0, makeCODElabel(t1, NULL)));
      }
    }
  }
  return 0;
}

 /*
   iconst_1                iconst_0
  ifeq stop_0              ifeq stop_0
  ---->                   ------->
  (drop stop 0)            goto stop_0
*/

int simplify_dumb_ifeqs(CODE **c)
{ int k, s0;
  if (is_ldc_int(*c, &k)) {
    if (k != 0 && is_ifeq(next(*c), &s0)) {
      droplabel(s0);
      return replace(c, 2, NULL);
    } else if (k == 0 && is_ifeq(next(*c), &s0)) {
      return replace(c, 2, makeCODEgoto(s0, NULL));
    }
  }
  return 0;
}


 /*
   iconst_1                iconst_0
  ifne stop_0              ifne stop_0
  ---->                   ------->
  goto stop_0             drop stop_0
*/

int simplify_dumb_ifnes(CODE **c)
{ int k, s0;
  if (is_ldc_int(*c, &k)) {
    if (k == 0 && is_ifne(next(*c), &s0)) {
      droplabel(s0);
      return replace(c, 2, NULL);
    } else if (k != 0 && is_ifne(next(*c), &s0)) {
      return replace(c, 2, makeCODEgoto(s0, NULL));
    }
  }
  return 0;
}

/*
  iconst_0
  goto false_13
  .
  .
  false_13:
  ifeq stop_12
  ------------>
  goto stop_12
  .
  .
  false_13:
  ifeq stop_12
*/

int simplify_jumping_ifs(CODE **c)
{ int x, f13, s12, f13l;
  if (is_ldc_int(*c, &x) && is_goto(next(*c), &f13))
   if ( is_label(destination(f13), &f13l) && is_ifeq(next(destination(f13)), &s12) && x == 0 && f13 == f13l) {
    copylabel(s12);
    droplabel(f13);
    if (!deadlabel(s12))
      return replace(c, 2, makeCODEgoto(s12, NULL));
  }
  return 0;
}

/* 
  aconst_null
  if_acmpne true_2
  --------->
  if_nonnull true_2
*/
int simplify_if_cmp_null(CODE **c)
{ int l;
  if ( is_aconst_null(*c) && is_if_acmpne(next(*c), &l) ) {
    return replace(c, 2, makeCODEifnonnull(l, NULL));
  } else if ( is_aconst_null(*c) && is_if_acmpeq(next(*c), &l) ) {
    return replace(c, 2, makeCODEifnull(l, NULL));
  }
  return 0;
}

/*

  ifnull null_8
  goto stop_9
  null_8:
  ------>
  ifnonnull stop_9
*/
int simplify_cryptic_null_ifs(CODE **c)
{ int n8, s9, vn8;
  if (is_ifnull(*c, &n8) && is_goto(next(*c), &s9) && is_label(next(next(*c)), &vn8) && n8 == vn8) {
    droplabel(n8);
    if (deadlabel(n8)) {
      return replace(c, 3, makeCODEifnonnull(s9, NULL));
    } else {
      return replace(c, 3, makeCODElabel(n8, makeCODEifnonnull(s9, NULL)));
    }
  }
  return 0;
}


/*
  if  t3
  iconst 0
  goto s4
  t3:
  iconst 1
  s4:
  dup
  ifne f2
  pop
  -------->
  !if   new
  iconst 1
  goto f2

  new:
*/
int simplify_chained_and2(CODE **c)
{
  int t3, k1, k2, s4, f2, t3l, s4l, new_l;
  if(is_ldc_int(next(*c), &k1) &&
     is_goto(next(next(*c)), &s4) &&
     is_label(next(next(next(*c))), &t3l) &&
     is_ldc_int(next(next(next(next(*c)))), &k2) &&
     is_label(next(next(next(next(next(*c))))), &s4l) &&
     is_dup(next(next(next(next(next(next(*c))))))) &&
     is_ifne(next(next(next(next(next(next(next(*c))))))), &f2) &&
     is_pop(next(next(next(next(next(next(next(next(*c))))))))) &&
     s4 == s4l){
    k1 =1;
    if (is_if_icmple(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmpgt(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_icmplt(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmpge(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_icmpge(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmplt(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_icmpne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmpeq(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_icmpgt(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmple(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_icmpeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_icmpne(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_ifnonnull(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEifnull(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_ifnull(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEifnonnull(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_acmpeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_acmpne(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_if_acmpne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEif_acmpeq(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_ifeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEifne(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } else if (is_ifne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4) && uniquelabel(t3)) {
        droplabel(s4);droplabel(t3);
        new_l = next_label();
        INSERTnewlabel(new_l, "new", next(next(next(next(next(next(next(next(next(*c))))))))), 1);
        return replace(c, 9, makeCODEifeq(new_l, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(new_l, NULL)))));
      }
    } 
  }
  return 0;

}

/*
  if_icmple true_3       t3
  iconst_0                k1
  goto stop_4             s4
  true_3:
  iconst_1                k2
  stop_4: 
  dup
  ifeq false_2            f2
  pop

  --------------->
  if_icmple true_3
  iconst_0
  (stop_4:)
  goto false_2
  true_3:
*/

int simplify_chained_and(CODE **c)
{
  int t3, k1, k2, s4, f2, t3l, s4l;
  if(is_ldc_int(next(*c), &k1) &&
     is_goto(next(next(*c)), &s4) &&
     is_label(next(next(next(*c))), &t3l) &&
     is_ldc_int(next(next(next(next(*c)))), &k2) &&
     is_label(next(next(next(next(next(*c))))), &s4l) &&
     is_dup(next(next(next(next(next(next(*c))))))) &&
     is_ifeq(next(next(next(next(next(next(next(*c))))))), &f2) &&
     is_pop(next(next(next(next(next(next(next(next(*c))))))))) &&
     s4 == s4l){
    if (is_if_icmple(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmple(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmple(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_icmplt(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmplt(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmplt(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_icmpge(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmpge(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmpge(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_icmpne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmpne(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmpne(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_icmpgt(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmpgt(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmpgt(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_icmpeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_icmpeq(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_icmpeq(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_ifnonnull(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEifnonnull(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEifnonnull(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_ifnull(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEifnull(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEifnull(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_acmpeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_acmpeq(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_acmpeq(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_if_acmpne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEif_acmpne(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEif_acmpne(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_ifeq(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEifeq(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEifeq(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } else if (is_ifne(*c, &t3) && t3 == t3l) {
      if (uniquelabel(s4))
        return replace(c, 9, makeCODEifne(t3, makeCODEldc_int(k1, makeCODEgoto(f2, makeCODElabel(t3, NULL)))));
      else
        return replace(c, 9, makeCODEifne(t3, makeCODEldc_int(k1, makeCODElabel(s4, makeCODEgoto(f2, makeCODElabel(t3, NULL))))));
    } 
  }
  return 0;
}


 /*
  ifeq true_1                         0
  iconst_0                 x
  goto stop_2
  true_1:
  iconst_1             y
  stop_2:
  ifeq stop_0
  ------------->
  ifeq new
  goto stop_0
  new:                             true_1 = new:       stop_2 = stop_0
*/
int simplify_cryptic_labels(CODE **c)
{
  int x, y;
  int t1, s2, s0;
  int l;
  if(is_ldc_int(next(*c), &x) &&
     is_goto(next(next(*c)), &s2) &&
     is_label(next(next(next(*c))), &t1) &&
     is_ldc_int(next(next(next(next(*c)))), &y) &&
     is_label(next(next(next(next(next(*c))))), &s2) &&
     is_ifeq(next(next(next(next(next(next(*c)))))), &s0) && x == 0 && y == 1
     ) {
    if (!uniquelabel(t1))
      return 0;
    if (!uniquelabel(s2))
      return 0;
    droplabel(t1);
    droplabel(s2);
    l = next_label();
    INSERTnewlabel(l, "uncryptic", next(next(next(next(next(next(next(*c))))))), 1);
    if (is_ifeq(*c, &t1))
      return replace(c, 7, makeCODEifeq(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_ifne(*c, &t1))
      return replace(c, 7, makeCODEifne(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_acmpeq(*c, &t1))
      return replace(c, 7, makeCODEif_acmpeq(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_acmpne(*c, &t1))
      return replace(c, 7, makeCODEif_acmpne(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_ifnull(*c, &t1))
      return replace(c, 7, makeCODEifnull(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_ifnonnull(*c, &t1))
      return replace(c, 7, makeCODEifnonnull(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmpeq(*c, &t1))
      return replace(c, 7, makeCODEif_icmpeq(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmpgt(*c, &t1))
      return replace(c, 7, makeCODEif_icmpgt(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmplt(*c, &t1))
      return replace(c, 7, makeCODEif_icmplt(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmple(*c, &t1))
      return replace(c, 7, makeCODEif_icmple(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmpge(*c, &t1))
      return replace(c, 7, makeCODEif_icmpge(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
    else if (is_if_icmpne(*c, &t1))
      return replace(c, 7, makeCODEif_icmpne(l, makeCODEgoto(s0, makeCODElabel(l, NULL))));
  }
  return 0;
}

/*
  iconst_0
  aload_0
  swap
  putfield Test/x I
  ------>
  aload_0
  iconst_0
  putfield Test/x I
*/
int simplify_class_var_swap_int(CODE **c)
{ int x, y; char* arg;
  if (is_ldc_int(*c, &x) && is_aload(next(*c), &y) && is_swap(next(next(*c))) && is_putfield(next(next(next(*c))), &arg)) {
    return replace(c, 4, makeCODEaload(y, makeCODEldc_int(x, makeCODEputfield(arg, NULL))));
  }
  return 0;
}
/*
  ldc arg1
  aload x
  swap
  putfield arg2
  ------>
  aload x
  ldc arg1
  putfield arg2
*/
int simplify_class_var_swap_string(CODE **c)
{ int x; char* arg1, *arg2;
  if (is_ldc_string(*c, &arg1) && is_aload(next(*c), &x) && is_swap(next(next(*c))) && is_putfield(next(next(next(*c))), &arg2)) {
    return replace(c, 4, makeCODEaload(x, makeCODEldc_string(arg1, makeCODEputfield(arg2, NULL))));
  }
  return 0;
}

/*
  aload_0
  getfield Test/x I
  iconst_1
  iadd
  aload_0
  swap
  putfield Test/x I
  ------->
  aload_0
  aload_0
  getfield Test/x I
  iconst_1
  iadd
  putfield Test/x I
*/ 
int simplify_class_var_add(CODE **c)
{ int x, y, k; char* arg1, *arg2;
  if( is_aload(*c, &x) &&
      is_getfield(next(*c), &arg1) &&
      is_iadd(next(next(next(*c)))) &&
      is_aload(next(next(next(next(*c)))), &y) &&
      is_swap(next(next(next(next(next(*c)))))) &&
      is_putfield(next(next(next(next(next(next(*c)))))), &arg2)
    ) {
    if (is_ldc_int(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEldc_int(k, makeCODEiadd(makeCODEputfield(arg2, NULL)))))));
    else if (is_iload(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEiload(k, makeCODEiadd(makeCODEputfield(arg2, NULL)))))));
 
  }
  return 0;
}
int simplify_class_var_sub(CODE **c)
{ int x, y, k; char* arg1, *arg2;
  if( is_aload(*c, &x) &&
      is_getfield(next(*c), &arg1) &&
      is_isub(next(next(next(*c)))) &&
      is_aload(next(next(next(next(*c)))), &y) &&
      is_swap(next(next(next(next(next(*c)))))) &&
      is_putfield(next(next(next(next(next(next(*c)))))), &arg2)
    ) {
    if (is_ldc_int(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEldc_int(k, makeCODEisub(makeCODEputfield(arg2, NULL)))))));
    else if (is_iload(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEiload(k, makeCODEisub(makeCODEputfield(arg2, NULL)))))));
 
  }
  return 0;
}
int simplify_class_var_mul(CODE **c)
{ int x, y, k; char* arg1, *arg2;
  if( is_aload(*c, &x) &&
      is_getfield(next(*c), &arg1) &&
      is_imul(next(next(next(*c)))) &&
      is_aload(next(next(next(next(*c)))), &y) &&
      is_swap(next(next(next(next(next(*c)))))) &&
      is_putfield(next(next(next(next(next(next(*c)))))), &arg2)
    ) {
    if (is_ldc_int(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEldc_int(k, makeCODEimul(makeCODEputfield(arg2, NULL)))))));
    else if (is_iload(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEiload(k, makeCODEimul(makeCODEputfield(arg2, NULL)))))));
 
  }
  return 0;
}
int simplify_class_var_div(CODE **c)
{ int x, y, k; char* arg1, *arg2;
  if( is_aload(*c, &x) &&
      is_getfield(next(*c), &arg1) &&
      is_idiv(next(next(next(*c)))) &&
      is_aload(next(next(next(next(*c)))), &y) &&
      is_swap(next(next(next(next(next(*c)))))) &&
      is_putfield(next(next(next(next(next(next(*c)))))), &arg2)
    ) {
    if (is_ldc_int(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEldc_int(k, makeCODEidiv(makeCODEputfield(arg2, NULL)))))));
    else if (is_iload(next(next(*c)), &k))
    return replace(c, 7, makeCODEaload(y, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEiload(k, makeCODEidiv(makeCODEputfield(arg2, NULL)))))));
 
  }
  return 0;
}
/*
  dup
  aload x
  swap
  putfield arg
  pop
  --------->
  aload x
  swap
  putfield arg
*/

int simplify_assign_class_var(CODE **c)
{ int x; char *arg;
  if (is_dup(*c) && is_aload(next(*c), &x) && is_swap(next(next(*c))) &&
      is_putfield(next(next(next(*c))), &arg) && is_pop(next(next(next(next(*c)))))) {
    return replace(c, 5, makeCODEaload(x, makeCODEswap(makeCODEputfield(arg, NULL))));
  }
  return 0;
}

/*
  astore x
  aload x
  -------->
  dup
  astore x
*/

int simplify_astore_aload(CODE **c)
{ int x, y;
  if (is_astore(*c, &x) && is_aload(next(*c), &y) && x == y) {
    return replace(c, 2, makeCODEdup(makeCODEastore(x, NULL)));
  }
  return 0;
}

/* 
  A:                   B:

  dup                  dup
  istore x             istore x
  pop                  pop
  iload x              ----------->
  --------->           istore
  dup 
  istore x
*/

int simplify_istore(CODE **c)
{ int x, y;
  if (is_dup(*c) &&
      is_istore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
    if ( is_iload(next(next(next(*c))), &y) && x== y) {
      return replace(c, 4, makeCODEdup(makeCODEistore(x, NULL))); /*case A*/
    } else {
     return replace(c,3,makeCODEistore(x,NULL)); /*case B*/
   }
  }
  return 0;
}

/*

  iconst_0
  istore 4
  iconst_0
  ------->
  iconst_0
  dup
  istore 4
*/
int simplify_iconst_istore_iconst(CODE **c)
{ int x, y, k;
  if (is_ldc_int(*c, &x) && is_istore(next(*c), &y) && is_ldc_int(next(next(*c)), &k) && x == k) {
    return replace(c, 3, makeCODEldc_int(k, makeCODEdup(makeCODEistore(y, NULL))));
  }
  return 0;
}

/* 

   ldc_int 0
   iadd
   --------->

   src: https://frippery.org/jopt/opt2.html
*/

int simplify_add_zero(CODE **c)
{ int x;
  if (is_ldc_int(*c, &x) && is_iadd(next(*c)) && x == 0) {
    return replace(c, 2, NULL);
  }
  return 0;
}

/* 

   iload 4           aload 4
   iload 4           aload 4
   --------->        -------->
   iload 4           aload 4
   dup               dup

   src: https://frippery.org/jopt/opt4.html
   only optimize when local index > 3 because i(a)load_3/2/1 is faster
*/
int simplify_duplicate_variables(CODE **c)
{ int x, y; 
  if (is_iload(*c, &x) &&
      is_iload(next(*c), &y) && x == y && x > 3) {
    return replace(c, 2,makeCODEiload(x, makeCODEdup(NULL)));
  } else if (is_aload(*c, &x) &&
      is_aload(next(*c), &y) && x == y && x > 3) {
    return replace(c, 2,makeCODEaload(x, makeCODEdup(NULL)));
  }
  return 0;
}

/*
   aload%1
   getfield %2 I
   aload%1
   getfield %2 I
   =
   aload%1
   getfield %2 I
   dup
*/

int simplify_duplicate_class_variables(CODE **c)
{ int x, y; char *arg1, *arg2;
  if (is_aload(*c, &x) && is_getfield(next(*c), &arg1) &&
      is_aload(next(next(*c)), &y) && is_getfield(next(next(next(*c))), &arg2) &&
      x == y && strcmp(arg1, arg2) == 0) {
    return replace(c, 4, makeCODEaload(x, makeCODEgetfield(arg1, makeCODEdup(NULL))));
  }
  return 0;

}

/* aload x
   aload x
   -------->
   aload x
   dup
*/

int simplify_aload_aload(CODE **c)
{ int x, y;
  if (is_aload(*c, &x) && is_aload(next(*c), &y) && x == y) {
    return replace(c, 2, makeCODEaload(x, makeCODEdup(NULL)));
  }
  return 0;
}

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

/* iload x
   ldc 1
   idiv
   ------->
   iload x
*/

int simplify_division(CODE **c)
{ int x, k;
  if (is_iload(*c, &x) && is_ldc_int(next(*c), &k) && is_idiv(next(next(*c)))
      && k == 1) {
    return replace(c, 3, makeCODEiload(x, NULL));
  }
  return 0;
}

/* dup              dup
 * astore x         astore x
 * pop              pop
 * -------->        aload x
 * astore x         ---------->
                    dup
                    astore x
 */
int simplify_astore(CODE **c)
{ int x, y;
  if (is_dup(*c) &&
      is_astore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
    if (is_aload(next(next(next(*c))), &y) && x==y) {
      return replace(c, 4, makeCODEdup(makeCODEastore(x, NULL)));
    } else {
     return replace(c,3,makeCODEastore(x,NULL));
   }
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



int simplify_deadlabel(CODE **c) 
{ int l;
  if (is_label(*c, &l) && deadlabel(l)) {
    return replace(c, 1, NULL);
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
  ADD_PATTERN(simplify_duplicate_variables);
  ADD_PATTERN(simplify_duplicate_class_variables);
  ADD_PATTERN(simplify_add_zero);
  ADD_PATTERN(simplify_istore);
  ADD_PATTERN(simplify_astore_aload);
  ADD_PATTERN(simplify_division);
  ADD_PATTERN(simplify_assign_class_var);
  ADD_PATTERN(simplify_aload_aload);
  ADD_PATTERN(simplify_iconst_istore_iconst);
  ADD_PATTERN(simplify_class_var_add);
  ADD_PATTERN(simplify_class_var_mul);
  ADD_PATTERN(simplify_class_var_div);
  ADD_PATTERN(simplify_class_var_sub);
  ADD_PATTERN(simplify_class_var_swap_int);
  ADD_PATTERN(simplify_class_var_swap_string);
  ADD_PATTERN(simplify_cryptic_labels);
  ADD_PATTERN(simplify_cryptic_null_ifs);
  ADD_PATTERN(simplify_if_cmp_null);
  ADD_PATTERN(simplify_chained_and);
  ADD_PATTERN(simplify_jumping_ifs);
  ADD_PATTERN(simplify_dumb_ifeqs);
  ADD_PATTERN(simplify_dumb_ifnes);
  ADD_PATTERN(simplify_continuous_labels);
  ADD_PATTERN(simplify_if_goto_label);
  ADD_PATTERN(simplify_deadlabel);
  ADD_PATTERN(simplify_dup_labels);
  ADD_PATTERN(simplify_aload_iload_swap);
  ADD_PATTERN(simplify_aload_aload_swap);
  ADD_PATTERN(simplify_chained_and2);
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


