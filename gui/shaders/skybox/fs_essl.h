static const uint8_t fs_skybox_essl[241] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH.o.><.......s
	0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _cubemap........
	0xfe, 0x7f, 0xca, 0x00, 0x00, 0x00, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, // ......precision 
	0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x69, 0x6e, 0x74, 0x3b, 0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, // highp int;.preci
	0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, // sion highp float
	0x3b, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, // ;.out lowp vec4 
	0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, 0x61, 0x30, 0x3b, 0x0a, // bgfx_FragData0;.
	0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, // in vec3 v_texcoo
	0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6c, 0x6f, 0x77, // rd0;.uniform low
	0x70, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x43, 0x75, 0x62, 0x65, 0x20, 0x73, 0x5f, // p samplerCube s_
	0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, // cubemap;.void ma
	0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, // in ().{.  bgfx_F
	0x72, 0x61, 0x67, 0x44, 0x61, 0x74, 0x61, 0x30, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, // ragData0 = textu
	0x72, 0x65, 0x20, 0x28, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x2c, 0x20, 0x76, // re (s_cubemap, v
	0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, // _texcoord0);.}..
	0x00,                                                                                           // .
};