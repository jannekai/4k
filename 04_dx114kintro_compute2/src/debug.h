#ifndef DEBUG_H
#define DEBUG_H
void loadShader(char* fileName, char* entryPoint, char* shaderModel, ID3DBlob** ppBlobOut);
void logMessage(const char* fmt, ...);
#endif