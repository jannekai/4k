import sys

# Read input shader
src = open('data/shader.hlsl', 'r').readlines()

# Minify shader
out = []
for line in src:
	line = line.strip()
	if len(line) == 0:
		continue;
	if line.startswith('//'):
		continue;		
	if line.startswith("#define"):
		line += "\\n"
	out.append(line)
	
# Write shader to src/shader.h
dst = open('src/shader.h', 'w');
dst.write("#ifndef SHADER_H\n");
dst.write("#define SHADER_H\n");
dst.write("static const char shader_hlsl[] = {\n");
for line in out:
	dst.write("\"");
	dst.write(line);
	dst.write("\"");	
	dst.write("\n");
dst.write("};\n");
dst.write("#endif\n");
