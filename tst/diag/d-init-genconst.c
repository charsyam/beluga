int x = 3.14;
int *p = &x;
int *q = p;                      /* error */
int *r1 = 0, *r2 = (void *)0;
int *s = ((void *)0, 0);         /* error */
