#ifndef SHADER_H
#define SHADER_H
static const char shader_hlsl[] = {
"#define X_RES 1280\n"
"#define Y_RES 720\n"
"RWTexture2D<float4> g_out;"
"[numthreads(1, 1, 1)]"
"void CS(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {"
"float x = float(dispatchThreadId.x) / float(X_RES);"
"float y = float(dispatchThreadId.y) / float(Y_RES);"
"g_out[dispatchThreadId.xy] = float4(x, y, 0, 1);"
"}"
};
#endif
