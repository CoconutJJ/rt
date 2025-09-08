#pragma once
#include <ostream>

#ifdef __APPLE__
#define USE_ACCELERATE
#endif

#ifdef USE_ACCELERATE
#include <simd/simd.h>
#endif

class Vec3 {
    public:
#ifdef USE_ACCELERATE
        simd_double3 vec;
        Vec3 ();
        Vec3 (const Vec3 &other);
        Vec3 (simd_double3 v);
        Vec3 (double x, double y, double z);
#else
        double x;
        double y;
        double z;
        Vec3 ();
        Vec3 (const Vec3 &other);
        Vec3 (double x, double y, double z);
#endif

        Vec3 &operator= (Vec3 other);

        // arithmetic
        Vec3 operator+ (Vec3 a);
        Vec3 operator+ (double d);
        Vec3 operator+= (double d);
        Vec3 operator- (Vec3 a);
        Vec3 operator- ();
        Vec3 operator* (double d);
        Vec3 operator* (Vec3 b);
        Vec3 operator/ (double d);
        Vec3 operator/= (double d);
        Vec3 operator+= (Vec3 b);
        Vec3 operator-= (Vec3 b);
        Vec3 operator*= (double d);

        // comparisons
        bool operator== (Vec3 b);
        bool operator< (Vec3 b) const;

#ifndef USE_ACCELERATE
        double &operator[] (int dim);
#endif
        double operator[] (int index) const;

        // math
        double dot (Vec3 b);
        double length_squared ();
        double length ();
        Vec3 cross (Vec3 b);
        Vec3 unit ();
        Vec3 clamp (double min, double max);
        Vec3 reflect (Vec3 normal);
        Vec3 refract (Vec3 n, double mu);
        Vec3 sph ();
        Vec3 sph_inv ();
        bool near_zero ();
        Vec3 rotate (Vec3 axis, double angle);

        // factories
        static Vec3 random_hemisphere ();
        static Vec3 random ();
        static Vec3 zero ();
        static Vec3 inf ();

        // friends
        friend Vec3 operator* (double scalar, Vec3 a);
        friend std::ostream &operator<< (std::ostream &out, Vec3 v);

    private:
        double argument (double y_opp, double x_adj);
};
