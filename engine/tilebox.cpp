#include "tilebox.h"

TileBox::TileBox(QVector3D _up, Quad _bottomQuad, std::array<Quad, 6> _innerQuads)
    : up(_up), bottomQuad(_bottomQuad), innerQuads{_innerQuads} {

}

// Note: this actually doesn't work (not used anyway), as origin isn't changed at all
Quad TileBox::getQuad(WallFacing facing) {
    Quad res(bottomQuad);
    switch (facing.facing) {
        case Facing::YNEG:
                res.right = bottomQuad.up;
                res.up = bottomQuad.right;
                break;
        case Facing::YPOS: {
            //res = Origin(res.getOrigin().add(up));
            break;
        }
        case Facing::XNEG: {
            res.right = bottomQuad.up;
            res.up = up;
            break;
        }
        case Facing::XPOS: {
            res.right = up;
            res.up = bottomQuad.up;
            break;
        }
        case Facing::ZNEG: {
            res.right = up;
            res.up = bottomQuad.right;
            break;
        }
        case Facing::ZPOS: {
            res.right = bottomQuad.right;
            res.up = up;
            break;
        }
    }
    return res;
}
Quad TileBox::getInnerQuad(WallFacing facing) {
    Quad res = innerQuads[0];
    switch (facing.facing) {
        case Facing::XNEG: res = innerQuads[0]; break;
        case Facing::XPOS: res = innerQuads[1]; break;
        case Facing::YNEG: res = innerQuads[2]; break;
        case Facing::YPOS: res = innerQuads[3]; break;
        case Facing::ZNEG: res = innerQuads[4]; break;
        case Facing::ZPOS: res = innerQuads[5]; break;
    };
        return res;
}