#pragma once

#include "triangle.hpp"
#include "vec3.hpp"
#include <cstddef>
#include <cstdio>
#include <map>
class ObjFile {
    public:
        enum TokenType { INTEGER, FLOAT, IDENT, VERTEX, VERTEX_TEXTURE, VERTEX_NORMAL, FACE, SLASH };

        struct Token {
                enum TokenType type;

                union {
                        int d;
                        double f;
                        char ident[32];
                };
        };
        char curr_char;
        std::map<size_t, Vec3> vertex_map;
        std::map<size_t, Vec3> texture_map;
        struct Token token;
        FILE *obj_file;

        size_t vertex_index;
        size_t vertex_texture_index;
        ObjFile (const char *filename);

        char peek_char ();
        bool match_char (char c);
        char advance_char ();

        bool match_token (TokenType type);
        struct Token parse_token ();
        struct Token advance_token ();
        struct Token peek_token ();
        struct Token consume_token (enum TokenType type, const char *error);
        struct Token parse_numeric ();
        struct Token parse_keyword (char *keyword);

        Vec3 parse_uv ();
        Vec3 parse_vertex_normal ();
        Vec3 parse_vertex ();
        Triangle parse_face ();
};
