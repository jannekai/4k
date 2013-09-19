/* File generated with Shader Minifier 1.0.2
 * http://www.ctrl-alt-test.fr
 */
#ifndef SHADER_H_
# define SHADER_H_
# define F_PS "t"
# define F_VS "f"

const char *shader_hlsl = ""
 "float4 f(float4 f:POSITION):SV_POSITION"
 "{"
   "return f;"
 "}"
 "float4 t(float4 f:SV_POSITION):SV_Target"
 "{"
   "return float4(1.,1.,0.,1.);"
 "}";

#endif // SHADER_H_
