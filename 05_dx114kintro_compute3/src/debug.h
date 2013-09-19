#ifndef DEBUG_H
#define DEBUG_H
void loadShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut);
void logMessage(const char* fmt, ...);
#endif