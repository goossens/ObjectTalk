static const uint8_t OtGeometryPbrVS_essl[914] =
{
	0x56, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0xd3, 0xbe, 0x68, 0xc6, 0x02, 0x00, 0x07, 0x75, // VSH.......h....u
	0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x04, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, // _model. ........
	0x0f, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, // .u_modelViewProj
	0x04, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x03, 0x00, 0x00, 0x70, 0x72, // ..........S...pr
	0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x69, 0x6e, // ecision highp in
	0x74, 0x3b, 0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, // t;.precision hig
	0x68, 0x70, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, // hp float;.in hig
	0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x62, 0x69, 0x74, 0x61, 0x6e, 0x67, // hp vec3 a_bitang
	0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, // ent;.in highp ve
	0x63, 0x33, 0x20, 0x61, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, 0x69, 0x6e, 0x20, // c3 a_normal;.in 
	0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, // highp vec3 a_pos
	0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, // ition;.in highp 
	0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, 0x74, 0x3b, 0x0a, // vec3 a_tangent;.
	0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x61, 0x5f, // in highp vec2 a_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x68, // texcoord0;.out h
	0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, 0x5f, 0x62, 0x69, 0x74, 0x61, // ighp vec3 v_bita
	0x6e, 0x67, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // ngent;.out highp
	0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, //  vec3 v_normal;.
	0x6f, 0x75, 0x74, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, // out highp vec3 v
	0x5f, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x68, 0x69, // _tangent;.out hi
	0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, // ghp vec2 v_texco
	0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6d, 0x61, // ord0;.uniform ma
	0x74, 0x34, 0x20, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x5b, 0x33, 0x32, 0x5d, 0x3b, 0x0a, // t4 u_model[32];.
	0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x6d, 0x61, // uniform highp ma
	0x74, 0x34, 0x20, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, // t4 u_modelViewPr
	0x6f, 0x6a, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, // oj;.void main ()
	0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, // .{.  v_texcoord0
	0x20, 0x3d, 0x20, 0x61, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, //  = a_texcoord0;.
	0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, //   highp vec4 tmp
	0x76, 0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // var_1;.  tmpvar_
	0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, // 1.w = 0.0;.  tmp
	0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x6e, 0x6f, // var_1.xyz = a_no
	0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, // rmal;.  v_normal
	0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x5b, 0x30, 0x5d, 0x20, 0x2a, //  = (u_model[0] *
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x29, 0x2e, 0x78, 0x79, 0x7a, 0x3b, 0x0a, //  tmpvar_1).xyz;.
	0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, //   highp vec4 tmp
	0x76, 0x61, 0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // var_2;.  tmpvar_
	0x32, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, // 2.w = 0.0;.  tmp
	0x76, 0x61, 0x72, 0x5f, 0x32, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x74, 0x61, // var_2.xyz = a_ta
	0x6e, 0x67, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x74, 0x61, 0x6e, 0x67, 0x65, // ngent;.  v_tange
	0x6e, 0x74, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x5b, 0x30, 0x5d, // nt = (u_model[0]
	0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x29, 0x2e, 0x78, 0x79, 0x7a, //  * tmpvar_2).xyz
	0x3b, 0x0a, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, // ;.  highp vec4 t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // mpvar_3;.  tmpva
	0x72, 0x5f, 0x33, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, // r_3.w = 0.0;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, // mpvar_3.xyz = a_
	0x62, 0x69, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x62, // bitangent;.  v_b
	0x69, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, 0x74, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x6d, 0x6f, // itangent = (u_mo
	0x64, 0x65, 0x6c, 0x5b, 0x30, 0x5d, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // del[0] * tmpvar_
	0x33, 0x29, 0x2e, 0x78, 0x79, 0x7a, 0x3b, 0x0a, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, // 3).xyz;.  highp 
	0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x3b, 0x0a, 0x20, // vec4 tmpvar_4;. 
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x31, 0x2e, //  tmpvar_4.w = 1.
	0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x78, 0x79, // 0;.  tmpvar_4.xy
	0x7a, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, // z = a_position;.
	0x20, 0x20, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, //   gl_Position = 
	0x28, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, // (u_modelViewProj
	0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, //  * tmpvar_4);.}.
	0x0a, 0x00,                                                                                     // ..
};