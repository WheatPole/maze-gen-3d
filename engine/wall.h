#ifndef WALL_H
#define WALL_H

#include "wallfacing.h"

class Wall {
private:
    uint8_t bin;
public:
    Wall(uint8_t _bin);
    void add(WallFacing face) {
        bin |= face.val();
    }
    void remove(WallFacing face) {
        bin ^= face.val();
    }

    void set(uint8_t _bin) { bin = _bin; }

    inline bool exists(WallFacing face) const {
        return ((bin & face.val()) > 0);
    }
};

#endif // WALL_H
