#ifndef WALLFACING_H
#define WALLFACING_H

#include <cstdint>
#include <cmath>
#include <QVector3D>

enum class Facing {
    XPOS = 1,
    XNEG = 2,
    YPOS = 4,
    YNEG = 8,
    ZPOS = 16,
    ZNEG = 32
};

class WallFacing {
public:
    static const WallFacing XPOS;
    static const WallFacing XNEG;
    static const WallFacing ZPOS;
    static const WallFacing ZNEG;
    static const WallFacing YPOS;
    static const WallFacing YNEG;

    const static WallFacing allFacings[6];
private:
    const static QVector3D XNEG_normal;
    const static QVector3D XPOS_normal;
    const static QVector3D ZNEG_normal;
    const static QVector3D ZPOS_normal;
    const static QVector3D YNEG_normal;
    const static QVector3D YPOS_normal;

public:
    Facing facing;

    explicit WallFacing(Facing _facing = Facing::XNEG);
    constexpr operator int() const { return static_cast<int>(facing); }
    constexpr inline int val() const { return static_cast<int>(facing); }

    static QVector3D getNormal(WallFacing face);

    QVector3D getNormal() const;
    // Returns the right vector facing of the current facing,
    // such that the origin is always in the corner closest to the actual origin of the cube
    // (so that would only include (0,0,0), (0,0,1), (0,1,0), (1,0,0) for corners)
    // this barely makes sense technically...
    WallFacing right() const;

    // Similar as above, but for the up vector facing
    WallFacing up() const;

    WallFacing inverse() const;

    // Find the closest matching normal
    static WallFacing fromNormal(QVector3D vec);

    WallFacing rotate(WallFacing orient) const;

    inline bool isPositive() const {
        return (val() == XPOS || val() == ZPOS || val() == YPOS);
    }

    /*inline const static WallFacing* allFacings() {
        return allArray;
    }*/
};

#endif // WALLFACING_H
