#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct vec3 {
    float x, y, z;
};

struct vec2 {
    float x, y;
};

struct vertex_data {
    std::vector<vec3> verticies;
    std::vector<vec2> tex_coords;
    std::vector<vec3> vertex_normals;
};

bool parse_face(const std::string& face_data,
                int vertex_index[3],
                int texture_index[3],
                int normal_index[3]) {
    int matches = std::sscanf(face_data.c_str(),
                              "f %d/%d/%d %d/%d/%d %d/%d/%d",
                              &vertex_index[0], &texture_index[0], &normal_index[0],
                              &vertex_index[1], &texture_index[1], &normal_index[1],
                              &vertex_index[2], &texture_index[2], &normal_index[2]);
    return matches == 9;
}

int load_model_obj(char *path, std::vector<vertex_data> *o_vertex) {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        std::cerr << "Error: Unable to open OBJ file." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<vec3> verticies;
    std::vector<vec3> vertex_normals;
    std::vector<vec2> tex_coords;
    std::vector<vertex_data> *result = o_vertex;

    while (std::getline(stream, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            vec3 tmp = {};
            ss >> tmp.x >> tmp.y >> tmp.z;
            verticies.push_back(tmp);
        } else if (prefix == "vn") {
            vec3 tmp = {};
            ss >> tmp.x >> tmp.y >> tmp.z;
            vertex_normals.push_back(tmp);
        } else if (prefix == "vt") {
            vec2 tmp = {};
            ss >> tmp.x >> tmp.y;
            tex_coords.push_back(tmp);
        } else if (prefix == "f") {
            std::string face_data = ss.str();
            int vertex_index[3], texture_index[3], normal_index[3];

            if (!parse_face(face_data, vertex_index, texture_index, normal_index)) {
                std::cerr << "Error: Invalid face format: " << face_data << std::endl;
                continue;
            }

            vertex_data vertex = {};
            for (int i = 0; i < 3; ++i) {
                if (vertex_index[i] <= 0 || vertex_index[i] > verticies.size() ||
                    texture_index[i] <= 0 || texture_index[i] > tex_coords.size() ||
                    normal_index[i] <= 0 || normal_index[i] > vertex_normals.size()) {
                    std::cerr << "Error: Face indices out of range in: " << face_data << std::endl;
                    continue;
                }

                vertex.verticies.push_back(verticies[vertex_index[i] - 1]);
                vertex.tex_coords.push_back(tex_coords[texture_index[i] - 1]);
                vertex.vertex_normals.push_back(vertex_normals[normal_index[i] - 1]);
            }
            result->push_back(vertex);
        }
    }

    std::cout << "Successfully parsed " << result->size() << " faces." << std::endl;
    stream.close();
    return 0;
}
