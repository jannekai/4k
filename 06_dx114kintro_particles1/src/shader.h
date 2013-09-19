#ifndef SHADER_H
#define SHADER_H
static const char SHADER_HLSL[] = {
"#define X_RES		1280\n"
"#define Y_RES		720\n"
"#define GROUPS_X	4096\n"
"#define GROUPS_Y	1\n"
"#define GROUPS_Z	1\n"
"#define THREAD_X	512\n"
"#define THREAD_Y	1\n"
"#define THREAD_Z	1\n"
"#define BUFFER_SIZE 2097152\n"
"#define M_PI 3.14159265358979323846\n"
"cbuffer StaticConstants : register(b0) {"
"float4	Quad[6];"
"float   Random[256];"
"};"
"cbuffer FrameConstants : register(b1) {"
"float4 Time;"
"matrix World;"
"matrix View;"
"matrix Projection;"
"};"
"struct Data {"
"float4 position;"
"float4 color;"
"};"
"StructuredBuffer<Data> g_dataIn;"
"RWStructuredBuffer<Data> g_dataOut;"
"struct VSO {"
"float4 position : SV_POSITION;"
"float4 color : COLOR;"
"};"
"float rand(float2 co){"
"return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);"
"}"
"[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]"
"void CS(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {"
"float x = rand(float2(dispatchThreadId.x, Time.x));"
"float y = rand(float2(Time.y, dispatchThreadId.x));"
"g_dataOut[dispatchThreadId.x].position = float4(x*20, y*20, (x+y)*20, 0);"
"g_dataOut[dispatchThreadId.x].color = float4(x + 0.5, y + 0.5, x+y-0.5, 1);"
"}"
"VSO VS(uint id : SV_VertexId) {"
"uint idx = id / 6;"
"uint vtx = id - idx*6;"
"VSO o;"
"o.position	= g_dataIn[idx].position + Quad[vtx];"
"o.position	= mul(o.position, World);"
"o.position	= mul(o.position, View);"
"o.position	= mul(o.position, Projection);"
"o.color		= g_dataIn[idx].color;"
"return o;"
"}"
"float4 PS(VSO i) : SV_Target {"
"return i.color;"
"}"
};
#endif
