enum Margin {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT
};

// HandData is intended to solve a common problem where we are only interested
// in a specific range for a particular value. Example:
// We are fed a x-coordinate value from a Kinect sensor: 480 (out of a possible
// 0 minimum and 640 maximum). However, we decided that we only want to work
// with the range between 320 and 600 where 320 should equate to 0 and 600
// should equate to 1. The solution is to take the input value, subtract the
// lower margin and divide by the range of interest (between 320 and 600, so
// the range is 280) to yield a value normalised to 1. Any result below the
// minimum (0) can be considered outside our range of interest and seen as 0.
// Similarly, any value over the 1 can be seen as the maximum (1).
// eg. value 480 would yield (480 - 320) / 280 = 0.5714285714285714
class HandCoords {
  float _offsetX, _offsetY, _offsetZ;
  float _rangeX, _rangeY, _rangeZ;
  float _dead[4]; // in case the sensor has aparent dead margins around sensor edge

public:
  float x, y, z;

  HandCoords::HandCoords()
    : x(0.0f), y(0.0f), z(0.0f)
  {
    _offsetX = 0.0f;
    _offsetY = 0.0f;
    _offsetZ = 0.0f;
    _rangeX = 0.0f;
    _rangeY = 0.0f;
    _rangeZ = 0.0f;
    _dead[TOP] = 0.0f;
    _dead[RIGHT] = 0.0f;
    _dead[BOTTOM] = 0.0f;
    _dead[LEFT] = 0.0f;
  }

  HandCoords::HandCoords(float offsetX, float offsetY, float offsetZ, float rangeX, float rangeY, float rangeZ)
    : x(0.0f), y(0.0f), z(0.0f)
  {
    _offsetX = offsetX;
    _offsetY = offsetY;
    _offsetZ = offsetZ;
    _rangeX = rangeX;
    _rangeY = rangeY;
    _rangeZ = rangeZ;
    _dead[TOP] = 0.0f;
    _dead[RIGHT] = 0.0f;
    _dead[BOTTOM] = 0.0f;
    _dead[LEFT] = 0.0f;
  }

  void HandCoords::set(float xIn, float yIn, float zIn)  {
     xIn?x = min(max((xIn - _offsetX - _dead[LEFT]) / (_rangeX - _dead[LEFT] - _dead[RIGHT]), 0.0f), 1.0f):0.0f;
     yIn?y = min(max((yIn - _offsetY - _dead[TOP]) / (_rangeY - _dead[TOP] - _dead[BOTTOM]), 0.0f), 1.0f):0.0f;
     zIn?z = min(max((zIn - _offsetZ) / _rangeZ, 0.0f), 1.0f):0.0f;
  }

  void HandCoords::setDead(float * margin) {
    _dead[TOP] = margin[TOP];
    _dead[RIGHT] = margin[RIGHT];
    _dead[BOTTOM] = margin[BOTTOM];
    _dead[LEFT] = margin[LEFT];
  }
};