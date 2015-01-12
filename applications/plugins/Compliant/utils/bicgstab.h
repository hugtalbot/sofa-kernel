#ifndef COMPLIANT_UTILS_CG_H
#define COMPLIANT_UTILS_CG_H

#include <cassert>
#include "krylov.h"


// Biconjugate gradient stabilized solver
// http://en.wikipedia.org/wiki/Biconjugate_gradient_stabilized_method

// example use:

// typedef bicgstab<double> solver;
// solver::params p;
// p.iterations = 100;
// p.precision = 1e-16;
// solver::vec x, b;
// solver::solve(x, A, b);

template<class U>
struct bicgstab
{

    typedef ::krylov<U> krylov;

    typedef typename krylov::vec vec;
    typedef typename krylov::real real;
    typedef typename krylov::natural natural;
    typedef typename krylov::params params;

    // solves Ax = b using bicgstab.
    // @A is a function object vec -> vec implementing matrix multiplication
    template<class Matrix>
    static void solve(vec& x, const Matrix& A, const vec& b, params& p)
    {
        vec residual = b;

        // deal with warm start
        if( !x.size() )
        {
            x = vec::Zero( b.size() );
        }
        else
        {
            assert(x.size() == b.size() );
            residual -= A(x);
        }

        // easy peasy
        data d;

		d.residual( residual );

        natural i;
        for( i = 0; i < p.iterations && d.phi > p.precision; ++i)
        {
            d.step(x, A);
        }
        p.iterations = i;
        p.precision = d.phi;

    }


    // contains all the data needed for cg iterations
    struct data
    {

        vec p;			// descent direction

        vec r, r0;			// residual

        vec v;

        vec s, t;




        vec Ap;			// A(p)

        real phi;
        real omega;
        real alpha;
        real rho, rho_prev;

        natural k;		// iteration

        void residual( const vec& rr ) {

            const typename vec::ConstantReturnType zero = vec::Zero(rr.size());

			r = rr;
            r0 = r;
            p = zero;
            v = zero;

            rho_prev = omega = alpha = 1;

            k = 1;

            phi = r.norm();
        }


        // performs one bicgstab step. returns false on singularity, true
        // otherwise. @A is a function object vec -> vec
        template<class Matrix>
        bool step(vec& x, const Matrix& A)
        {
            if( !phi ) return true;

            rho = r0.dot(r);

            if( !rho_prev || !omega ) return false;

            real beta = (rho/rho_prev)/(alpha/omega);

            p = r + beta * ( p - omega*v );

            v = A(p);

            real denom = r0.dot(v);
            if( !denom ) return false;

            alpha = rho / denom;

            s = r - alpha * v;

            t = A(s);

            denom = t.dot(t);
            if( !denom ) return false;

            omega = t.dot(s) / denom;

            x += alpha*p + omega*s;

            r = s - omega*t;

            rho_prev = rho;


            phi = r.norm();


            return true;
        }


    };


};



#endif
