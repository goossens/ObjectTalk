static const uint8_t OtGeometryTerrainVS_spv[1702] =
{
	0x56, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x17, 0x8e, 0xa9, 0x02, 0x00, 0x07, 0x75, // VSH............u
	0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x04, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // _model..........
	0x0f, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, // .u_modelViewProj
	0x04, 0x01, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x03, 0x02, // ..........`.....
	0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x08, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, // #...............
	0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, // ................
	0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 0x00, 0x00, // ..GLSL.std.450..
	0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, // ................
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, // ..........main..
	0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x53, 0x00, // ..C...F...O...S.
	0x00, 0x00, 0x56, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, 0x01, // ..V.............
	0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, // ..........main..
	0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x55, 0x6e, 0x69, 0x66, 0x6f, 0x72, // ..........Unifor
	0x6d, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x1f, 0x00, // mBlock..........
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, 0x6c, 0x00, 0x06, 0x00, // ......u_model...
	0x07, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x75, 0x5f, 0x6d, 0x6f, 0x64, 0x65, // ..........u_mode
	0x6c, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x00, 0x05, 0x00, 0x03, 0x00, 0x21, 0x00, // lViewProj.....!.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x43, 0x00, 0x00, 0x00, 0x61, 0x5f, // ..........C...a_
	0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x46, 0x00, // normal........F.
	0x00, 0x00, 0x61, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x05, 0x00, // ..a_position....
	0x0a, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, // ..O...@entryPoin
	0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, // tOutput.gl_Posit
	0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x05, 0x00, 0x09, 0x00, 0x53, 0x00, 0x00, 0x00, 0x40, 0x65, // ion.......S...@e
	0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x2e, // ntryPointOutput.
	0x76, 0x5f, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x00, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x56, 0x00, // v_normal......V.
	0x00, 0x00, 0x40, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x4f, 0x75, 0x74, // ..@entryPointOut
	0x70, 0x75, 0x74, 0x2e, 0x76, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, // put.v_position..
	0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x40, 0x00, // ..G...........@.
	0x00, 0x00, 0x48, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, // ..H.............
	0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, // ..H...........#.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, // ......H.........
	0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x04, 0x00, 0x1f, 0x00, // ..........H.....
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1f, 0x00, // ..........H.....
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x48, 0x00, // ......#.......H.
	0x05, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, // ................
	0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, // ..G...........G.
	0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, // ..!...".......G.
	0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, // ..!...!.......G.
	0x04, 0x00, 0x43, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, // ..C...........G.
	0x04, 0x00, 0x46, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, // ..F...........G.
	0x04, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, // ..O...........G.
	0x04, 0x00, 0x53, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, // ..S...........G.
	0x04, 0x00, 0x56, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, // ..V.............
	0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, // ......!.........
	0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, // .......... .....
	0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x17, 0x00, // ................
	0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x15, 0x00, // ................
	0x04, 0x00, 0x12, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, // ...... .......+.
	0x04, 0x00, 0x12, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, // ..............+.
	0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x18, 0x00, // .............?..
	0x04, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x15, 0x00, // ................
	0x04, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, // ...... .......+.
	0x04, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x1c, 0x00, // .......... .....
	0x04, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x1e, 0x00, // ................
	0x04, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x20, 0x00, // .............. .
	0x04, 0x00, 0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3b, 0x00, // .. ...........;.
	0x04, 0x00, 0x20, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2b, 0x00, // .. ...!.......+.
	0x04, 0x00, 0x12, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, // ......"....... .
	0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x2b, 0x00, // ..#...........+.
	0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, // ......*....... .
	0x04, 0x00, 0x42, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, // ..B...........;.
	0x04, 0x00, 0x42, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3b, 0x00, // ..B...C.......;.
	0x04, 0x00, 0x42, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, // ..B...F....... .
	0x04, 0x00, 0x4e, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, // ..N...........;.
	0x04, 0x00, 0x4e, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, // ..N...O....... .
	0x04, 0x00, 0x52, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, // ..R...........;.
	0x04, 0x00, 0x52, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, // ..R...S.......;.
	0x04, 0x00, 0x52, 0x00, 0x00, 0x00, 0x56, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x36, 0x00, // ..R...V.......6.
	0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, // ................
	0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, // ..........=.....
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, // ..D...C...=.....
	0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, // ..G...F...Q.....
	0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, // ..i...G.......Q.
	0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x01, 0x00, // ......j...G.....
	0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x00, 0x00, 0x47, 0x00, // ..Q.......k...G.
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x09, 0x00, 0x00, 0x00, 0x6c, 0x00, // ......P.......l.
	0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x00, 0x00, 0x16, 0x00, // ..i...j...k.....
	0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x23, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x21, 0x00, // ..A...#...m...!.
	0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x1b, 0x00, // .."..."...=.....
	0x00, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x90, 0x00, 0x05, 0x00, 0x09, 0x00, // ..n...m.........
	0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x4f, 0x00, // ..o...l...n...O.
	0x08, 0x00, 0x07, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x6f, 0x00, // ......p...o...o.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x51, 0x00, // ..............Q.
	0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, // ......s...D.....
	0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x44, 0x00, // ..Q.......t...D.
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x75, 0x00, // ......Q.......u.
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x09, 0x00, // ..D.......P.....
	0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x75, 0x00, // ..v...s...t...u.
	0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x41, 0x00, 0x06, 0x00, 0x23, 0x00, 0x00, 0x00, 0x77, 0x00, // ..*...A...#...w.
	0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x3d, 0x00, // ..!..."..."...=.
	0x04, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x90, 0x00, // ......x...w.....
	0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x78, 0x00, // ......y...v...x.
	0x00, 0x00, 0x4f, 0x00, 0x08, 0x00, 0x07, 0x00, 0x00, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x79, 0x00, // ..O.......z...y.
	0x00, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, // ..y.............
	0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x7d, 0x00, 0x00, 0x00, 0x47, 0x00, // ..Q.......}...G.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x7e, 0x00, // ......Q.......~.
	0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, // ..G.......Q.....
	0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, // ......G.......P.
	0x07, 0x00, 0x09, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x7d, 0x00, 0x00, 0x00, 0x7e, 0x00, // ..........}...~.
	0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x23, 0x00, // ..........A...#.
	0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x3d, 0x00, // ......!.......=.
	0x04, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x90, 0x00, // ................
	0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x82, 0x00, // ................
	0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x3e, 0x00, // ..>...O.......>.
	0x03, 0x00, 0x53, 0x00, 0x00, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x56, 0x00, // ..S...z...>...V.
	0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x02, // ..p.......8.....
	0x02, 0x00, 0x01, 0x00, 0x40, 0x08,                                                             // ....@.
};