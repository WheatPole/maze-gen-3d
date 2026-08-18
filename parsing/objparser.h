#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "mesh.h"

class ObjParser
{
public:
    ObjParser();
    static void parse(Mesh &mesh);
    static std::string constructObjString(std::vector<QVector3D> &innerVertices, std::vector<QVector3D> &outerVertices, std::vector<QVector3D> &normals, std::vector<Face*> &faces);
};

#endif // OBJPARSER_H
