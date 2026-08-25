#include "objparser.h"
#include <fstream>
#include <QDebug>

ObjParser::ObjParser() {}

void ObjParser::parse(Mesh &mesh) {
    std::string stringified = constructObjString(mesh.vertices, mesh.normals, mesh.faces);
    //try {
    std::ofstream writer("C:/Users/PC/Documents/3DMaze/output.obj");
    if (!writer.is_open()) {
        qDebug() << "Error: Could not open or create the file!";
    }
        writer << stringified;
        writer.close();
        qDebug() << "Finished!";
    /*}
    catch (IOException exception) {
        System.out.println("Error reading file: ");
        exception.printStackTrace();
    }*/
}

std::string ObjParser::constructObjString(std::vector<QVector3D> &vertices, std::vector<QVector3D> &normals, std::vector<Face*> &faces) {
    // TODO: add an option to flip Y and Z (so Z is the "up" axis)
    std::string text = "# Automatically generated file\n\n";
    text.append("o Object\n");
    for (QVector3D vertex : vertices) {
        text += "v " + std::to_string(vertex.x()) + " " + std::to_string(vertex.y()) + " " + std::to_string(vertex.z()) + "\n";
    }

    for (QVector3D vertex : normals) {
        text += "vn " + std::to_string(vertex.x()) + " " + std::to_string(vertex.y()) + " " + std::to_string(vertex.z()) + "\n";
    }

    for (Face* face : faces) {
        int normal = face->normal;
        std::array<int, 4> indices = face->indices;
        text += "f ";
        for (int index : indices) {
            text += std::to_string(index) + "//" + std::to_string(normal) + " ";
        }
        text += "\n";
    }
    return text;
}