#include "wallfacing.h"

const WallFacing WallFacing::XPOS(Facing::XPOS);
const WallFacing WallFacing::XNEG(Facing::XNEG);
const WallFacing WallFacing::ZPOS(Facing::ZPOS);
const WallFacing WallFacing::ZNEG(Facing::ZNEG);
const WallFacing WallFacing::YPOS(Facing::YPOS);
const WallFacing WallFacing::YNEG(Facing::YNEG);

const QVector3D WallFacing::XNEG_normal(-1, 0, 0);
const QVector3D WallFacing::XPOS_normal(1, 0, 0);
const QVector3D WallFacing::ZNEG_normal(0, 0, -1);
const QVector3D WallFacing::ZPOS_normal(0, 0, 1);
const QVector3D WallFacing::YNEG_normal(0, -1, 0);
const QVector3D WallFacing::YPOS_normal(0, 1, 0);

const WallFacing WallFacing::allFacings[] = {
    WallFacing::XNEG,
    WallFacing::XPOS,
    WallFacing::YNEG,
    WallFacing::YPOS,
    WallFacing::ZNEG,
    WallFacing::ZPOS
};

WallFacing::WallFacing(Facing _facing) : facing(_facing) {

}

QVector3D WallFacing::getNormal(WallFacing face) {
    if (face == XPOS) {
        return XPOS_normal;
    }
    else if (face == XNEG) {
        return XNEG_normal;
    }
    else if (face == YPOS) {
        return YPOS_normal;
    }
    else if (face == YNEG) {
        return YNEG_normal;
    }
    else if (face == ZPOS) {
        return ZPOS_normal;
    }
    else if (face == ZNEG) {
        return ZNEG_normal;
    }

    return QVector3D(0, 0, 0);
}

QVector3D WallFacing::getNormal() const {
    // note for myself: implicitely converts WallFacing to int
    if (val() == XPOS) {
        return XPOS_normal;
    }
    else if (val() == XNEG) {
        return XNEG_normal;
    }
    else if (val() == YPOS) {
        return YPOS_normal;
    }
    else if (val() == YNEG) {
        return YNEG_normal;
    }
    else if (val() == ZPOS) {
        return ZPOS_normal;
    }
    else if (val() == ZNEG) {
        return ZNEG_normal;
    }
    return QVector3D(0, 0, 0);
}

WallFacing WallFacing::right() const {
    if (val() == XPOS) {
        return YPOS;
    }
    else if (val() == XNEG) {
        return ZPOS;
    }
    else if (val() == YPOS) {
        return ZPOS;
    }
    else if (val() == YNEG) {
        return XPOS;
    }
    else if (val() == ZPOS) {
        return XPOS;
    }
    else if (val() == ZNEG) {
        return YPOS;
    }
    return XPOS;
}

WallFacing WallFacing::up() const {
    if (val() == XPOS) {
        return ZPOS;
    }
    else if (val() == XNEG) {
        return YPOS;
    }
    else if (val() == YPOS) {
        return XPOS;
    }
    else if (val() == YNEG) {
        return ZPOS;
    }
    else if (val() == ZPOS) {
        return YPOS;
    }
    else if (val() == ZNEG) {
        return XPOS;
    }
    return XPOS;
}

WallFacing WallFacing::inverse() const {
    if (val() == XPOS) {
        return XNEG;
    }
    else if (val() == XNEG) {
        return XPOS;
    }
    else if (val() == YPOS) {
        return YNEG;
    }
    else if (val() == YNEG) {
        return YPOS;
    }
    else if (val() == ZPOS) {
        return ZNEG;
    }
    else if (val() == ZNEG) {
        return ZPOS;
    }
    return XPOS;
}

WallFacing WallFacing::fromNormal(QVector3D vec) {
    double min = 43833;
    WallFacing result;
    //const WallFacing (&all)[] = allFacings;
    for (const WallFacing &facing : allFacings) {
        QVector3D temp(facing.getNormal());
        double val = (temp - vec).length();
        if (val < min) {
            result = facing;
            min = val;
        }
    }

    return result;
}

WallFacing WallFacing::rotate(WallFacing orient) const {
    QVector3D vec = getNormal();

    // Vector rotation around orient
    QVector3D norm = orient.getNormal();
    double angle = M_PI/2;
    QVector3D rot = vec * cos(angle) + QVector3D::crossProduct(vec, norm) * sin(angle) + norm * QVector3D::dotProduct(norm, vec) * (1 - sin(angle));

    return fromNormal(rot);
}