#include "obj.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <vector>

ObjFile::ObjFile (const char *filename) : vertex_index (0), vertex_texture_index (0), parsed (false)
{
        this->obj_file = fopen (filename, "r");

        if (!this->obj_file) {
                std::perror ("fopen");
                exit (EXIT_FAILURE);
        }
}

ObjFile::~ObjFile ()
{
        fclose (this->obj_file);
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
        bool is_negative = false;
        if (this->match_char ('-'))
                is_negative = true;

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

        return (struct ObjFile::Token){ .type = FLOAT, .f = is_negative ? -value : value };
}

struct ObjFile::Token ObjFile::parse_keyword (char *keyword)
{
        enum TokenType type;
        if (strcmp (keyword, "vt") == 0) {
                type = VERTEX_TEXTURE;
        } else if (strcmp (keyword, "vn") == 0) {
                type = VERTEX_NORMAL;
        } else if (strcmp (keyword, "v") == 0) {
                type = VERTEX;
        } else if (strcmp (keyword, "f") == 0) {
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
                case '-': return this->parse_numeric ();
                case '#': {
                        while (this->peek_char () != '\n')
                                this->advance_char ();
                        continue;
                }
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

        return (struct ObjFile::Token){ .type = END };
}
struct ObjFile::Token ObjFile::peek_token ()
{
        return this->token;
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

struct ObjFile::Token ObjFile::consume_token (enum ObjFile::TokenType type, const char *error)
{
        if (this->peek_token ().type != type) {
                throw std::logic_error (error);
        }

        return this->advance_token ();
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
        this->consume_token (ObjFile::VERTEX_NORMAL, "expected vertex normal 'vn'");

        struct ObjFile::Token x = this->consume_token (FLOAT, "expected x-coordinate for vertex");

        struct ObjFile::Token y = this->consume_token (FLOAT, "expected y-coordinate for vertex");

        struct ObjFile::Token z = this->consume_token (FLOAT, "expected z-coordinate for vertex");

        return Vec3 (x.f, y.f, z.f);
}

Vec3 ObjFile::parse_uv ()
{
        this->consume_token (ObjFile::VERTEX_TEXTURE, "expected vertex normal 'vt'");

        struct ObjFile::Token x = this->consume_token (FLOAT, "expected u-coordinate for vertex texture coordinate");

        struct ObjFile::Token y = this->consume_token (FLOAT, "expected v-coordinate for vertex texture coordinate");

        return Vec3 (x.f, y.f, 0);
}

ObjFile::FaceTriple ObjFile::parse_face_indicies ()
{
        FaceTriple triple = { std::nullopt, std::nullopt, std::nullopt };

        struct ObjFile::Token vertex_index = this->consume_token (ObjFile::INTEGER, "expected vertex index");
        std::get<0> (triple) = vertex_index.d;

        if (!this->match_token (SLASH))
                return triple;

        struct ObjFile::Token current_token = this->peek_token ();
        if (this->match_token (ObjFile::INTEGER)) {
                std::get<1> (triple) = current_token.d;
        }

        if (!this->match_token (SLASH)) {
                return triple;
        }

        current_token = this->peek_token ();
        if (this->match_token (ObjFile::INTEGER)) {
                std::get<2> (triple) = current_token.d;
        }

        return triple;
}

ObjFile::Face ObjFile::parse_face ()
{
        this->consume_token (ObjFile::FACE, "expected face 'f'");

        Face face;

        while (this->peek_token ().type == INTEGER) {
                FaceTriple triple = this->parse_face_indicies ();
                face.emplace_back (triple);
        }

        return face;
}

std::vector<ObjFile::Face> ObjFile::fan_triangulate_face (Face &face)
{
}

void ObjFile::parse ()
{
        if (this->parsed)
                return;

        this->advance_char ();
        this->advance_token ();
        size_t vertex_index = 0;
        size_t normal_index = 0;
        while (!this->match_token (END)) {
                struct Token command = this->peek_token ();

                switch (command.type) {
                case VERTEX: {
                        Vec3 vertex = this->parse_vertex ();
                        this->vertex_map[vertex_index++] = vertex;
                        break;
                }
                case VERTEX_TEXTURE: {
                        Vec3 texture = this->parse_uv ();
                        this->texture_map[vertex_texture_index++] = texture;
                        break;
                }
                case VERTEX_NORMAL: {
                        Vec3 normal = this->parse_vertex_normal ();
                        this->normal_map[normal_index++] = normal;
                        break;
                }
                case FACE: {
                        faces.emplace_back (this->parse_face ());
                        break;
                }
                default: throw std::logic_error ("error parsing .obj file: unknown command");
                }
        }

        this->parsed = true;
}