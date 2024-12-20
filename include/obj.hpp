#pragma once

#include "vec3.hpp"
#include <cstddef>
#include <cstdio>
#include <map>
#include <optional>
#include <vector>
class ObjFile {
    public:
        enum TokenType { INTEGER, FLOAT, IDENT, VERTEX, VERTEX_TEXTURE, VERTEX_NORMAL, FACE, SLASH, END };

        struct Token {
                enum TokenType type;

                union {
                        int d;
                        double f;
                        char ident[32];
                };
        };

        using FaceTriple = std::tuple<std::optional<int>, std::optional<int>, std::optional<int> >;
        using Face = std::vector<FaceTriple>;

        bool parsed;
        bool is_triangular_mesh;
        char curr_char;
        std::map<size_t, Vec3> vertex_map;
        std::map<size_t, Vec3> texture_map;
        std::map<size_t, Vec3> normal_map;
        std::vector<Face> faces;

        struct Token token;
        FILE *obj_file;

        size_t vertex_index;
        size_t vertex_texture_index;
        ObjFile (const char *filename);
        ~ObjFile ();
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

        FaceTriple parse_face_indicies ();

        Vec3 parse_uv ();
        Vec3 parse_vertex_normal ();
        Vec3 parse_vertex ();
        Face parse_face ();

        std::vector<Face> fan_triangulate_face (Face &face);
        void fan_triangulate ();

        void parse ();
};
