#define X_RES 1280
#define Y_RES 720

struct Data {
	float4 color;
};

// For Gradient we need g_data as output
StructuredBuffer<Data> g_dataIn : register(t0);
RWStructuredBuffer<Data> g_dataOut : register(u0);
RWTexture2D<float4> g_out : register(u1);

[numthreads(1, 1, 1)]
void Gradient(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {	
	int i = dispatchThreadId.x + dispatchThreadId.y * X_RES;
	float x = float(dispatchThreadId.x) / float(X_RES);
	float y = float(dispatchThreadId.y) / float(Y_RES);
	g_dataOut[i].color = float4(x, y, 0, 1);
	// g_out[dispatchThreadId.xy] = float4(x, y, 0, 1);
}

[numthreads(1, 1, 1)]
void Checkers(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {	
	uint i = dispatchThreadId.x + dispatchThreadId.y * X_RES;
	float y = float(dispatchThreadId.x) / float(X_RES);
	float x = float(dispatchThreadId.y) / float(Y_RES);
	// g_out[dispatchThreadId.xy] = float4(x, y, 0, 1);
	g_out[dispatchThreadId.xy] = float4(x, y, 0, 1) * g_dataIn[i].color;
}
