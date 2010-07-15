!!NVgp4.0
PRIMITIVE_IN TRIANGLES;
PRIMITIVE_OUT TRIANGLE_STRIP;
VERTICES_OUT 3;
# cgc version 2.2.0017, build date Jan 29 2010
# command line args: -profile gp4gp
# source file: drawNormals.cg
#vendor NVIDIA Corporation
#version 2.2.0.17
#profile gp4gp
#program GPcalcNormals
#semantic GPcalcNormals.position : POSITION
#semantic GPcalcNormals.color : COLOR
#semantic GPcalcNormals.normal : TEXCOORD0
#semantic GPcalcNormals.objectPosition : TEXCOORD1
#semantic red
#semantic green
#semantic blue
#semantic magenta
#semantic yellow
#semantic cyan
#semantic white
#var float4 position.__data[0] : $vin.VERTEX[0].POSITION : HPOS[0][32] : 0 : 1
#var float4 position.__data[1] : $vin.VERTEX[1].POSITION : HPOS[1][32] : 0 : 1
#var float4 position.__data[2] : $vin.VERTEX[2].POSITION : HPOS[2][32] : 0 : 1
#var float4 color.__data[0] : $vin.VERTEX[0].COLOR : COL0[0][33] : 1 : 1
#var float4 color.__data[1] : $vin.VERTEX[1].COLOR : COL0[1][33] : 1 : 1
#var float4 color.__data[2] : $vin.VERTEX[2].COLOR : COL0[2][33] : 1 : 1
#var float3 normal.__data[0] : $vin.VERTEX[0].TEXCOORD0 :  : 2 : 0
#var float3 normal.__data[1] : $vin.VERTEX[1].TEXCOORD0 :  : 2 : 0
#var float3 normal.__data[2] : $vin.VERTEX[2].TEXCOORD0 :  : 2 : 0
#var float4 objectPosition.__data[0] : $vin.VERTEX[0].TEXCOORD1 : TEX1[0][38] : 3 : 1
#var float4 objectPosition.__data[1] : $vin.VERTEX[1].TEXCOORD1 : TEX1[1][38] : 3 : 1
#var float4 objectPosition.__data[2] : $vin.VERTEX[2].TEXCOORD1 : TEX1[2][38] : 3 : 1
#var float3 red :  :  : -1 : 0
#var float3 green :  :  : -1 : 0
#var float3 blue :  :  : -1 : 0
#var float3 magenta :  :  : -1 : 0
#var float3 yellow :  :  : -1 : 0
#var float3 cyan :  :  : -1 : 0
#var float3 white :  :  : -1 : 0
#default red = 1 0 0
#default green = 0 1 0
#default blue = 0 0 1
#default magenta = 1 0 1
#default yellow = 1 1 0
#default cyan = 0 1 1
#default white = 1 1 1
ATTRIB vertex_texcoord[] = { vertex.texcoord[0..1] };
ATTRIB vertex_color = vertex.color;
ATTRIB vertex_position = vertex.position;
OUTPUT result_texcoord[] = { result.texcoord[0..0] };
TEMP R0, R1, R2;
MOV.F R0.xyz, vertex[0].texcoord[1];
ADD.F R1.xyz, -R0, vertex[2].texcoord[1];
ADD.F R0.xyz, vertex[1].texcoord[1], -R0;
MUL.F R2.xyz, R0.zxyw, R1.yzxw;
MAD.F R1.xyz, R0.yzxw, R1.zxyw, -R2;
MOV.F result.texcoord[0].xyz, R1;
MOV.F result.position, vertex[0].position;
MOV.F R0.xyz, R1;
MOV.F result.color, vertex[0].color;
EMIT;
MOV.F result.position, vertex[1].position;
MOV.F result.color, vertex[1].color;
MOV.F result.texcoord[0].xyz, R0;
EMIT;
MOV.F result.position, vertex[2].position;
MOV.F result.color, vertex[2].color;
MOV.F result.texcoord[0].xyz, R0;
EMIT;
END
# 18 instructions, 3 R-regs
