characters = {
	'A' : [
		'0110',
		'1001',
		'1111',
		'1001',
		'1001'
	],
	'B' : [
		'1110',
		'1001',
		'1110',
		'1001',
		'1110'
	],
	'C' : [
		'1111',
		'1001',
		'1000',
		'1001',
		'1111'
	],
	'D' : [
		'1110',
		'1001',
		'1001',
		'1001',
		'1110'
	],
	'E' : [
		'1111',
		'1000',
		'1110',
		'1000',
		'1111'
	],
	'F' : [
		'1111',
		'1000',
		'1110',
		'1000',
		'1000'
	],
	'G' : [
		'1111',
		'1000',
		'1010',
		'1001',
		'1111'
	],
	'H' : [
		'1001',
		'1001',
		'1111',
		'1001',
		'1001'
	],
	'I' : [
		'0010',
		'0010',
		'0010',
		'0010',
		'0010'
	],
	'J' : [
		'0001',
		'0001',
		'0001',
		'1001',
		'1111'
	],
	'K' : [
		'1001',
		'1010',
		'1100',
		'1010',
		'1001'
	],
	'L' : [
		'1000',
		'1000',
		'1000',
		'1000',
		'1111'
	],
	'M' : [
		'1001',
		'1111',
		'1111',
		'1001',
		'1001'
	],
	'N' : [
		'1001',
		'1101',
		'1011',
		'1001',
		'1001'
	],
	'O' : [
		'0110',
		'1001',
		'1001',
		'1001',
		'0110'
	],
	'P' : [
		'1110',
		'1001',
		'1110',
		'1000',
		'1000'
	],
	'Q' : [
		'0110',
		'1001',
		'1001',
		'0101',
		'1010'
	],
	'R' : [
		'1110',
		'1001',
		'1110',
		'1010',
		'1001'
	],
	'S' : [
		'1111',
		'1000',
		'1111',
		'0001',
		'1111'
	],
	'T' : [
		'0111',
		'0010',
		'0010',
		'0010',
		'0010'
	],
	'U' : [
		'1001',
		'1001',
		'1001',
		'1001',
		'0110'
	],
	'V' : [
		'1001',
		'1001',
		'1001',
		'0110',
		'0110'
	],
	'W' : [
		'1001',
		'1001',
		'1001',
		'1111',
		'0110'
	],
	'X' : [
		'1001',
		'1001',
		'0110',
		'1001',
		'1001'
	],
	'Y' : [
		'1001',
		'0110',
		'0010',
		'0010',
		'0010'
	],
	'Z' : [
		'1111',
		'0001',
		'0010',
		'1100',
		'1111'
	]
}

texts = [
	"CUBICLE", 
	"ENDYMIO", 
	"SKYFOX", 
	"JUHOAP",
	"KEWLERS",
	"BYTERAPERS",
	"STREAM",
	"INTRO",
	"MFX",
	"MOODS",
	"CELL"	
	]
indexes = dict()


i = 0;
defines = ""
symbols = "static unsigned char g_symbols[] = { \n";
for k, s in characters.iteritems():	
	defines += "#define SYM_" + str(k) + " " + str(i) + "\n"
	indexes[k] = i;
	symbols += "    "
	v = ((int(s[0][0]) << 3)  + (int(s[0][1]) << 2) + (int(s[0][2]) << 1) + (int(s[0][3])))
	symbols += hex(v)  + ","
	v = ((int(s[1][0]) << 3)  + (int(s[1][1]) << 2) + (int(s[1][2]) << 1) + (int(s[1][3])))
	symbols += hex(v)  + ","
	v = ((int(s[2][0]) << 3)  + (int(s[2][1]) << 2) + (int(s[2][2]) << 1) + (int(s[2][3])))
	symbols += hex(v)  + ","
	v = ((int(s[3][0]) << 3)  + (int(s[3][1]) << 2) + (int(s[3][2]) << 1) + (int(s[3][3])))
	symbols += hex(v)  + ","
	v = ((int(s[4][0]) << 3)  + (int(s[4][1]) << 2) + (int(s[4][2]) << 1) + (int(s[4][3])))
	symbols += hex(v)  + ","
	symbols += "\n"
	i += 1
	
symbols = symbols[:-2]
symbols += "\n};\n"
defines += "#define SYM_LENGTH " + str(i) + "\n"

i = 0;
strings = "static unsigned char g_strings[] = { \n";
for t in texts:
	defines += "#define STR_" + t + "_INDEX " + str(i) + "\n"
	defines += "#define STR_" + t + "_LENGTH " + str(len(t)) + "\n"
	strings += "    "
	for l in t:
		strings += str(indexes[l]) + ","
	strings += "\n"	
	i += len(t)
strings = strings[:-2] + "\n};\n"
defines += "#define STR_LENGTH " + str(i) + "\n"

f = open('../src/symbols.h', "w+")
f.write("#ifndef SYMBOLS_H\n")
f.write("#define SYMBOLS_H\n")
f.write("\n");
f.write(defines)
f.write("\n");
f.write(symbols)
f.write("\n");
f.write(strings)
f.write("\n");
f.write("#endif\n")
