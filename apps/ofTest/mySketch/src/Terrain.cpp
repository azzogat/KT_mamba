#include "Terrain.h"
#include "ofVec2f.h"

float Saturate(float val){
  if (val < 0.0f) {
    return 0;
  }
  if (val > 1.0f) {
    return 1.0f;
  }
  return val;
};
float SampleHeightField(HeightField& field,float u, float v) {

  int pixelX = Saturate(u)*(field.xDim-1);
  int pixelY = Saturate(v)*(field.zDim-1);

  return field.values[pixelX][pixelY];
}



Terrain* Terrain::Create(float width,float depth,int numHorizontalVerts,int numVerticalVerts, ofVec3f centre) {
  Terrain* tmp = new Terrain();

  tmp->m_heightField.xDim = numHorizontalVerts;
  tmp->m_heightField.zDim = numVerticalVerts;
  tmp->planeWidth = width;
  tmp->planeDepth = depth;

  tmp->m_heightField.values = new float*[tmp->m_heightField.xDim];


  for (int i = 0; i < tmp->m_heightField.xDim; ++i) {
    tmp->m_heightField.values[i] = new float[tmp->m_heightField.zDim];
  }


  for (int i = 0; i < tmp->m_heightField.xDim; ++i) {
    for (int j = 0; j < tmp->m_heightField.zDim; ++j) {

      tmp->m_heightField.values[i][j] = 0.0f;
    }
  }

  unsigned int index1; 
  unsigned int index2; 
  unsigned int index3;

  tmp->indexBuffer.reserve(6*((numVerticalVerts-1)*(numHorizontalVerts-1)));
  tmp->normalBuffer.reserve(numHorizontalVerts*numVerticalVerts);
  tmp->posBuffer.reserve(numHorizontalVerts*numVerticalVerts);
  tmp->uvBuffer.reserve(numHorizontalVerts*numVerticalVerts);

  for (int i = 0 ; i < numVerticalVerts-1; i++) {
    for (int j = 0; j < numHorizontalVerts-1; j++) {
      //first triangle of quad

      index1 = (i * numHorizontalVerts)+j;
      index2 = ((i+1) * numHorizontalVerts)+j;
      index3 = ((i+1) * numHorizontalVerts)+j+1;

      tmp->indexBuffer.push_back(index1);
      tmp->indexBuffer.push_back(index2);
      tmp->indexBuffer.push_back(index3);

      //second triangle of quad

      index1 = ((i+1) * numHorizontalVerts)+j+1;
      index2 = (i * numHorizontalVerts)+j+1;
      index3 = (i * numHorizontalVerts)+j;

      tmp->indexBuffer.push_back(index1);
      tmp->indexBuffer.push_back(index2);
      tmp->indexBuffer.push_back(index3);
    }
  }


  float posX;
  float posZ;

  for (float z = 0; z < numVerticalVerts; ++z) {
    for (float x = 0; x < numHorizontalVerts; ++x) {

      posX = (x / (float)(numHorizontalVerts-1)) * width;
      posZ = (z / (float)(numVerticalVerts-1)) * depth;

      posX -= (width / 2.0f); 
      posZ -= (depth / 2.0f); 

      posX += centre.x;
      posZ += centre.z;

      tmp->posBuffer.push_back(ofVec3f(posX,centre.y,posZ));
      tmp->normalBuffer.push_back(ofVec3f(0,1,0));
      tmp->uvBuffer.push_back(ofVec2f(x,z));
    }
  }

  tmp->m_meshVbo.setIndexData(&tmp->indexBuffer[0],tmp->indexBuffer.size(),GL_STATIC_DRAW);
  tmp->m_meshVbo.setVertexData(&tmp->posBuffer[0],tmp->posBuffer.size(),GL_DYNAMIC_DRAW);
  tmp->m_meshVbo.setNormalData(&tmp->normalBuffer[0],tmp->normalBuffer.size(),GL_DYNAMIC_DRAW);
  tmp->m_meshVbo.setTexCoordData(&tmp->uvBuffer[0],tmp->uvBuffer.size(),GL_STATIC_DRAW);

  return tmp;
}

Terrain::~Terrain() {
  for (int i = 0; i < m_heightField.xDim; ++i) {
    delete[] m_heightField.values[i];
  }
  delete [] m_heightField.values;
}


void Terrain::Update() {
  RecalculateTerrain();
}

void Terrain::Draw() {
  m_meshVbo.drawElements(GL_TRIANGLES,indexBuffer.size());
}

void Terrain::RecalculateTerrain() {
  float u = 0;
  float v = 0;

  for (float z = 0; z < m_heightField.zDim; ++z) {
    for (float x = 0; x < m_heightField.xDim; ++x) {

      ofVec3f pos = posBuffer[(z * m_heightField.xDim)+x];
      u = pos.x / planeWidth;
      v = pos.z / planeDepth;
      pos.y = SampleHeightField(m_heightField,u,v);

      posBuffer[(z * m_heightField.xDim)+x] = pos;

      //tmp->normalBuffer.push_back(ofVec3f(0,1,0));
    }
  }

  m_meshVbo.updateVertexData(&posBuffer[0],posBuffer.size());
}
