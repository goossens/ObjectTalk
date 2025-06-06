static const uint8_t OtRgbaCurvesFS_mtl[2448] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x10, 0x73, // FSH.o.><.......s
	0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x11, // _textureSampler.
	0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, // ..........s_text
	0x75, 0x72, 0x65, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, // ureTexture......
	0x00, 0x00, 0x00, 0x00, 0x0c, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, // .....s_lutSample
	0x72, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x73, 0x5f, 0x6c, 0x75, // r...........s_lu
	0x74, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, // tTexture........
	0x00, 0x00, 0x0b, 0x75, 0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x12, 0x03, // ...u_rbgcurves..
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, // .........s_textu
	0x72, 0x65, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x73, 0x5f, 0x6c, // re...........s_l
	0x75, 0x74, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x08, 0x00, 0x00, // ut..............
	0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x5f, // #include <metal_
	0x73, 0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, // stdlib>.#include
	0x20, 0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, 0x6d, 0x64, 0x2e, 0x68, 0x3e, 0x0a, 0x0a, //  <simd/simd.h>..
	0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x73, 0x70, 0x61, 0x63, 0x65, 0x20, // using namespace 
	0x6d, 0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x5f, // metal;..struct _
	0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, // Global.{.    flo
	0x61, 0x74, 0x34, 0x20, 0x75, 0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, // at4 u_rbgcurves[
	0x33, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, // 3];.};..struct x
	0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x0a, 0x7b, // latMtlMain_out.{
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, // .    float4 bgfx
	0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, 0x61, 0x30, 0x20, 0x5b, 0x5b, 0x63, 0x6f, 0x6c, // _FragData0 [[col
	0x6f, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, // or(0)]];.};..str
	0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, // uct xlatMtlMain_
	0x69, 0x6e, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x20, // in.{.    float2 
	0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, // v_texcoord0 [[us
	0x65, 0x72, 0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, // er(locn0)]];.};.
	0x0a, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, // .fragment xlatMt
	0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, // lMain_out xlatMt
	0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x28, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, // lMain(xlatMtlMai
	0x6e, 0x5f, 0x69, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x5b, 0x5b, 0x73, 0x74, 0x61, 0x67, 0x65, 0x5f, // n_in in [[stage_
	0x69, 0x6e, 0x5d, 0x5d, 0x2c, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x20, 0x5f, // in]], constant _
	0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x26, 0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x20, 0x5b, // Global& _mtl_u [
	0x5b, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, // [buffer(0)]], te
	0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x64, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, // xture2d<float> s
	0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, // _texture [[textu
	0x72, 0x65, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, // re(0)]], texture
	0x32, 0x64, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x20, // 2d<float> s_lut 
	0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x31, 0x29, 0x5d, 0x5d, 0x2c, 0x20, // [[texture(1)]], 
	0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, // sampler s_textur
	0x65, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, 0x6c, // eSampler [[sampl
	0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // er(0)]], sampler
	0x20, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x5b, 0x5b, //  s_lutSampler [[
	0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x31, 0x29, 0x5d, 0x5d, 0x29, 0x0a, 0x7b, 0x0a, // sampler(1)]]).{.
	0x20, 0x20, 0x20, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, //     xlatMtlMain_
	0x6f, 0x75, 0x74, 0x20, 0x6f, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x7b, 0x7d, 0x3b, 0x0a, 0x20, 0x20, // out out = {};.  
	0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, 0x34, 0x34, 0x35, 0x20, 0x3d, 0x20, //   float4 _445 = 
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, // s_texture.sample
	0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, // (s_textureSample
	0x72, 0x2c, 0x20, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, // r, in.v_texcoord
	0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, // 0);.    float4 _
	0x35, 0x36, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x69, 0x6e, 0x74, // 564;.    if (int
	0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, // (_mtl_u.u_rbgcur
	0x76, 0x65, 0x73, 0x5b, 0x30, 0x5d, 0x2e, 0x78, 0x29, 0x20, 0x3d, 0x3d, 0x20, 0x30, 0x29, 0x0a, // ves[0].x) == 0).
	0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, //     {.        _5
	0x36, 0x34, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x73, 0x5f, 0x6c, 0x75, // 64 = float4(s_lu
	0x74, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, // t.sample(s_lutSa
	0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, // mpler, float2(_4
	0x34, 0x35, 0x2e, 0x78, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, 0x2c, 0x20, 0x73, // 45.x, 0.5)).x, s
	0x5f, 0x6c, 0x75, 0x74, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, // _lut.sample(s_lu
	0x74, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, // tSampler, float2
	0x28, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x79, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, // (_445.y, 0.5)).x
	0x2c, 0x20, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, // , s_lut.sample(s
	0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, // _lutSampler, flo
	0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x7a, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, // at2(_445.z, 0.5)
	0x29, 0x2e, 0x78, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x77, 0x29, 0x3b, 0x0a, 0x20, 0x20, // ).x, _445.w);.  
	0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x20, 0x20, 0x20, //   }.    else.   
	0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, //  {.        float
	0x34, 0x20, 0x5f, 0x35, 0x36, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 4 _565;.        
	0x69, 0x66, 0x20, 0x28, 0x69, 0x6e, 0x74, 0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, // if (int(_mtl_u.u
	0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x30, 0x5d, 0x2e, 0x78, 0x29, // _rbgcurves[0].x)
	0x20, 0x3d, 0x3d, 0x20, 0x31, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, //  == 1).        {
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, // .            _56
	0x35, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, // 5 = float4(s_lut
	0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, // .sample(s_lutSam
	0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, 0x34, // pler, float2(_44
	0x35, 0x2e, 0x78, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, 0x2c, 0x20, 0x5f, 0x34, // 5.x, 0.5)).x, _4
	0x34, 0x35, 0x2e, 0x79, 0x7a, 0x77, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 45.yzw);.       
	0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a, //  }.        else.
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //         {.      
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, 0x35, 0x36, //       float4 _56
	0x36, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x69, // 6;.            i
	0x66, 0x20, 0x28, 0x69, 0x6e, 0x74, 0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, // f (int(_mtl_u.u_
	0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x30, 0x5d, 0x2e, 0x78, 0x29, 0x20, // rbgcurves[0].x) 
	0x3d, 0x3d, 0x20, 0x32, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // == 2).          
	0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //   {.            
	0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x36, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, //     _566 = float
	0x34, 0x28, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x78, 0x2c, 0x20, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x2e, // 4(_445.x, s_lut.
	0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, 0x70, // sample(s_lutSamp
	0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, 0x34, 0x35, // ler, float2(_445
	0x2e, 0x79, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, 0x2c, 0x20, 0x5f, 0x34, 0x34, // .y, 0.5)).x, _44
	0x35, 0x2e, 0x7a, 0x77, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 5.zw);.         
	0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //    }.           
	0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //  else.          
	0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //   {.            
	0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, 0x35, 0x36, 0x37, 0x3b, //     float4 _567;
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // .               
	0x20, 0x69, 0x66, 0x20, 0x28, 0x69, 0x6e, 0x74, 0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, //  if (int(_mtl_u.
	0x75, 0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x30, 0x5d, 0x2e, 0x78, // u_rbgcurves[0].x
	0x29, 0x20, 0x3d, 0x3d, 0x20, 0x33, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // ) == 3).        
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //         {.      
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, //               _5
	0x36, 0x37, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x5f, 0x34, 0x34, 0x35, // 67 = float4(_445
	0x2e, 0x78, 0x79, 0x2c, 0x20, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, // .xy, s_lut.sampl
	0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, // e(s_lutSampler, 
	0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x7a, 0x2c, 0x20, 0x30, // float2(_445.z, 0
	0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x77, 0x29, 0x3b, // .5)).x, _445.w);
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // .               
	0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //  }.             
	0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //    else.        
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //         {.      
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, //               fl
	0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, 0x35, 0x36, 0x38, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, // oat4 _568;.     
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x69, //                i
	0x66, 0x20, 0x28, 0x69, 0x6e, 0x74, 0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, // f (int(_mtl_u.u_
	0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x30, 0x5d, 0x2e, 0x78, 0x29, 0x20, // rbgcurves[0].x) 
	0x3d, 0x3d, 0x20, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // == 4).          
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, //           {.    
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //                 
	0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x38, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, //     _568 = float
	0x34, 0x28, 0x5f, 0x34, 0x34, 0x35, 0x2e, 0x78, 0x79, 0x7a, 0x2c, 0x20, 0x73, 0x5f, 0x6c, 0x75, // 4(_445.xyz, s_lu
	0x74, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x6c, 0x75, 0x74, 0x53, 0x61, // t.sample(s_lutSa
	0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x5f, 0x34, // mpler, float2(_4
	0x34, 0x35, 0x2e, 0x77, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x2e, 0x78, 0x29, 0x3b, 0x0a, // 45.w, 0.5)).x);.
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //                 
	0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //     }.          
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x20, //           else. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //                 
	0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //    {.           
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, //              _56
	0x38, 0x20, 0x3d, 0x20, 0x5f, 0x34, 0x34, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 8 = _445;.      
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, //               }.
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //                 
	0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x37, 0x20, 0x3d, 0x20, 0x5f, 0x35, 0x36, 0x38, 0x3b, //     _567 = _568;
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // .               
	0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //  }.             
	0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x36, 0x20, 0x3d, 0x20, 0x5f, 0x35, 0x36, 0x37, 0x3b, 0x0a, //    _566 = _567;.
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, //             }.  
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x35, 0x20, 0x3d, //           _565 =
	0x20, 0x5f, 0x35, 0x36, 0x36, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, //  _566;.        }
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x36, 0x34, 0x20, 0x3d, 0x20, // .        _564 = 
	0x5f, 0x35, 0x36, 0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, // _565;.    }.    
	0x6f, 0x75, 0x74, 0x2e, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, // out.bgfx_FragDat
	0x61, 0x30, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x28, 0x5f, 0x35, 0x36, // a0 = float4((_56
	0x34, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x2d, 0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, // 4.xyz - _mtl_u.u
	0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x31, 0x5d, 0x2e, 0x78, 0x79, // _rbgcurves[1].xy
	0x7a, 0x29, 0x20, 0x2a, 0x20, 0x28, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x31, 0x2e, 0x30, // z) * (float3(1.0
	0x29, 0x20, 0x2f, 0x20, 0x28, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, 0x72, 0x62, // ) / (_mtl_u.u_rb
	0x67, 0x63, 0x75, 0x72, 0x76, 0x65, 0x73, 0x5b, 0x32, 0x5d, 0x2e, 0x78, 0x79, 0x7a, 0x20, 0x2d, // gcurves[2].xyz -
	0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, 0x72, 0x62, 0x67, 0x63, 0x75, 0x72, //  _mtl_u.u_rbgcur
	0x76, 0x65, 0x73, 0x5b, 0x31, 0x5d, 0x2e, 0x78, 0x79, 0x7a, 0x29, 0x29, 0x2e, 0x78, 0x2c, 0x20, // ves[1].xyz)).x, 
	0x5f, 0x35, 0x36, 0x34, 0x2e, 0x77, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x72, 0x65, 0x74, // _564.w);.    ret
	0x75, 0x72, 0x6e, 0x20, 0x6f, 0x75, 0x74, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00, 0x00, 0x70, 0x00, // urn out;.}....p.
};
