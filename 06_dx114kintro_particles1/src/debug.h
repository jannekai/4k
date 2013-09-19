#ifndef DEBUG_H
#define DEBUG_H

void LoadPixelShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11PixelShader** ppShader, bool reload);
void LoadGeometryShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11GeometryShader** ppShader, bool reload);
void LoadVertexShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11VertexShader** ppShader, bool reload);
void LoadComputeShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11ComputeShader** ppShader, bool reload);
void LogMessage(const char* fmt, ...);
bool IsShaderModified(const char* fileName);

#define LOAD_PS(shader, entrypoint, pBlob, pShader) { LoadPixelShader(shader, entrypoint, "ps_5_0", &pBlob, &pShader, false); }
#define LOAD_GS(shader, entrypoint, pBlob, pShader) { LoadGeometryShader(shader, entrypoint, "gs_5_0", &pBlob, &pShader, false); }
#define LOAD_VS(shader, entrypoint, pBlob, pShader) { LoadVertexShader(shader, entrypoint, "vs_5_0", &pBlob, &pShader, false); }
#define LOAD_CS(shader, entrypoint, pBlob, pShader) { LoadComputeShader(shader, entrypoint, "cs_5_0", &pBlob, &pShader, false); }

#define RELOAD_PS(shader, entrypoint, pBlob, pShader) { LoadPixelShader(shader, entrypoint, "ps_5_0", &pBlob, &pShader, true); }
#define RELOAD_GS(shader, entrypoint, pBlob, pShader) { LoadGeometryShader(shader, entrypoint, "gs_5_0", &pBlob, &pShader, true); }
#define RELOAD_VS(shader, entrypoint, pBlob, pShader) { LoadVertexShader(shader, entrypoint, "vs_5_0", &pBlob, &pShader, true); }
#define RELOAD_CS(shader, entrypoint, pBlob, pShader) { LoadComputeShader(shader, entrypoint, "cs_5_0", &pBlob, &pShader, true); }

#define LOG(e,...) { LogMessage(e,__VA_ARGS__); }

#endif