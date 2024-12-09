#include "obj.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>


ObjFile::ObjFile (const char *filename) : vertex_index (0), vertex_texture_index (0)
{
        this->obj_file = fopen (filename, "r");

        if (!this->obj_file) {
                std::perror ("fopen");
                exit (EXIT_FAILURE);
        }
}

char ObjFile::advance_char ()
{
        char old = curr_char;
        curr_char = fgetc (this->obj_file);

        return old;
}

char ObjFile::peek_char ()
{
        return this->curr_char;
}

bool ObjFile::match_char (char c)
{
        if (this->peek_char () == c) {
                this->advance_char ();
                return true;
        }

        return false;
}

struct ObjFile::Token ObjFile::parse_numeric ()
{
        double value = 0;

        while ('0' <= this->peek_char () && this->peek_char () <= '9') {
                value *= 10;

                value += this->peek_char () - '0';

                this->advance_char ();
        }

        if (!this->match_char ('.'))
                return (struct ObjFile::Token){ .type = INTEGER, .d = int (value) };

        int place = 1;
        while ('0' <= this->peek_char () && this->peek_char () <= '9') {
                value += (this->peek_char () - '0') / std::pow (10.0, place++);

                this->advance_char ();
        }

        return (struct ObjFile::Token){ .type = FLOAT, .f = value };
}

struct ObjFile::Token ObjFile::parse_keyword (char *keyword)
{
        enum TokenType type;
        if (strcmp (keyword, "vt") == 0) {
                type = VERTEX_TEXTURE;
        } else if (strcmp (keyword, "vn")) {
                type = VERTEX_NORMAL;
        } else if (strcmp (keyword, "v")) {
                type = VERTEX;
        } else if (strcmp (keyword, "f")) {
                type = FACE;
        } else {
                type = IDENT;
        }

        return (struct ObjFile::Token){ .type = type };
}

struct ObjFile::Token ObjFile::parse_token ()
{
        while (this->peek_char () != EOF) {
                switch (this->peek_char ()) {
                case ' ':
                case '\n':
                case '\r':
                case '\t': this->advance_char (); continue;
                case '/': {
                        this->advance_char ();

                        return (struct Token){ .type = SLASH };
                }

                default:

                        if ('0' <= this->peek_char () && this->peek_char () <= '9')
                                return this->parse_numeric ();

                        char ident[80];
                        char *p = ident;

                        while ('a' <= this->peek_char () && this->peek_char () <= 'z') {
                                *p = this->peek_char ();
                                p++;
                                this->advance_char ();
                        }

                        *p = '\0';

                        return this->parse_keyword (ident);
                }
        }
}

struct ObjFile::Token ObjFile::advance_token ()
{
        struct Token old = this->token;

        this->token = this->parse_token ();

        return old;
}

bool ObjFile::match_token (enum ObjFile::TokenType type)
{
        if (this->peek_token ().type == type) {
                this->advance_token ();
                return true;
        }

        return false;
}

Vec3 ObjFile::parse_vertex ()
{
        this->consume_token (ObjFile::VERTEX, "expected vertex declaration 'v'");

        struct ObjFile::Token x = this->consume_token (FLOAT, "expected x-coordinate for vertex");

        struct ObjFile::Token y = this->consume_token (FLOAT, "expected y-coordinate for vertex");

        struct ObjFile::Token z = this->consume_token (FLOAT, "expected z-coordinate for vertex");

        return Vec3 (x.f, y.f, z.f);
}

Vec3 ObjFile::parse_vertex_normal ()
{
        this->consume_token(ObjFile::VERTEX_NORMAL, "expected vertex normal 'vn'");

        struct ObjFile::Token x = this->consume_token (FLOAT, "expected x-coordinate for vertex");

        struct ObjFile::Token y = this->consume_token (FLOAT, "expected y-coordinate for vertex");

        struct ObjFile::Token z = this->consume_token (FLOAT, "expected z-coordinate for vertex");

        return Vec3 (x.f, y.f, z.f);
}

Triangle ObjFile::parse_face()
{

        this->consume_token(ObjFile::FACE, "expected face 'f'");

        

}