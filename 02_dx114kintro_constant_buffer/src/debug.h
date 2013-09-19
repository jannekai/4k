#ifndef DEBUG_H
#define DEBUG_H

void loadShader(char* fileName, char* entryPoint, char* shaderModel, ID3DBlob** ppBlobOut);
void debugRS();
void debugTime(float time, float beat, float measure, float hit);
#endif