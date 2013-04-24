#ifndef terrainShader_h__
#define terrainShader_h__

const char* terrainVShader = 
  
  "uniform mat4 mToV;"
  "uniform mat4 vToP;"
  "attribute vec3 pos;"
  "attribute vec3 normal;"
  "attribute vec2 uv;"
  "attribute vec3 color;"
  "varying vec3 outNormal;"
  "varying vec2 outUV;"
  "varying vec3 outColor;"
  "varying vec3 outPos;"
  "void main() { "
  "outNormal = normal;"
  "outUV = uv;"
  "outPos = pos;"
  "outColor = color;"
  "gl_Position = vToP * mToV *vec4(pos,1);"
  "}";

const char* terrainPShader = 
  //"varying vec3 outNormal;"
  //"varying vec2 outUV;"
  //"varying vec3 outColor;"
  //"varying vec3 outPos;"
  //"uniform sampler2D grassTex;"
  //"uniform sampler2D sandTex;"
  //"uniform sampler2D rockTex;"

  //"vec3 SampleTriPlanar(sampler2D tex,vec3 normal,vec3 PositionWS) {"

  //  "float mXY = abs(normal.z);"
  //  "float mXZ = abs(normal.y);"
  //  "float mYZ = abs(normal.x);"

  //  "float scale = 1.0 / 4;"

  //  "float total = mXY + mXZ + mYZ;"
  //  "mXY /= total;"
  //  "mXZ /= total;"
  //  "mYZ /= total;"

  //  "vec3 cXY = texture2D(tex, PositionWS.xy * scale).xyz;"
  //  "vec3 cXZ = texture2D(tex, PositionWS.xz * scale).xyz;"
  //  "vec3 cYZ = texture2D(tex, PositionWS.yz * scale).xyz;"

  //  "return cXY*mXY + cXZ*mXZ + cYZ*mYZ;" 
  //"}"

  "void main(){"
    //"float slope = 1 - abs(dot(outNormal,vec3(0,1,0)));"
    //"vec3 color;"
    //"vec3 grass = SampleTriPlanar(grassTex,outNormal,outPos);"
    //"vec3 sand = SampleTriPlanar(sandTex,outNormal,outPos);"
    //"vec3 rock = SampleTriPlanar(rockTex,outNormal,outPos);"
    //"float grassHi = 0.3;"
    //"float sandLo = 0.5;"
    //"float sandHi = 0.6;"
    //"float rockLo = 0.7;"

    //"if (slope <= grassHi) {"
    //  "color = grass;"
    //"}"
    //"if (slope > grassHi && slope <= sandLo) {"
    // "float blend = smoothstep(grassHi,sandLo,slope);"
    //  "color += (1-blend) * grass;"
    //  "color += (blend) * sand;"
    //"}"
    //"if (slope > sandLo && slope <= sandHi) {"
    //  "color = sand;"
    //"}"
    //"if (slope > sandHi && slope <= rockLo) {"
    //  "float blend = smoothstep(sandHi,rockLo,slope);"
    //  "color += (1-blend) * sand;"
    //  "color += (blend) * rock;"
    //"}"
    //"if(slope >= rockLo) color = rock;"
    //"color = vec3(1,1,1);"
    "gl_FragColor = vec4(1,0,0,1);"
  "}";

#endif // terrainShader_h__
