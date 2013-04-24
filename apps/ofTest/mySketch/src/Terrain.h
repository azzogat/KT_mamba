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
  void Draw(unsigned int shaderID);
  void AdjustHeight(float diff,float x,float z,float radius);
  void HighLightPosition(float x,float z,float radius);
  void Reset();
  ~Terrain();
private:
  Terrain():isGeomDirty(false){}

  void UpdateVBO();
  HeightField m_heightField;
  ofVbo m_meshVbo;

  std::vector<unsigned int> indexBuffer;
  std::vector<ofVec3f> posBuffer;
  std::vector<ofVec3f> normalBuffer;
  std::vector<ofVec2f> uvBuffer;
  std::vector<ofVec3f> colorBuffer;
  float planeWidth;
  float planeDepth;
  bool isGeomDirty;
  bool isColorDirty;
  unsigned int vao;
  unsigned int vboPos;
  unsigned int vboNormal;
  unsigned int vboUV;
  unsigned int vboColor;
  unsigned int vboIndex;
  unsigned int grassTex;
  unsigned int sandTex;
  unsigned int rockTex;
  float lastx;
  float lastz;
};
#endif // Terrain_h__
