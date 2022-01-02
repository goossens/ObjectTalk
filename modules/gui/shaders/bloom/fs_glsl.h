static const uint8_t fs_bloom_glsl[1645] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH.o.><.......s
	0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _texture........
	0xfe, 0x7f, 0x46, 0x06, 0x00, 0x00, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, // ..F...in vec2 v_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, // texcoord0;.unifo
	0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x20, 0x73, 0x5f, 0x74, // rm sampler2D s_t
	0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, // exture;.void mai
	0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x73, 0x75, // n ().{.  vec4 su
	0x6d, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x76, // m_2;.  sum_2 = v
	0x65, 0x63, 0x34, 0x28, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, // ec4(0.0, 0.0, 0.
	0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x28, // 0, 0.0);.  for (
	0x69, 0x6e, 0x74, 0x20, 0x79, 0x5f, 0x31, 0x20, 0x3d, 0x20, 0x2d, 0x33, 0x3b, 0x20, 0x79, 0x5f, // int y_1 = -3; y_
	0x31, 0x20, 0x3c, 0x20, 0x33, 0x3b, 0x20, 0x79, 0x5f, 0x31, 0x2b, 0x2b, 0x29, 0x20, 0x7b, 0x0a, // 1 < 3; y_1++) {.
	0x20, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, //     vec2 tmpvar_
	0x33, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, // 3;.    tmpvar_3.
	0x78, 0x20, 0x3d, 0x20, 0x2d, 0x33, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, // x = -3.0;.    tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, // pvar_3.y = float
	0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, // (y_1);.    sum_2
	0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, //  = (sum_2 + (tex
	0x74, 0x75, 0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, // ture (s_texture,
	0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, //  (v_texcoord0 + 
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, // .      (tmpvar_3
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, //  * 0.004).    ))
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x76, //  * 0.25));.    v
	0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x3b, 0x0a, 0x20, 0x20, // ec2 tmpvar_4;.  
	0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x2d, //   tmpvar_4.x = -
	0x32, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // 2.0;.    tmpvar_
	0x34, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, // 4.y = float(y_1)
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, // ;.    sum_2 = (s
	0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, // um_2 + (texture 
	0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, // (s_texture, (v_t
	0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, // excoord0 + .    
	0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x20, 0x2a, 0x20, 0x30, 0x2e, //   (tmpvar_4 * 0.
	0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, // 004).    )) * 0.
	0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, // 25));.    vec2 t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, // mpvar_5;.    tmp
	0x76, 0x61, 0x72, 0x5f, 0x35, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x2d, 0x31, 0x2e, 0x30, 0x3b, 0x0a, // var_5.x = -1.0;.
	0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x2e, 0x79, 0x20, 0x3d, //     tmpvar_5.y =
	0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, //  float(y_1);.   
	0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, //  sum_2 = (sum_2 
	0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, // + (texture (s_te
	0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, // xture, (v_texcoo
	0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, // rd0 + .      (tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, // pvar_5 * 0.004).
	0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, //     )) * 0.25));
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // .    vec2 tmpvar
	0x5f, 0x36, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, // _6;.    tmpvar_6
	0x2e, 0x78, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, // .x = 0.0;.    tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, // pvar_6.y = float
	0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, // (y_1);.    sum_2
	0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, //  = (sum_2 + (tex
	0x74, 0x75, 0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, // ture (s_texture,
	0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, //  (v_texcoord0 + 
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, // .      (tmpvar_6
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, //  * 0.004).    ))
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x76, //  * 0.25));.    v
	0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x3b, 0x0a, 0x20, 0x20, // ec2 tmpvar_7;.  
	0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x31, //   tmpvar_7.x = 1
	0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, // .0;.    tmpvar_7
	0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, // .y = float(y_1);
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, // .    sum_2 = (su
	0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, 0x28, // m_2 + (texture (
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, // s_texture, (v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, // xcoord0 + .     
	0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, //  (tmpvar_7 * 0.0
	0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, // 04).    )) * 0.2
	0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, // 5));.    vec2 tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, // pvar_8;.    tmpv
	0x61, 0x72, 0x5f, 0x38, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x32, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, // ar_8.x = 2.0;.  
	0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, //   tmpvar_8.y = f
	0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, // loat(y_1);.    s
	0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, // um_2 = (sum_2 + 
	0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, // (texture (s_text
	0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, // ure, (v_texcoord
	0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, // 0 + .      (tmpv
	0x61, 0x72, 0x5f, 0x38, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, // ar_8 * 0.004).  
	0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, //   )) * 0.25));. 
	0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, //  };.  vec4 tmpva
	0x72, 0x5f, 0x39, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x20, // r_9;.  tmpvar_9 
	0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, // = texture (s_tex
	0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, // ture, v_texcoord
	0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, // 0);.  float tmpv
	0x61, 0x72, 0x5f, 0x31, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // ar_10;.  tmpvar_
	0x31, 0x30, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, // 10 = (((tmpvar_9
	0x2e, 0x78, 0x20, 0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x2e, 0x79, 0x29, // .x + tmpvar_9.y)
	0x20, 0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x2e, 0x7a, 0x29, 0x20, 0x2f, //  + tmpvar_9.z) /
	0x20, 0x33, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x28, 0x74, 0x6d, //  3.0);.  if ((tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x33, 0x29, 0x29, 0x20, // pvar_10 < 0.3)) 
	0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, // {.    gl_FragCol
	0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2a, 0x20, // or = (((sum_2 * 
	0x73, 0x75, 0x6d, 0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x31, 0x32, 0x29, 0x20, // sum_2) * 0.012) 
	0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x7d, // + tmpvar_9);.  }
	0x20, 0x65, 0x6c, 0x73, 0x65, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, //  else {.    if (
	0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x35, // (tmpvar_10 < 0.5
	0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, // )) {.      gl_Fr
	0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, // agColor = (((sum
	0x5f, 0x32, 0x20, 0x2a, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, // _2 * sum_2) * 0.
	0x30, 0x30, 0x39, 0x29, 0x20, 0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x29, // 009) + tmpvar_9)
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x20, 0x7b, 0x0a, 0x20, // ;.    } else {. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, //      gl_FragColo
	0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2a, 0x20, 0x73, // r = (((sum_2 * s
	0x75, 0x6d, 0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x37, 0x35, 0x29, 0x20, // um_2) * 0.0075) 
	0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, // + tmpvar_9);.   
	0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                   //  };.  };.}...
};