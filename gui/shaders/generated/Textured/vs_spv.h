static const uint8_t OtTexturedVS_spv[2632] =
{
	0x56, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0xd9, 0x1e, 0xec, 0xa8, 0x03, 0x00, 0x07, 0x75, // VSH............u
	0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x04, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // _model..........
	0x0b, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x04, 0x01, 0x00, 0x08, // .u_modelView....
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, // .......u_modelVi
	0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x04, 0x01, 0x40, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // ewProj..@.......
	0xe8, 0x09, 0x00, 0x00, 0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x08, 0x00, // ......#.........
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, // ................
	0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, // ........GLSL.std
	0x2e, 0x34, 0x35, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, // .450............
	0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, // ................
	0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, // main....`...c...
	0x66, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, // f...j...w...{...
	0x7e, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x85, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, // ~...............
	0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, // ................
	0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, // ....main........
	0x26, 0x00, 0x00, 0x00, 0x55, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x42, 0x6c, 0x6f, 0x63, 0x6b, // &...UniformBlock
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........&.......
	0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x00, 0x06, 0x00, 0x06, 0x00, 0x26, 0x00, 0x00, 0x00, // u_model.....&...
	0x01, 0x00, 0x00, 0x00, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x00, // ....u_modelView.
	0x06, 0x00, 0x07, 0x00, 0x26, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x75, 0x5f, 0x6d, 0x6f, // ....&.......u_mo
	0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x00, 0x05, 0x00, 0x03, 0x00, // delViewProj.....
	0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x60, 0x00, 0x00, 0x00, // (...........`...
	0x61, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, // a_normal........
	0x63, 0x00, 0x00, 0x00, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, // c...a_position..
	0x05, 0x00, 0x05, 0x00, 0x66, 0x00, 0x00, 0x00, 0x61, 0x5f, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, // ....f...a_tangen
	0x74, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x61, 0x5f, 0x74, 0x65, // t.......j...a_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x77, 0x00, 0x00, 0x00, // xcoord0.....w...
	0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, // @entryPointOutpu
	0x74, 0x2e, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, // t.gl_Position...
	0x05, 0x00, 0x09, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, // ....{...@entryPo
	0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, 0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, // intOutput.v_norm
	0x61, 0x6c, 0x00, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, // al......~...@ent
	0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, 0x76, 0x5f, // ryPointOutput.v_
	0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x09, 0x00, // position........
	0x81, 0x00, 0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, // ....@entryPointO
	0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, 0x76, 0x5f, 0x74, 0x61, 0x6e, 0x67, 0x65, 0x6e, 0x74, 0x00, // utput.v_tangent.
	0x05, 0x00, 0x0a, 0x00, 0x85, 0x00, 0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, // ........@entryPo
	0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, // intOutput.v_texc
	0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x00, 0x00, 0x05, 0x00, 0x09, 0x00, 0x88, 0x00, 0x00, 0x00, // oord0...........
	0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, // @entryPointOutpu
	0x74, 0x2e, 0x76, 0x5f, 0x76, 0x69, 0x65, 0x77, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, // t.v_view....G...
	0x25, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x48, 0x00, 0x04, 0x00, // %.......@...H...
	0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, // &...........H...
	0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // &.......#.......
	0x48, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, // H...&...........
	0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ....H...&.......
	0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ....H...&.......
	0x23, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, // #.......H...&...
	0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x04, 0x00, // ............H...
	0x26, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, // &...........H...
	0x26, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x40, 0x08, 0x00, 0x00, // &.......#...@...
	0x48, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, // H...&...........
	0x10, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 0x26, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // ....G...&.......
	0x47, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // G...(...".......
	0x47, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // G...(...!.......
	0x47, 0x00, 0x04, 0x00, 0x60, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // G...`...........
	0x47, 0x00, 0x04, 0x00, 0x63, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // G...c...........
	0x47, 0x00, 0x04, 0x00, 0x66, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // G...f...........
	0x47, 0x00, 0x04, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // G...j...........
	0x47, 0x00, 0x04, 0x00, 0x77, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // G...w...........
	0x47, 0x00, 0x04, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // G...{...........
	0x47, 0x00, 0x04, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // G...~...........
	0x47, 0x00, 0x04, 0x00, 0x81, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // G...............
	0x47, 0x00, 0x04, 0x00, 0x85, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // G...............
	0x47, 0x00, 0x04, 0x00, 0x88, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, // G...............
	0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, // ........!.......
	0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, // ............ ...
	0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // ................
	0x17, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // ................
	0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, // ................
	0x15, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ........ .......
	0x2b, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // +...............
	0x2b, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // +...............
	0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, // +..............?
	0x18, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, // ...."...........
	0x15, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ....#... .......
	0x2b, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, // +...#...$... ...
	0x1c, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, // ....%..."...$...
	0x1e, 0x00, 0x05, 0x00, 0x26, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, // ....&...%..."...
	0x22, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x27, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // "... ...'.......
	0x26, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x27, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, // &...;...'...(...
	0x02, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, // ....+.......)...
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // .... ...*.......
	0x22, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00, // "...+.......;...
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // .... ..._.......
	0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, // ....;..._...`...
	0x01, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, // ....;..._...c...
	0x01, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, // ....;..._...f...
	0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x69, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // .... ...i.......
	0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x69, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, // ....;...i...j...
	0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x76, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // .... ...v.......
	0x0b, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x76, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, // ....;...v...w...
	0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // .... ...z.......
	0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, // ....;...z...{...
	0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, // ....;...z...~...
	0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, // ....;...z.......
	0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // .... ...........
	0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x85, 0x00, 0x00, 0x00, // ....;...........
	0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, // ....;...z.......
	0x03, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, // ....6...........
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, // ................
	0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, // =.......a...`...
	0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, // =.......d...c...
	0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, // =.......g...f...
	0x3d, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, // =.......k...j...
	0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x9a, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, // Q...........d...
	0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x00, // ....Q...........
	0x64, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, // d.......Q.......
	0x9c, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, // ....d.......P...
	0x0b, 0x00, 0x00, 0x00, 0x9d, 0x00, 0x00, 0x00, 0x9a, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x00, // ................
	0x9c, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x2a, 0x00, 0x00, 0x00, // ........A...*...
	0x9e, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, // ....(...)...)...
	0x3d, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x9e, 0x00, 0x00, 0x00, // =..."...........
	0x90, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x9d, 0x00, 0x00, 0x00, // ................
	0x9f, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x08, 0x00, 0x07, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00, // ....O...........
	0xa0, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ................
	0x02, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x00, // ....Q...........
	0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, // d.......Q.......
	0xa5, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, // ....d.......Q...
	0x06, 0x00, 0x00, 0x00, 0xa6, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // ........d.......
	0x50, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xa7, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x00, // P...............
	0xa5, 0x00, 0x00, 0x00, 0xa6, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, // ............A...
	0x2a, 0x00, 0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, // *.......(.......
	0x3d, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, // =..."...........
	0x90, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0xa7, 0x00, 0x00, 0x00, // ................
	0xa9, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x08, 0x00, 0x07, 0x00, 0x00, 0x00, 0xab, 0x00, 0x00, 0x00, // ....O...........
	0xaa, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ................
	0x02, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0xae, 0x00, 0x00, 0x00, // ....Q...........
	0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, // a.......Q.......
	0xaf, 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, // ....a.......Q...
	0x06, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, // ........a.......
	0x50, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xb1, 0x00, 0x00, 0x00, 0xae, 0x00, 0x00, 0x00, // P...............
	0xaf, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, // ........;...A...
	0x2a, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, // *.......(...)...
	0x29, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0xb3, 0x00, 0x00, 0x00, // )...=...".......
	0xb2, 0x00, 0x00, 0x00, 0x90, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, // ................
	0xb1, 0x00, 0x00, 0x00, 0xb3, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x08, 0x00, 0x07, 0x00, 0x00, 0x00, // ........O.......
	0xb5, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, // ........Q.......
	0xb8, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, // ....g.......Q...
	0x06, 0x00, 0x00, 0x00, 0xb9, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // ........g.......
	0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0xba, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x00, // Q...........g...
	0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, 0xbb, 0x00, 0x00, 0x00, // ....P...........
	0xb8, 0x00, 0x00, 0x00, 0xb9, 0x00, 0x00, 0x00, 0xba, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00, // ............;...
	0x41, 0x00, 0x06, 0x00, 0x2a, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, // A...*.......(...
	0x29, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, // )...)...=..."...
	0xbd, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x90, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, // ................
	0xbe, 0x00, 0x00, 0x00, 0xbb, 0x00, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x08, 0x00, // ............O...
	0x07, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x00, 0xbe, 0x00, 0x00, 0x00, 0xbe, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, // ............Q...
	0x06, 0x00, 0x00, 0x00, 0xc4, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........d.......
	0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, // Q...........d...
	0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, // ....Q...........
	0x64, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, // d.......P.......
	0xc7, 0x00, 0x00, 0x00, 0xc4, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, // ................
	0x1d, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x2a, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, // ....A...*.......
	0x28, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, // (.......=..."...
	0xc9, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x90, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, // ................
	0xca, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x00, 0xc9, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, // ............>...
	0x77, 0x00, 0x00, 0x00, 0xca, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x7b, 0x00, 0x00, 0x00, // w.......>...{...
	0xb5, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00, // ....>...~.......
	0x3e, 0x00, 0x03, 0x00, 0x81, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, // >...........>...
	0x85, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x88, 0x00, 0x00, 0x00, // ....k...>.......
	0xab, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x04, 0x02, 0x00, // ........8.......
	0x01, 0x00, 0x03, 0x00, 0x10, 0x00, 0x80, 0x08,                                                 // ........
};