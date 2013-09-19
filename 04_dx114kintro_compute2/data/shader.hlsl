#define X_RES 1280
#define Y_RES 720

RWTexture2D<float4> g_out;

//--------------------------------------------------------------------------------------
// Compute shader
//--------------------------------------------------------------------------------------
[numthreads(1, 1, 1)]
void CS(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint groupIndex: SV_GroupIndex) {	
	float x = float(dispatchThreadId.x) / float(X_RES);
	float y = float(dispatchThreadId.y) / float(Y_RES);
	g_out[dispatchThreadId.xy] = float4(x, y, 0, 1);
}
