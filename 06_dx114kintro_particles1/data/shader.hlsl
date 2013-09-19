#define X_RES		1280
#define Y_RES		720
#define GROUPS_X	2
#define GROUPS_Y	1
#define GROUPS_Z	1
#define THREAD_X	512
#define THREAD_Y	1
#define THREAD_Z	1
#define BUFFER_SIZE GROUPS_X * THREAD_X
#define M_PI 3.14159265358979323846

cbuffer StaticConstants : register(b0) {
	float4	Quad[6];
	float4  Random[64];	
};

cbuffer FrameConstants : register(b1) {
	matrix	World;
	matrix	View;
	matrix	Projection;
	uint	Sample;
	float	Time;
};

struct Data {
	float4 position;
	float4 color;
	float4 time;
};

StructuredBuffer<Data> g_dataIn;
RWStructuredBuffer<Data> g_dataOut;

struct VSO {
	float4 position : SV_POSITION;
	float4 diffuse	: COLOR0;
	float4 specular	: COLOR1;
};

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void CS(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {		
	Data o;
	uint idx = (dispatchThreadId.x % 64);	
	
	o.position = Random[idx];	
	o.color = float4(float(idx), float(Sample), float(Time+0.1), 1.123);
	o.time = World[0];
	g_dataOut[dispatchThreadId.x] = o;
}

VSO VS(uint id : SV_VertexId) {	
	uint idx = id / 6;
	uint vtx = id - idx*6;

	VSO o;
	o.position	= g_dataIn[idx].position;
	/*
	 + Quad[vtx];
	o.position	= mul(o.position, World);
	o.position	= mul(o.position, View);
	o.position	= mul(o.position, Projection);
	*/
	o.diffuse	= g_dataIn[idx].color;
	o.specular	= g_dataIn[idx].time;
	return o;
}

float4 PS(VSO i) : SV_Target {
	return i.diffuse * i.specular;
}