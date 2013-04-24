#include "Terrain.h"
#include "ofVec2f.h"
#include "ofImage.h"

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

unsigned int CreateTexture(unsigned char* pixels, unsigned int width, unsigned int height) {
  
  unsigned int id = 0;
  glGenTextures(1,&id);
  
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return id;
}

ofVec3f CalculatePointNormal(HeightField& field,float x, float y,float stepX,float stepY,float XWspace,float YWspace) {

  ofVec3f normal;

  //stepX = 1.0f/(field.xDim -1);
  //stepY = 1.0f/(field.yDim -1);

  float h0 = SampleHeightField(field, x + 0, y - stepY );
  float h1 = SampleHeightField(field, x - stepX, y + 0 );
  float h2 = SampleHeightField(field, x + stepX, y + 0 );
  float h3 = SampleHeightField(field, x + 0, y + stepY );

  normal.x = h1 - h2;
  normal.y = 2.0f * ofVec2f(stepX*XWspace,stepY*YWspace).length();
  normal.z = h0 - h3;

  //h0 = SampleHeightField(field, x + stepX, y + stepY );
  //h1 = SampleHeightField(field, x - stepX, y + stepY );
  //h2 = SampleHeightField(field, x + stepX, y - stepY );
  //h3 = SampleHeightField(field, x - stepX, y - stepY );

  //normal.x += h1 - h2;
  //normal.y += D3DXVec2Length(&D3DXVECTOR2(stepX*XWspace,stepY*YWspace));
  //normal.z += h0 - h3;


  normal.normalize();
  return normal;
}
Terrain* Terrain::Create(float width,float depth,int numHorizontalVerts,int numVerticalVerts, ofVec3f centre) {
  Terrain* tmp = new Terrain();

  ofImage grassImg;
  grassImg.loadImage("grass.png");
  ofImage sandImg;
  sandImg.loadImage("sand.png");
  ofImage rockImg;
  rockImg.loadImage("rock.png");


  tmp->grassTex = CreateTexture(grassImg.getPixels(),grassImg.getWidth(),grassImg.getHeight());
  tmp->sandTex = CreateTexture(sandImg.getPixels(),sandImg.getWidth(),sandImg.getHeight());
  tmp->rockTex = CreateTexture(rockImg.getPixels(),rockImg.getWidth(),rockImg.getHeight());

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
      tmp->colorBuffer.push_back(ofVec3f(0,0,0));
    }
  }

  ofVec3f vertices[3] = {ofVec3f(-0.5f,0,-5),ofVec3f(0.5f,0,-5),ofVec3f(0,0.5f,-5)};
  unsigned int indices[3] = {0,1,2};

  glGenVertexArrays(1,&tmp->vao);
  glBindVertexArray(tmp->vao);

  glGenBuffers(1,&tmp->vboPos);
  glBindBuffer(GL_ARRAY_BUFFER,tmp->vboPos);
  glBufferData(GL_ARRAY_BUFFER,tmp->posBuffer.size()*sizeof(ofVec3f),&tmp->posBuffer[0],GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,false,0,0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1,&tmp->vboNormal);
  glBindBuffer(GL_ARRAY_BUFFER,tmp->vboNormal);
  glBufferData(GL_ARRAY_BUFFER,tmp->normalBuffer.size()*sizeof(ofVec3f),&tmp->normalBuffer[0],GL_DYNAMIC_DRAW);

  glVertexAttribPointer(1,3,GL_FLOAT,false,0,0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1,&tmp->vboUV);
  glBindBuffer(GL_ARRAY_BUFFER,tmp->vboUV);
  glBufferData(GL_ARRAY_BUFFER,tmp->uvBuffer.size()*sizeof(ofVec2f),&tmp->uvBuffer[0],GL_STATIC_DRAW);

  glVertexAttribPointer(2,2,GL_FLOAT,false,0,0);
  glEnableVertexAttribArray(2);

  glGenBuffers(1,&tmp->vboColor);
  glBindBuffer(GL_ARRAY_BUFFER,tmp->vboColor);
  glBufferData(GL_ARRAY_BUFFER,tmp->colorBuffer.size()*sizeof(ofVec3f),&tmp->colorBuffer[0],GL_DYNAMIC_DRAW);

  glVertexAttribPointer(3,3,GL_FLOAT,false,0,0);
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);

  glGenBuffers(1,&tmp->vboIndex);
  glBindBuffer(GL_ARRAY_BUFFER,tmp->vboIndex);
  glBufferData(GL_ARRAY_BUFFER,tmp->indexBuffer.size()*sizeof(unsigned int),&tmp->indexBuffer[0],GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  tmp->lastx = -1;
  tmp->lastz = -1;

  return tmp;
}

