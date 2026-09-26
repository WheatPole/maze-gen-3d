#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "mesh.h"

class ObjParser
{
public:
    ObjParser();
    static void parse(Mesh &mesh);
    static std::string constructObjString(Mesh &mesh) {
        return constructObjString(mesh.vertices, mesh.normals, mesh.faces);
    }
    static std::string constructObjString(std::vector<QVector3D> &vertices, std::vector<QVector3D> &normals, std::vector<Face*> &faces);
};

#endif // OBJPARSER_H
