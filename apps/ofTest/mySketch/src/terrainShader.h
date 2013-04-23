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
  "varying float height;"
  "void main() { "
  "height = pos.y / 10.0f;"
  "outNormal = normal;"
  "outUV = uv;"
  "outColor = color;"
  "gl_Position = vToP * mToV *vec4(pos,1);"
  "}";

const char* terrainPShader = 
  "varying vec3 outNormal;"
  "varying vec2 outUV;"
  "varying vec3 outColor;"
  "varying float height;"
  "void main(){"
    "float slope = 1 - abs(dot(outNormal,vec3(0,1,0)));"
    "vec3 color = vec3(0,0,0);"
    //"color.xyz = 1.0;"
    //"if (slope <= 0.2) {"
    //  "color = vec3(0,1,0);"
    //"}"
    //"if (slope > 0.2 && slope <= 0.25) {"
    // "float blend = smoothstep(0.2,0.25,slope);"
    //  "color += (1-blend) * vec3(0,1,0);"
    //  "color += (blend) * vec3(1,1,0);"
    //"}"
    //"if (slope > 0.25 && slope <= 0.3) {"
    //  "color = vec3(1,1,0);"
    //"}"
    //"if (slope > 0.3 && slope <= 0.35) {"
    //  "float blend = smoothstep(0.3,0.35,slope);"
    //  "color += (1-blend) * vec3(1,1,0);"
    //  "color += (blend) * vec3(0.3,0.3,0.3);"
    //"}"
    //"if(slope <= 0.35) color = vec3(0.3,0.3,0.3);"
    //"color.xyz = slope;"
    "color += outColor;"
    "gl_FragColor = vec4(outNormal,1);"
  "}";

#endif // terrainShader_h__