Terrain::~Terrain() {
  for (int i = 0; i < m_heightField.xDim; ++i) {
    delete[] m_heightField.values[i];
  }
  delete [] m_heightField.values;
}


void Terrain::Update() {
  if (isGeomDirty || isColorDirty) {
      UpdateVBO();
  }

}

void Terrain::Draw(unsigned int shaderID) {
  
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);

  unsigned int texLoc;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,grassTex);
  texLoc = glGetUniformLocation(shaderID,"grassTex");
  glUniform1i(texLoc,0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,sandTex);
  texLoc = glGetUniformLocation(shaderID,"sandTex");
  glUniform1i(texLoc,1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,rockTex);
  texLoc = glGetUniformLocation(shaderID,"rockTex");
  glUniform1i(texLoc,2);

  glDrawElements(GL_TRIANGLES,indexBuffer.size(),GL_UNSIGNED_INT,NULL);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);


  //m_meshVbo.drawElements(GL_TRIANGLES,indexBuffer.size());
  //m_meshVbo.unbind();
}

void Terrain::UpdateVBO() {
  float u = 0;
  float v = 0;

  if (isGeomDirty) {

    for (int z = 0; z < m_heightField.zDim; ++z) {
      for (int x = 0; x < m_heightField.xDim; ++x) {

        ofVec3f pos = posBuffer[(z * m_heightField.xDim)+x];

        u = (float)x / (float)m_heightField.xDim;
        v = (float)z / (float)m_heightField.zDim;

        pos.y = SampleHeightField(m_heightField,u,v);

        posBuffer[(z * m_heightField.xDim)+x] = pos;
        normalBuffer[(z * m_heightField.xDim)+x] = CalculatePointNormal(m_heightField,
          u,v,
          1.0f/(float)m_heightField.xDim,
          1.0f/(float)m_heightField.zDim,
          planeWidth,
          planeDepth);
      }                                                                 
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferSubData(GL_ARRAY_BUFFER, 0, posBuffer.size()*sizeof(ofVec3f),&posBuffer[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normalBuffer.size()*sizeof(ofVec3f),&normalBuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  if (isColorDirty) {
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colorBuffer.size()*sizeof(ofVec3f),&colorBuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }



  isGeomDirty = false;
  isColorDirty = false;
}

void Terrain::AdjustHeight( float diff,float x,float z,float radius ) {
  if (diff != 0) {
    isGeomDirty = true;
    int centerX = x*(m_heightField.xDim-1);
    int centerZ = z*(m_heightField.zDim-1);


    float radiusInFiled = m_heightField.xDim * radius;
    ofVec2f vecToPoint;
    ofVec2f centre(centerX,centerZ);

    for (int i = 0; i < m_heightField.xDim; ++i) {
      for (int j = 0; j < m_heightField.zDim; ++j) {

        vecToPoint = centre - ofVec2f(i,j);
        float d = vecToPoint.length();

        if (d < radiusInFiled) {
          float normD = (1.0f-(d/radiusInFiled));

          float finalDiff = sinf(normD)*diff;
          m_heightField.values[i][j] += finalDiff;
       }
      }
    }
  }
}

void Terrain::HighLightPosition( float x,float z,float radius ) {

  if (x != lastx && z != lastz) {
	  int centerX = x*(m_heightField.xDim-1);
	  int centerZ = z*(m_heightField.zDim-1);
	
	
	  float radiusInFiled = m_heightField.xDim * radius;
	  ofVec2f vecToPoint;
	  ofVec2f centre(centerX,centerZ);
	
	 // colorBuffer[(centerZ*m_heightField.xDim)+centerX] = ofVec3f(1,0,0);
	
	  for (int i = 0; i < m_heightField.xDim; ++i) {
	    for (int j = 0; j < m_heightField.zDim; ++j) {
	
	      vecToPoint = centre - ofVec2f(i,j);
	      float d = vecToPoint.length();
	      float normD = (1.0f-(d/radiusInFiled));
	
	      colorBuffer[(j*m_heightField.xDim)+i] = d < radiusInFiled ? ofVec3f(normD,0,0) : ofVec3f(0,0,0);
	    }
	  }

    lastx = x;
    lastz = z;

    isColorDirty = true;
  }
  


}

void Terrain::Reset()
{
  for (int i = 0; i < m_heightField.xDim; ++i) {
    for (int j = 0; j < m_heightField.zDim; ++j) {

      m_heightField.values[i][j] = 0.0f;
    }
  }
  
  isGeomDirty = true;

  UpdateVBO();
}
