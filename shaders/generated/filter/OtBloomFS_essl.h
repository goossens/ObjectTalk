static const uint8_t OtBloomFS_essl[1821] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH.o.><.......s
	0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _texture........
	0x00, 0x00, 0xf6, 0x06, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, // ......varying hi
	0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, // ghp vec2 v_texco
	0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, // ord0;.uniform sa
	0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, // mpler2D s_textur
	0x65, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, // e;.void main ().
	0x7b, 0x0a, 0x20, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x73, 0x75, // {.  lowp vec4 su
	0x6d, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, // m_2;.  lowp vec4
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, //  tmpvar_3;.  tmp
	0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, // var_3.x = 0.0;. 
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x30, 0x2e, //  tmpvar_3.y = 0.
	0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x7a, 0x20, // 0;.  tmpvar_3.z 
	0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // = 0.0;.  tmpvar_
	0x33, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x73, 0x75, 0x6d, // 3.w = 0.0;.  sum
	0x5f, 0x32, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, // _2 = tmpvar_3;. 
	0x20, 0x66, 0x6f, 0x72, 0x20, 0x28, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x69, 0x6e, 0x74, 0x20, //  for (highp int 
	0x79, 0x5f, 0x31, 0x20, 0x3d, 0x20, 0x2d, 0x33, 0x3b, 0x20, 0x79, 0x5f, 0x31, 0x20, 0x3c, 0x20, // y_1 = -3; y_1 < 
	0x33, 0x3b, 0x20, 0x79, 0x5f, 0x31, 0x2b, 0x2b, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, // 3; y_1++) {.    
	0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // highp vec2 tmpva
	0x72, 0x5f, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // r_4;.    tmpvar_
	0x34, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x2d, 0x33, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // 4.x = -3.0;.    
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, // tmpvar_4.y = flo
	0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, // at(y_1);.    sum
	0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, // _2 = (sum_2 + (t
	0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, // exture2D (s_text
	0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, // ure, (v_texcoord
	0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, // 0 + .      (tmpv
	0x61, 0x72, 0x5f, 0x34, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, // ar_4 * 0.004).  
	0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, //   )) * 0.25));. 
	0x20, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, //    highp vec2 tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, // pvar_5;.    tmpv
	0x61, 0x72, 0x5f, 0x35, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x2d, 0x32, 0x2e, 0x30, 0x3b, 0x0a, 0x20, // ar_5.x = -2.0;. 
	0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x2e, 0x79, 0x20, 0x3d, 0x20, //    tmpvar_5.y = 
	0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // float(y_1);.    
	0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, // sum_2 = (sum_2 +
	0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, 0x73, 0x5f, 0x74, //  (texture2D (s_t
	0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, // exture, (v_texco
	0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, // ord0 + .      (t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, // mpvar_5 * 0.004)
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, // .    )) * 0.25))
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, // ;.    highp vec2
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, //  tmpvar_6;.    t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x2d, 0x31, 0x2e, 0x30, // mpvar_6.x = -1.0
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x2e, 0x79, // ;.    tmpvar_6.y
	0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, //  = float(y_1);. 
	0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, //    sum_2 = (sum_
	0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, // 2 + (texture2D (
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, // s_texture, (v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, // xcoord0 + .     
	0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, //  (tmpvar_6 * 0.0
	0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, // 04).    )) * 0.2
	0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, // 5));.    highp v
	0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x3b, 0x0a, 0x20, 0x20, // ec2 tmpvar_7;.  
	0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x30, //   tmpvar_7.x = 0
	0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, // .0;.    tmpvar_7
	0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, 0x29, 0x3b, // .y = float(y_1);
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x73, 0x75, // .    sum_2 = (su
	0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, // m_2 + (texture2D
	0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, 0x76, 0x5f, //  (s_texture, (v_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, 0x20, 0x20, // texcoord0 + .   
	0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x20, 0x2a, 0x20, 0x30, //    (tmpvar_7 * 0
	0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x30, // .004).    )) * 0
	0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .25));.    highp
	0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x3b, 0x0a, //  vec2 tmpvar_8;.
	0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x2e, 0x78, 0x20, 0x3d, //     tmpvar_8.x =
	0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, //  1.0;.    tmpvar
	0x5f, 0x38, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, 0x5f, 0x31, // _8.y = float(y_1
	0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, // );.    sum_2 = (
	0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, // sum_2 + (texture
	0x32, 0x44, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x28, // 2D (s_texture, (
	0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x0a, 0x20, // v_texcoord0 + . 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x20, 0x2a, //      (tmpvar_8 *
	0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20, 0x2a, //  0.004).    )) *
	0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x68, 0x69, 0x67, //  0.25));.    hig
	0x68, 0x70, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, // hp vec2 tmpvar_9
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x2e, 0x78, // ;.    tmpvar_9.x
	0x20, 0x3d, 0x20, 0x32, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, //  = 2.0;.    tmpv
	0x61, 0x72, 0x5f, 0x39, 0x2e, 0x79, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x28, 0x79, // ar_9.y = float(y
	0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x3d, // _1);.    sum_2 =
	0x20, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, //  (sum_2 + (textu
	0x72, 0x65, 0x32, 0x44, 0x20, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, // re2D (s_texture,
	0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, //  (v_texcoord0 + 
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, // .      (tmpvar_9
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, //  * 0.004).    ))
	0x20, 0x2a, 0x20, 0x30, 0x2e, 0x32, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, //  * 0.25));.  };.
	0x20, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, //   lowp vec4 tmpv
	0x61, 0x72, 0x5f, 0x31, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // ar_10;.  tmpvar_
	0x31, 0x30, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, // 10 = texture2D (
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, // s_texture, v_tex
	0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, // coord0);.  lowp 
	0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x3b, // float tmpvar_11;
	0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x20, 0x3d, 0x20, 0x28, // .  tmpvar_11 = (
	0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x2e, 0x78, 0x20, 0x2b, 0x20, // ((tmpvar_10.x + 
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x2e, 0x79, 0x29, 0x20, 0x2b, 0x20, 0x74, // tmpvar_10.y) + t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x2e, 0x7a, 0x29, 0x20, 0x2f, 0x20, 0x33, 0x2e, // mpvar_10.z) / 3.
	0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, // 0);.  if ((tmpva
	0x72, 0x5f, 0x31, 0x31, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x33, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, // r_11 < 0.3)) {. 
	0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, //    gl_FragColor 
	0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2a, 0x20, 0x73, 0x75, 0x6d, // = (((sum_2 * sum
	0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x31, 0x32, 0x29, 0x20, 0x2b, 0x20, 0x74, // _2) * 0.012) + t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x20, 0x65, // mpvar_10);.  } e
	0x6c, 0x73, 0x65, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x28, 0x74, // lse {.    if ((t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, // mpvar_11 < 0.5))
	0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, //  {.      gl_Frag
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, // Color = (((sum_2
	0x20, 0x2a, 0x20, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, //  * sum_2) * 0.00
	0x39, 0x29, 0x20, 0x2b, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x29, 0x3b, // 9) + tmpvar_10);
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x20, 0x7b, 0x0a, 0x20, 0x20, // .    } else {.  
	0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, //     gl_FragColor
	0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x73, 0x75, 0x6d, 0x5f, 0x32, 0x20, 0x2a, 0x20, 0x73, 0x75, //  = (((sum_2 * su
	0x6d, 0x5f, 0x32, 0x29, 0x20, 0x2a, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x37, 0x35, 0x29, 0x20, 0x2b, // m_2) * 0.0075) +
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, //  tmpvar_10);.   
	0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                   //  };.  };.}...
};