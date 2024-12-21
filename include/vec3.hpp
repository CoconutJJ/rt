#pragma once
#include <ostream>
class Vec3 {
    public:
        double x;
        double y;
        double z;
        Vec3 ();
        Vec3 (double x, double y, double z);
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
        bool operator== (Vec3 b);
        bool operator< (Vec3 b) const;
        double &operator[] (int dim);
        double operator[] (int index) const;
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
        static Vec3 random ();
        static Vec3 zero ();
        static Vec3 inf ();
        Vec3 rotate (Vec3 axis, double angle);
        friend Vec3 operator* (double scalar, Vec3 a);
        friend std::ostream &operator<< (std::ostream &out, Vec3 v);

    private:
        double argument (double y_opp, double x_adj);
};
