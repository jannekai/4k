#ifndef SHADER_H
#define SHADER_H
static const char shader_hlsl[] = {
"#define THREAD_GROUPS_X	1280\n"
"#define THREAD_GROUPS_Y	1\n"
"#define THREAD_GROUPS_Z 1\n"
"#define THREADS_X	720\n"
"#define THREADS_Y	1\n"
"#define THREADS_Z	1\n"
"struct BufferStruct {"
"float4 position : SV_Position;"
"float4 color : COLOR;"
"};"
"StructuredBuffer<BufferStruct> g_in;"
"RWStructuredBuffer<BufferStruct> g_out;"
"[numthreads(THREADS_X, THREADS_Y, THREADS_Z)]"
"void CS(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {"
"float x = float(groupId.x) / float(THREAD_GROUPS_X);"
"float y = float(groupThreadId.x) / float(THREADS_X);"
"g_out[dispatchThreadId.x].position = float4(((x*2) - 1), ((y*2)-1), 0, 1);"
"g_out[dispatchThreadId.x].color = float4(x, y, 0, 1);"
"}"
"BufferStruct VS(uint id : SV_VertexID)"
"{"
"return g_in[id];"
"}"
"float4 PS(BufferStruct i) : SV_Target"
"{"
"return i.color;"
"}"
};
#endif
