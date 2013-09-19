import sys

replacements = dict()

# Read defines
#defines = open('src/defines.h', 'r').readlines()
#for line in defines:
#    line = line.strip()
#    if not line.startswith('#define'):
#        continue
#    parts = line.split()
#    replacements[parts[1]] = parts[2]

# Read input shader
src = open('data/shader.hlsl', 'r').readlines()

# Minify shader
out = []
for line in src:
    line = line.strip()
    if len(line) == 0:
        continue
    if line.startswith('//'):
        continue		
    if line.startswith("#define"):
        line += "\\n"
    for k, v in replacements.iteritems():
        line = line.replace(k,v)        
    out.append(line)

# Write shader to src/shader.h
dst = open('src/shader.h', 'w')
dst.write("#ifndef SHADER_H\n")
dst.write("#define SHADER_H\n")
dst.write("static const char SHADER_HLSL[] = {\n")
for line in out:
    dst.write("\"")
    dst.write(line)
    dst.write("\"")	
    dst.write("\n")
dst.write("};\n")
dst.write("#endif\n")
