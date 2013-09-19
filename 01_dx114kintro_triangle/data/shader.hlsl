struct BufferStruct {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// StructuredBuffer<BufferStruct> g_in;
// RWStructuredBuffer<BufferStruct> g_out;

//--------------------------------------------------------------------------------------
// Compute shader
//--------------------------------------------------------------------------------------
/*
[numthreads(1, 1, 1)]
void CS(uint3 gtID : SV_GroupThreadID, uint giID : SV_GroupIndex)
{
	// g_out[giID].position = float3(gtID.x, gtID.y, 10);
	g_out[giID].position = float4(0, 0, 10, 0);
	g_out[giID].color = float4(gtID.x, gtID.y, 0, 0);
}
*/

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
BufferStruct VS(uint id : SV_VertexID)
{
	BufferStruct o;
	o.position = float4(0, 0, 0, 1);
	o.color = float4(0, 1, 0, 1);
	return o;
	// return g_in[id];
}

//--------------------------------------------------------------------------------------
// Geometry shader
//--------------------------------------------------------------------------------------
[maxvertexcount(3)]
void GS(point BufferStruct input[1], inout TriangleStream<BufferStruct> ss)
{
	BufferStruct o;

	float4 g_offsets[] = {
		float4( 0.0,  0.5, 0, 0),
		float4( 0.5, -0.5, 0, 0),
		float4(-0.5, -0.5, 0, 0),
	};	

	for (int i = 0; i < 3; i++) {
		o.color = input[0].color;
		o.position = input[0].position + g_offsets[i];
		ss.Append(o);
	}
	ss.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(BufferStruct i) : SV_Target
{
	return i.color;
}
