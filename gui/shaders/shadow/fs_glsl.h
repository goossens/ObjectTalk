static const uint8_t OtShadowFS_glsl[161] =
{
	0x46, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x00, // FSH.............
	0x00, 0x00, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, // ..void main ().{
	0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, // .  vec4 tmpvar_1
	0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x20, 0x3d, // ;.  tmpvar_1.x =
	0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, //  0.0;.  tmpvar_1
	0x2e, 0x79, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, // .y = 0.0;.  tmpv
	0x61, 0x72, 0x5f, 0x31, 0x2e, 0x7a, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, // ar_1.z = 0.0;.  
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, // tmpvar_1.w = 0.0
	0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, // ;.  gl_FragColor
	0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, //  = tmpvar_1;.}..
	0x00,                                                                                           // .
};