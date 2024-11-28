#pragma once
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

        double dot (Vec3 b);
        double length_squared ();
        double length ();
        Vec3 elem_mul (Vec3 b);
        Vec3 cross (Vec3 b);
        Vec3 unit ();
        Vec3 clamp (double min, double max);
        Vec3 reflect (Vec3 normal);
        Vec3 refract (Vec3 n, double mu);
        bool near_zero ();
        static Vec3 random ();
        static Vec3 zero ();
};
