//--------------------------------------------------------------------------------------
// Constant buffer
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer {
	float g_time;
	float g_beat;
	float g_measure;
	float g_hit;
};

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct BufferStruct {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
BufferStruct VS(uint id : SV_VertexID)
{
	BufferStruct o;
	o.position = float4(0, 0, 0, 1);
	o.color = float4(0.5, g_hit - floor(g_hit), 0, 1);
	return o;
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
