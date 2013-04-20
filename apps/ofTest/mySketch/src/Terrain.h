#ifndef Terrain_h__
#define Terrain_h__

#include "ofVec3f.h"
#include "ofVbo.h"

struct HeightField {
  int xDim;
  int zDim;
  float** values;
};

class Terrain {
public:
  static Terrain* Create(float width,float depth,int numHorizontalVerts,int numVerticalVerts, ofVec3f centre);
  void Update();
  void Draw();
  void AdjustHeight(float diff,float x,float z,float radius);
  ~Terrain();
private:
  Terrain():isDirty(false){}

  void RecalculateTerrain();
  HeightField m_heightField;
  ofVbo m_meshVbo;

  std::vector<unsigned int> indexBuffer;
  std::vector<ofVec3f> posBuffer;
  std::vector<ofVec3f> normalBuffer;
  std::vector<ofVec2f> uvBuffer;
  std::vector<ofFloatColor> colorBuffer;
  float planeWidth;
  float planeDepth;
  bool isDirty;
};
#endif // Terrain_h__
