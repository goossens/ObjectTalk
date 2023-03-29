static const uint8_t OtGridFS_essl[1500] =
{
	0x46, 0x53, 0x48, 0x0b, 0x4e, 0x87, 0x44, 0x57, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0a, 0x75, // FSH.N.DW.......u
	0x5f, 0x76, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x04, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, // _viewProj.......
	0x00, 0x00, 0x00, 0x06, 0x75, 0x5f, 0x67, 0x72, 0x69, 0x64, 0x02, 0x01, 0x00, 0x00, 0x01, 0x00, // ....u_grid......
	0x00, 0x00, 0x00, 0x00, 0xa3, 0x05, 0x00, 0x00, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, // ........precisio
	0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x69, 0x6e, 0x74, 0x3b, 0x0a, 0x70, 0x72, 0x65, // n highp int;.pre
	0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x66, 0x6c, 0x6f, // cision highp flo
	0x61, 0x74, 0x3b, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x6d, 0x65, 0x64, 0x69, 0x75, 0x6d, 0x70, 0x20, // at;.out mediump 
	0x76, 0x65, 0x63, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, // vec4 bgfx_FragCo
	0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, 0x5f, 0x66, // lor;.in vec3 v_f
	0x61, 0x72, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76, 0x5f, 0x6e, 0x65, // ar;.in vec3 v_ne
	0x61, 0x72, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6d, 0x61, 0x74, 0x34, // ar;.uniform mat4
	0x20, 0x75, 0x5f, 0x76, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x3b, 0x0a, 0x75, 0x6e, 0x69, //  u_viewProj;.uni
	0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x75, 0x5f, 0x67, 0x72, 0x69, 0x64, // form vec4 u_grid
	0x5b, 0x31, 0x5d, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, // [1];.void main (
	0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, // ).{.  vec4 color
	0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, // _1;.  float tmpv
	0x61, 0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, // ar_2;.  tmpvar_2
	0x20, 0x3d, 0x20, 0x28, 0x2d, 0x28, 0x76, 0x5f, 0x6e, 0x65, 0x61, 0x72, 0x2e, 0x79, 0x29, 0x20, //  = (-(v_near.y) 
	0x2f, 0x20, 0x28, 0x76, 0x5f, 0x66, 0x61, 0x72, 0x2e, 0x79, 0x20, 0x2d, 0x20, 0x76, 0x5f, 0x6e, // / (v_far.y - v_n
	0x65, 0x61, 0x72, 0x2e, 0x79, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x28, // ear.y));.  if ((
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x29, // tmpvar_2 < 0.0))
	0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x64, 0x69, 0x73, 0x63, 0x61, 0x72, 0x64, 0x3b, 0x0a, //  {.    discard;.
	0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x74, 0x6d, 0x70, 0x76, //   };.  vec3 tmpv
	0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, // ar_3;.  tmpvar_3
	0x20, 0x3d, 0x20, 0x28, 0x76, 0x5f, 0x6e, 0x65, 0x61, 0x72, 0x20, 0x2b, 0x20, 0x28, 0x74, 0x6d, //  = (v_near + (tm
	0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x20, 0x2a, 0x20, 0x28, 0x76, 0x5f, 0x66, 0x61, 0x72, 0x20, // pvar_2 * (v_far 
	0x2d, 0x20, 0x76, 0x5f, 0x6e, 0x65, 0x61, 0x72, 0x29, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, // - v_near)));.  v
	0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x3b, 0x0a, 0x20, 0x20, // ec4 tmpvar_4;.  
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, // tmpvar_4.w = 1.0
	0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x34, 0x2e, 0x78, 0x79, 0x7a, // ;.  tmpvar_4.xyz
	0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x76, //  = tmpvar_3;.  v
	0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x3b, 0x0a, 0x20, 0x20, // ec4 tmpvar_5;.  
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x20, 0x3d, 0x20, 0x28, 0x75, 0x5f, 0x76, 0x69, // tmpvar_5 = (u_vi
	0x65, 0x77, 0x50, 0x72, 0x6f, 0x6a, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // ewProj * tmpvar_
	0x34, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // 4);.  vec3 tmpva
	0x72, 0x5f, 0x36, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x20, // r_6;.  tmpvar_6 
	0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x2e, 0x78, 0x79, 0x7a, 0x20, // = (tmpvar_5.xyz 
	0x2f, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x35, 0x2e, 0x77, 0x29, 0x3b, 0x0a, 0x20, // / tmpvar_5.w);. 
	0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x3b, //  float tmpvar_7;
	0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x20, 0x3d, 0x20, 0x28, 0x28, // .  tmpvar_7 = ((
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x36, 0x2e, 0x7a, 0x20, 0x2b, 0x20, 0x31, 0x2e, 0x30, // tmpvar_6.z + 1.0
	0x29, 0x20, 0x2f, 0x20, 0x32, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, // ) / 2.0);.  vec2
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, //  tmpvar_8;.  tmp
	0x76, 0x61, 0x72, 0x5f, 0x38, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // var_8 = (tmpvar_
	0x33, 0x2e, 0x78, 0x7a, 0x20, 0x2f, 0x20, 0x75, 0x5f, 0x67, 0x72, 0x69, 0x64, 0x5b, 0x30, 0x5d, // 3.xz / u_grid[0]
	0x2e, 0x78, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, // .x);.  vec2 tmpv
	0x61, 0x72, 0x5f, 0x39, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, // ar_9;.  tmpvar_9
	0x20, 0x3d, 0x20, 0x28, 0x61, 0x62, 0x73, 0x28, 0x64, 0x46, 0x64, 0x78, 0x28, 0x74, 0x6d, 0x70, //  = (abs(dFdx(tmp
	0x76, 0x61, 0x72, 0x5f, 0x38, 0x29, 0x29, 0x20, 0x2b, 0x20, 0x61, 0x62, 0x73, 0x28, 0x64, 0x46, // var_8)) + abs(dF
	0x64, 0x79, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x29, 0x29, 0x29, 0x3b, 0x0a, // dy(tmpvar_8)));.
	0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, //   vec2 tmpvar_10
	0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x20, 0x3d, 0x20, // ;.  tmpvar_10 = 
	0x28, 0x61, 0x62, 0x73, 0x28, 0x28, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x72, 0x61, 0x63, 0x74, // (abs((.    fract
	0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x38, 0x20, 0x2d, 0x20, 0x30, 0x2e, 0x35, // ((tmpvar_8 - 0.5
	0x29, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x2d, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x20, 0x2f, 0x20, // )).   - 0.5)) / 
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x66, 0x6c, 0x6f, // tmpvar_9);.  flo
	0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x3b, 0x0a, 0x20, 0x20, // at tmpvar_11;.  
	0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x20, 0x3d, 0x20, 0x6d, 0x69, 0x6e, 0x20, // tmpvar_11 = min 
	0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x39, 0x2e, 0x79, 0x2c, 0x20, 0x31, 0x2e, 0x30, // (tmpvar_9.y, 1.0
	0x29, 0x3b, 0x0a, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // );.  float tmpva
	0x72, 0x5f, 0x31, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, // r_12;.  tmpvar_1
	0x32, 0x20, 0x3d, 0x20, 0x6d, 0x69, 0x6e, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // 2 = min (tmpvar_
	0x39, 0x2e, 0x78, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, // 9.x, 1.0);.  vec
	0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, // 4 tmpvar_13;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x33, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x3d, 0x20, 0x76, // mpvar_13.xyz = v
	0x65, 0x63, 0x33, 0x28, 0x30, 0x2e, 0x36, 0x2c, 0x20, 0x30, 0x2e, 0x36, 0x2c, 0x20, 0x30, 0x2e, // ec3(0.6, 0.6, 0.
	0x36, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x33, 0x2e, // 6);.  tmpvar_13.
	0x77, 0x20, 0x3d, 0x20, 0x28, 0x31, 0x2e, 0x30, 0x20, 0x2d, 0x20, 0x6d, 0x69, 0x6e, 0x20, 0x28, // w = (1.0 - min (
	0x6d, 0x69, 0x6e, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x2e, 0x78, // min (tmpvar_10.x
	0x2c, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x30, 0x2e, 0x79, 0x29, 0x2c, 0x20, // , tmpvar_10.y), 
	0x31, 0x2e, 0x30, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x5f, 0x31, // 1.0));.  color_1
	0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x33, 0x3b, 0x0a, 0x20, 0x20, //  = tmpvar_13;.  
	0x69, 0x66, 0x20, 0x28, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x78, // if (((tmpvar_3.x
	0x20, 0x3e, 0x20, 0x28, 0x2d, 0x30, 0x2e, 0x31, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, //  > (-0.1 * tmpva
	0x72, 0x5f, 0x31, 0x32, 0x29, 0x29, 0x20, 0x26, 0x26, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, // r_12)) && (tmpva
	0x72, 0x5f, 0x33, 0x2e, 0x78, 0x20, 0x3c, 0x20, 0x28, 0x30, 0x2e, 0x31, 0x20, 0x2a, 0x20, 0x74, // r_3.x < (0.1 * t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x31, 0x29, 0x29, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, // mpvar_11)))) {. 
	0x20, 0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x5f, 0x31, 0x2e, 0x7a, 0x20, 0x3d, 0x20, 0x31, //    color_1.z = 1
	0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x28, // .0;.  };.  if ((
	0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x7a, 0x20, 0x3e, 0x20, 0x28, 0x2d, // (tmpvar_3.z > (-
	0x30, 0x2e, 0x31, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x32, 0x29, // 0.1 * tmpvar_12)
	0x29, 0x20, 0x26, 0x26, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x7a, // ) && (tmpvar_3.z
	0x20, 0x3c, 0x20, 0x28, 0x30, 0x2e, 0x31, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, //  < (0.1 * tmpvar
	0x5f, 0x31, 0x31, 0x29, 0x29, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x63, 0x6f, // _11)))) {.    co
	0x6c, 0x6f, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x20, 0x3d, 0x20, 0x31, 0x2e, 0x30, 0x3b, 0x0a, 0x20, // lor_1.x = 1.0;. 
	0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, //  };.  color_1.w 
	0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x33, 0x2e, 0x77, 0x20, 0x2a, // = (tmpvar_13.w *
	0x20, 0x6d, 0x69, 0x6e, 0x20, 0x28, 0x28, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x28, 0x31, 0x2e, 0x30, //  min ((.    (1.0
	0x20, 0x2d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x37, 0x29, 0x0a, 0x20, 0x20, 0x20, //  - tmpvar_7).   
	0x2a, 0x20, 0x31, 0x2e, 0x32, 0x29, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x29, 0x3b, 0x0a, 0x20, // * 1.2), 1.0));. 
	0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, //  bgfx_FragColor 
	0x3d, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, // = color_1;.  gl_
	0x46, 0x72, 0x61, 0x67, 0x44, 0x65, 0x70, 0x74, 0x68, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, // FragDepth = tmpv
	0x61, 0x72, 0x5f, 0x36, 0x2e, 0x7a, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                         // ar_6.z;.}...
};