static const uint8_t OtSkySphereVS_essl[574] =
{
	0x56, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x1e, 0x3e, 0x3c, 0x01, 0x00, 0x0f, 0x75, // VSH.....o.><...u
	0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x04, 0x01, // _modelViewProj..
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x02, 0x00, 0x00, 0x70, 0x72, 0x65, 0x63, // ............prec
	0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x69, 0x6e, 0x74, 0x3b, // ision highp int;
	0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .precision highp
	0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, //  float;.in highp
	0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, //  vec3 a_position
	0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, // ;.in highp vec2 
	0x61, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x6f, 0x75, 0x74, // a_texcoord0;.out
	0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, //  highp vec2 v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, // xcoord0;.uniform
	0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x6d, 0x61, 0x74, 0x34, 0x20, 0x75, 0x5f, 0x6d, 0x6f, //  highp mat4 u_mo
	0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x3b, 0x0a, 0x76, 0x6f, 0x69, // delViewProj;.voi
	0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x68, 0x69, // d main ().{.  hi
	0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x70, 0x6f, 0x73, 0x5f, 0x31, 0x3b, 0x0a, // ghp vec4 pos_1;.
	0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, //   highp vec2 tmp
	0x76, 0x61, 0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // var_2;.  tmpvar_
	0x32, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x28, 0x31, 0x2e, 0x30, 0x20, 0x2d, 0x20, 0x61, 0x5f, 0x74, // 2.x = (1.0 - a_t
	0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x2e, 0x78, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, // excoord0.x);.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x61, 0x5f, 0x74, 0x65, // mpvar_2.y = a_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x2e, 0x79, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x5f, 0x74, // xcoord0.y;.  v_t
	0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // excoord0 = tmpva
	0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, // r_2;.  highp vec
	0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, // 4 tmpvar_3;.  tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, // pvar_3.w = 1.0;.
	0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, //   tmpvar_3.xyz =
	0x20, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x20, 0x20, 0x68, //  a_position;.  h
	0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // ighp vec4 tmpvar
	0x5f, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x20, 0x3d, // _4;.  tmpvar_4 =
	0x20, 0x28, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, //  (u_modelViewPro
	0x6a, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x29, 0x3b, 0x0a, 0x20, // j * tmpvar_3);. 
	0x20, 0x70, 0x6f, 0x73, 0x5f, 0x31, 0x2e, 0x78, 0x79, 0x77, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, //  pos_1.xyw = tmp
	0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x78, 0x79, 0x77, 0x3b, 0x0a, 0x20, 0x20, 0x70, 0x6f, 0x73, // var_4.xyw;.  pos
	0x5f, 0x31, 0x2e, 0x7a, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, // _1.z = tmpvar_4.
	0x77, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, // w;.  gl_Position
	0x20, 0x3d, 0x20, 0x70, 0x6f, 0x73, 0x5f, 0x31, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,             //  = pos_1;.}...
};