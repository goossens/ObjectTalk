static const uint8_t OtNormaledVSI_glsl[1108] =
{
	0x56, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x82, 0x69, 0x6f, 0x4e, 0x03, 0x00, 0x0a, 0x75, // VSH......ioN...u
	0x5f, 0x76, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x04, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // _viewProj.......
	0x00, 0x00, 0x00, 0x0e, 0x75, 0x5f, 0x75, 0x76, 0x5f, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x6f, // ....u_uv_transfo
	0x72, 0x6d, 0x03, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x75, 0x5f, 0x73, // rm...........u_s
	0x68, 0x61, 0x64, 0x6f, 0x77, 0x02, 0x05, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // hadow...........
	0x04, 0x00, 0x00, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, // ...attribute vec
	0x33, 0x20, 0x61, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, // 3 a_normal;.attr
	0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, // ibute vec3 a_pos
	0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, // ition;.attribute
	0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x61, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, //  vec2 a_texcoord
	0x30, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, // 0;.attribute vec
	0x34, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x30, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, // 4 i_data0;.attri
	0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, // bute vec4 i_data
	0x31, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, // 1;.attribute vec
	0x34, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x32, 0x3b, 0x0a, 0x61, 0x74, 0x74, 0x72, 0x69, // 4 i_data2;.attri
	0x62, 0x75, 0x74, 0x65, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, // bute vec4 i_data
	0x33, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, // 3;.varying vec3 
	0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, // v_normal;.varyin
	0x67, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x76, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, // g vec4 v_positio
	0x6e, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, // n;.varying vec4 
	0x76, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, // v_shadow;.varyin
	0x67, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, // g vec2 v_texcoor
	0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6d, 0x61, 0x74, 0x34, // d0;.uniform mat4
	0x20, 0x75, 0x5f, 0x76, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x3b, 0x0a, 0x75, 0x6e, 0x69, //  u_viewProj;.uni
	0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6d, 0x61, 0x74, 0x33, 0x20, 0x75, 0x5f, 0x75, 0x76, 0x5f, 0x74, // form mat3 u_uv_t
	0x72, 0x61, 0x6e, 0x73, 0x66, 0x6f, 0x72, 0x6d, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, // ransform;.unifor
	0x6d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x75, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, 0x5b, // m vec4 u_shadow[
	0x35, 0x5d, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, // 5];.void main ()
	0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x6d, 0x61, 0x74, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // .{.  mat4 tmpvar
	0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x5b, 0x30, // _1;.  tmpvar_1[0
	0x5d, 0x20, 0x3d, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, // ] = i_data0;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x5b, 0x31, 0x5d, 0x20, 0x3d, 0x20, 0x69, 0x5f, 0x64, // mpvar_1[1] = i_d
	0x61, 0x74, 0x61, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, // ata1;.  tmpvar_1
	0x5b, 0x32, 0x5d, 0x20, 0x3d, 0x20, 0x69, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x32, 0x3b, 0x0a, 0x20, // [2] = i_data2;. 
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x5b, 0x33, 0x5d, 0x20, 0x3d, 0x20, 0x69, //  tmpvar_1[3] = i
	0x5f, 0x64, 0x61, 0x74, 0x61, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, // _data3;.  vec4 t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // mpvar_2;.  tmpva
	0x72, 0x5f, 0x32, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, // r_2.w = 1.0;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, // mpvar_2.xyz = a_
	0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x70, 0x6f, // position;.  v_po
	0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // sition = (tmpvar
	0x5f, 0x31, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x29, 0x3b, 0x0a, // _1 * tmpvar_2);.
	0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, //   vec4 tmpvar_3;
	0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x77, 0x20, 0x3d, 0x20, // .  tmpvar_3.w = 
	0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, // 0.0;.  tmpvar_3.
	0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, // xyz = a_normal;.
	0x20, 0x20, 0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, //   v_normal = (tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // pvar_1 * tmpvar_
	0x33, 0x29, 0x2e, 0x78, 0x79, 0x7a, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x74, // 3).xyz;.  vec3 t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // mpvar_4;.  tmpva
	0x72, 0x5f, 0x34, 0x2e, 0x7a, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, // r_4.z = 1.0;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x78, 0x79, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x74, // mpvar_4.xy = a_t
	0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x74, 0x65, // excoord0;.  v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x75, 0x76, 0x5f, // xcoord0 = (u_uv_
	0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, // transform * tmpv
	0x61, 0x72, 0x5f, 0x34, 0x29, 0x2e, 0x78, 0x79, 0x3b, 0x0a, 0x20, 0x20, 0x6d, 0x61, 0x74, 0x34, // ar_4).xy;.  mat4
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, //  tmpvar_5;.  tmp
	0x76, 0x61, 0x72, 0x5f, 0x35, 0x5b, 0x30, 0x5d, 0x20, 0x3d, 0x20, 0x75, 0x5f, 0x73, 0x68, 0x61, // var_5[0] = u_sha
	0x64, 0x6f, 0x77, 0x5b, 0x31, 0x5d, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // dow[1];.  tmpvar
	0x5f, 0x35, 0x5b, 0x31, 0x5d, 0x20, 0x3d, 0x20, 0x75, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, // _5[1] = u_shadow
	0x5b, 0x32, 0x5d, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x5b, // [2];.  tmpvar_5[
	0x32, 0x5d, 0x20, 0x3d, 0x20, 0x75, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, 0x5b, 0x33, 0x5d, // 2] = u_shadow[3]
	0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x5b, 0x33, 0x5d, 0x20, // ;.  tmpvar_5[3] 
	0x3d, 0x20, 0x75, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, 0x5b, 0x34, 0x5d, 0x3b, 0x0a, 0x20, // = u_shadow[4];. 
	0x20, 0x76, 0x5f, 0x73, 0x68, 0x61, 0x64, 0x6f, 0x77, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, //  v_shadow = (tmp
	0x76, 0x61, 0x72, 0x5f, 0x35, 0x20, 0x2a, 0x20, 0x76, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, // var_5 * v_positi
	0x6f, 0x6e, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, // on);.  gl_Positi
	0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x76, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, // on = (u_viewProj
	0x20, 0x2a, 0x20, 0x76, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x29, 0x3b, 0x0a, //  * v_position);.
	0x7d, 0x0a, 0x0a, 0x00,                                                                         // }...
};