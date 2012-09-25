/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

/*
 Authors: Ben Leslie
 Description: 
  Complex arithmetic as per 7.3
 Status: Functions defined, not implemented
*/

#error The complex library is incomplete

#define complex _Complex
#define _Complex_I (const float _Complex) foo

#define I _Complex_I

/* 7.3.5 Trigonometric functions */

/* 7.3.5.1 cacos */
double complex cacos(double complex z);
float complex cacosf(float complex z);
long double complex cacosl(long double complex z);

/* 7.3.5.2 casin */
double complex casin(double complex z);
float complex casinf(float complex z);
long double complex casinl(long double complex z);

/* 7.3.5.3 catan */
double complex catan(double complex z);
float complex catanf(float complex z);
long double complex catanl(long double complex z);

/* 7.3.5.4 ccos */
double complex ccos(double complex z);
float complex ccosf(float complex z);
long double complex ccosl(long double complex z);

/* 7.3.5.5 csin */
double complex csin(double complex z);
float complex csinf(float complex z);
long double complex csinl(long double complex z);

/* 7.3.5.6 ctan */
double complex ctan(double complex z);
float complex ctanf(float complex z);
long double complex ctanl(long double complex z);

/* 7.3.6 Hyperbolic functions */

/* 7.3.6.1 cacosh */
double complex cacosh(double complex z);
float complex cacoshf(float complex z);
long double complex cacoshf(long double complex z);

/* 7.3.6.2 casinh */
double complex casinh(double complex z);
float complex casinhf(float complex z);
long double complex casinhl(long double complex z);

/* 7.3.6.3 catanh */
double complex catanh(double complex z);
float complex catanhf(float complex z);
long double complex catanhl(long double complex z);

/* 7.3.6.4 ccosh */
double complex ccosh(double complex z);
float complex ccoshf(float complex z);
long double complex ccoshl(long double complex z);

/* 7.3.6.5 csinh */
double complex csinh(double complex z);
float complex csinhf(float complex z);
long double complex csinhl(long double complex z);

/* 7.3.6.6 ctanh */
double complex ctanh(double complex z);
float complex ctanhl(float complex z);
long double complex ctanhl(long double complex z);

/* 7.3.7 Exponential and logarithmic functions */

/* 7.3.7.1 cexp */
double complex cexp(double complex z);
float complex cexpf(float complex z);
long double complex cexpl(long double complex z);

/* 7.3.7.2 clog */
double complex clog(double complex z);
float complex clogf(float complex z);
long double complex clogl(long double complex z);

/* 7.3.8 Power and absolute value functions */

/* 7.3.8.1 cabs */
double complex cabs(double complex z);
float complex cabsf(float complex z);
long double complex cabsl(long double complex z);

/* 7.3.8.2 cpow */
double complex cpow(double complex z);
float complex cpowf(float complex z);
long double complex cpowl(long double complex z);

/* 7.3.8.3 csqrt */
double complex csqrt(double complex z);
float complex csqrtf(float complex z);
long double complex csqrtl(long double complex z);

/* 7.3.9 Manipulation functions */

/* 7.3.9.1 carg */
double complex carg(double complex z);
float complex cargf(float complex z);
long double complex cargl(long double complex z);

/* 7.3.9.2 cimag */
double complex cimag(double complex z);
float complex cimagf(float complex z);
long double complex cimagl(long double complex z);

/* 7.3.9.3 conj */
double complex conj(double complex z);
float complex conjf(float complex z);
long double complex conjl(long double complex z);

/* 7.3.9.4 cproj */
double complex cproj(double complex z);
float complex cprojf(float complex z);
long double complex cprojl(long double complex z);

/* 7.3.9.5 creal */
double complex creal(double complex z);
float complex crealf(float complex z);
long double complex creall(long double complex z);
