static const uint8_t OtSkySphereFS_glsl[362] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0c, 0x75, // FSH.o.><.......u
	0x5f, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x02, 0x03, 0x00, 0x00, 0x03, // _background.....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x73, 0x5f, 0x73, 0x6b, 0x79, 0x53, 0x70, 0x68, 0x65, 0x72, // ......s_skySpher
	0x65, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // eTexture........
	0x00, 0x00, 0x23, 0x01, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, // ..#...varying ve
	0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, 0x0a, // c2 v_texcoord0;.
	0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x75, 0x5f, 0x62, // uniform vec4 u_b
	0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5b, 0x33, 0x5d, 0x3b, 0x0a, 0x75, 0x6e, // ackground[3];.un
	0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x20, // iform sampler2D 
	0x73, 0x5f, 0x73, 0x6b, 0x79, 0x53, 0x70, 0x68, 0x65, 0x72, 0x65, 0x54, 0x65, 0x78, 0x74, 0x75, // s_skySphereTextu
	0x72, 0x65, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, // re;.void main ()
	0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // .{.  vec4 tmpvar
	0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, // _1;.  tmpvar_1.w
	0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, //  = 1.0;.  tmpvar
	0x5f, 0x31, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x62, 0x61, 0x63, 0x6b, // _1.xyz = (u_back
	0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5b, 0x30, 0x5d, 0x2e, 0x78, 0x20, 0x2a, 0x20, 0x70, 0x6f, // ground[0].x * po
	0x77, 0x20, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x44, 0x20, 0x28, 0x73, 0x5f, // w (texture2D (s_
	0x73, 0x6b, 0x79, 0x53, 0x70, 0x68, 0x65, 0x72, 0x65, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, // skySphereTexture
	0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x2e, 0x78, // , v_texcoord0).x
	0x79, 0x7a, 0x2c, 0x20, 0x75, 0x5f, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, // yz, u_background
	0x5b, 0x30, 0x5d, 0x2e, 0x79, 0x79, 0x79, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, // [0].yyy));.  gl_
	0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, // FragColor = tmpv
	0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                     // ar_1;.}...
};