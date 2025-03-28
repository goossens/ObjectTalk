static const uint8_t OtIblIrradianceMapCS_mtl[2342] =
{
	0x43, 0x53, 0x48, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x73, // CSH............s
	0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x01, // _cubemapSampler.
	0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, // ..........s_cube
	0x6d, 0x61, 0x70, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x01, 0x01, 0xff, 0xff, 0x01, 0x00, // mapTexture......
	0x00, 0x00, 0x00, 0x00, 0x08, 0x73, 0x5f, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x01, 0x01, 0xff, // .....s_target...
	0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, // ........s_cubema
	0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x06, // p...............
	0x00, 0xad, 0x08, 0x00, 0x00, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x6d, // .....#include <m
	0x65, 0x74, 0x61, 0x6c, 0x5f, 0x73, 0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, 0x69, 0x6e, // etal_stdlib>.#in
	0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, 0x6d, 0x64, // clude <simd/simd
	0x2e, 0x68, 0x3e, 0x0a, 0x0a, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x73, // .h>..using names
	0x70, 0x61, 0x63, 0x65, 0x20, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x6b, 0x65, 0x72, // pace metal;..ker
	0x6e, 0x65, 0x6c, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, // nel void xlatMtl
	0x4d, 0x61, 0x69, 0x6e, 0x28, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x63, 0x75, 0x62, 0x65, // Main(texturecube
	0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, // <float> s_cubema
	0x70, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x30, 0x29, 0x5d, 0x5d, // p [[texture(0)]]
	0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x64, 0x5f, 0x61, 0x72, 0x72, 0x61, // , texture2d_arra
	0x79, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x2c, 0x20, 0x61, 0x63, 0x63, 0x65, 0x73, 0x73, 0x3a, // y<float, access:
	0x3a, 0x77, 0x72, 0x69, 0x74, 0x65, 0x3e, 0x20, 0x73, 0x5f, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, // :write> s_target
	0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x31, 0x29, 0x5d, 0x5d, 0x2c, //  [[texture(1)]],
	0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, //  sampler s_cubem
	0x61, 0x70, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, // apSampler [[samp
	0x6c, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x33, 0x20, // ler(0)]], uint3 
	0x67, 0x6c, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, 0x76, 0x6f, 0x63, 0x61, 0x74, // gl_GlobalInvocat
	0x69, 0x6f, 0x6e, 0x49, 0x44, 0x20, 0x5b, 0x5b, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x5f, 0x70, // ionID [[thread_p
	0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x69, 0x6e, 0x5f, 0x67, 0x72, 0x69, 0x64, 0x5d, // osition_in_grid]
	0x5d, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x20, // ]).{.    float2 
	0x5f, 0x34, 0x34, 0x30, 0x20, 0x3d, 0x20, 0x28, 0x28, 0x28, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, // _440 = (((float2
	0x28, 0x69, 0x6e, 0x74, 0x33, 0x28, 0x67, 0x6c, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, // (int3(gl_GlobalI
	0x6e, 0x76, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x49, 0x44, 0x29, 0x2e, 0x78, 0x79, 0x29, // nvocationID).xy)
	0x20, 0x2b, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x30, 0x2e, 0x35, 0x29, 0x29, 0x20, //  + float2(0.5)) 
	0x2a, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x30, 0x2e, 0x30, 0x31, 0x35, 0x36, 0x32, // * float2(0.01562
	0x35, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x32, 0x2e, 0x30, 0x29, 0x20, 0x2d, 0x20, 0x66, 0x6c, 0x6f, // 5)) * 2.0) - flo
	0x61, 0x74, 0x32, 0x28, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, // at2(1.0);.    fl
	0x6f, 0x61, 0x74, 0x20, 0x5f, 0x34, 0x34, 0x32, 0x20, 0x3d, 0x20, 0x5f, 0x34, 0x34, 0x30, 0x2e, // oat _442 = _440.
	0x79, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x5f, 0x34, 0x34, // y;.    float _44
	0x33, 0x20, 0x3d, 0x20, 0x5f, 0x34, 0x34, 0x32, 0x20, 0x2a, 0x20, 0x28, 0x2d, 0x31, 0x2e, 0x30, // 3 = _442 * (-1.0
	0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x20, 0x5f, 0x35, // );.    float3 _5
	0x34, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x77, 0x69, 0x74, 0x63, 0x68, 0x20, 0x28, // 41;.    switch (
	0x69, 0x6e, 0x74, 0x33, 0x28, 0x67, 0x6c, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, // int3(gl_GlobalIn
	0x76, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x49, 0x44, 0x29, 0x2e, 0x7a, 0x29, 0x0a, 0x20, // vocationID).z). 
	0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, 0x61, 0x73, //    {.        cas
	0x65, 0x20, 0x30, 0x3a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, // e 0:.        {. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x31, 0x20, //            _541 
	0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x5f, 0x34, // = float3(1.0, _4
	0x34, 0x33, 0x2c, 0x20, 0x2d, 0x5f, 0x34, 0x34, 0x30, 0x2e, 0x78, 0x29, 0x3b, 0x0a, 0x20, 0x20, // 43, -_440.x);.  
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, //           break;
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, // .        }.     
	0x20, 0x20, 0x20, 0x63, 0x61, 0x73, 0x65, 0x20, 0x31, 0x3a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, //    case 1:.     
	0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //    {.           
	0x20, 0x5f, 0x35, 0x34, 0x31, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x2d, //  _541 = float3(-
	0x31, 0x2e, 0x30, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x33, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x30, 0x2e, // 1.0, _443, _440.
	0x78, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // x);.            
	0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, // break;.        }
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, 0x61, 0x73, 0x65, 0x20, 0x32, 0x3a, // .        case 2:
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, // .        {.     
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x31, 0x20, 0x3d, 0x20, 0x66, 0x6c, //        _541 = fl
	0x6f, 0x61, 0x74, 0x33, 0x28, 0x5f, 0x34, 0x34, 0x30, 0x2e, 0x78, 0x2c, 0x20, 0x31, 0x2e, 0x30, // oat3(_440.x, 1.0
	0x2c, 0x20, 0x5f, 0x34, 0x34, 0x32, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // , _442);.       
	0x20, 0x20, 0x20, 0x20, 0x20, 0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, //      break;.    
	0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, 0x61, //     }.        ca
	0x73, 0x65, 0x20, 0x33, 0x3a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, // se 3:.        {.
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x31, //             _541
	0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x5f, 0x34, 0x34, 0x30, 0x2e, 0x78, //  = float3(_440.x
	0x2c, 0x20, 0x2d, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x33, 0x29, 0x3b, 0x0a, 0x20, // , -1.0, _443);. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x62, 0x72, 0x65, 0x61, 0x6b, //            break
	0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, // ;.        }.    
	0x20, 0x20, 0x20, 0x20, 0x63, 0x61, 0x73, 0x65, 0x20, 0x34, 0x3a, 0x0a, 0x20, 0x20, 0x20, 0x20, //     case 4:.    
	0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //     {.          
	0x20, 0x20, 0x5f, 0x35, 0x34, 0x31, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, //   _541 = float3(
	0x5f, 0x34, 0x34, 0x30, 0x2e, 0x78, 0x2c, 0x20, 0x5f, 0x34, 0x34, 0x33, 0x2c, 0x20, 0x31, 0x2e, // _440.x, _443, 1.
	0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 0);.            
	0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, // break;.        }
	0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, // .        default
	0x3a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, // :.        {.    
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x31, 0x20, 0x3d, 0x20, 0x66, //         _541 = f
	0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x2d, 0x5f, 0x34, 0x34, 0x30, 0x2e, 0x78, 0x2c, 0x20, 0x5f, // loat3(-_440.x, _
	0x34, 0x34, 0x33, 0x2c, 0x20, 0x2d, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // 443, -1.0);.    
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, 0x0a, 0x20, //         break;. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, //        }.    }. 
	0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x20, 0x5f, 0x33, 0x34, 0x33, 0x20, 0x3d, //    float3 _343 =
	0x20, 0x66, 0x61, 0x73, 0x74, 0x3a, 0x3a, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69, 0x7a, 0x65, //  fast::normalize
	0x28, 0x5f, 0x35, 0x34, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, // (_541);.    floa
	0x74, 0x33, 0x20, 0x5f, 0x33, 0x35, 0x33, 0x20, 0x3d, 0x20, 0x66, 0x61, 0x73, 0x74, 0x3a, 0x3a, // t3 _353 = fast::
	0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69, 0x7a, 0x65, 0x28, 0x63, 0x72, 0x6f, 0x73, 0x73, 0x28, // normalize(cross(
	0x73, 0x65, 0x6c, 0x65, 0x63, 0x74, 0x28, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x31, 0x2e, // select(float3(1.
	0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x2c, 0x20, 0x66, 0x6c, // 0, 0.0, 0.0), fl
	0x6f, 0x61, 0x74, 0x33, 0x28, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x31, // oat3(0.0, 0.0, 1
	0x2e, 0x30, 0x29, 0x2c, 0x20, 0x62, 0x6f, 0x6f, 0x6c, 0x33, 0x28, 0x61, 0x62, 0x73, 0x28, 0x5f, // .0), bool3(abs(_
	0x33, 0x34, 0x33, 0x2e, 0x7a, 0x29, 0x20, 0x3c, 0x20, 0x30, 0x2e, 0x39, 0x39, 0x39, 0x30, 0x30, // 343.z) < 0.99900
	0x30, 0x30, 0x31, 0x32, 0x38, 0x37, 0x34, 0x36, 0x30, 0x33, 0x32, 0x37, 0x31, 0x34, 0x38, 0x34, // 0012874603271484
	0x33, 0x37, 0x35, 0x29, 0x29, 0x2c, 0x20, 0x5f, 0x33, 0x34, 0x33, 0x29, 0x29, 0x3b, 0x0a, 0x20, // 375)), _343));. 
	0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x20, 0x5f, 0x33, 0x35, 0x36, 0x20, 0x3d, //    float3 _356 =
	0x20, 0x63, 0x72, 0x6f, 0x73, 0x73, 0x28, 0x5f, 0x33, 0x34, 0x33, 0x2c, 0x20, 0x5f, 0x33, 0x35, //  cross(_343, _35
	0x33, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x5f, 0x35, // 3);.    float _5
	0x34, 0x32, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x20, 0x5f, // 42;.    float3 _
	0x35, 0x34, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x20, 0x5f, 0x35, // 543;.    uint _5
	0x34, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x34, 0x20, 0x3d, 0x20, 0x30, // 44;.    _544 = 0
	0x75, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x33, 0x20, 0x3d, 0x20, 0x66, 0x6c, // u;.    _543 = fl
	0x6f, 0x61, 0x74, 0x33, 0x28, 0x30, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x5f, // oat3(0.0);.    _
	0x35, 0x34, 0x32, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, // 542 = 0.0;.    f
	0x6c, 0x6f, 0x61, 0x74, 0x33, 0x20, 0x5f, 0x35, 0x34, 0x37, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // loat3 _547;.    
	0x75, 0x69, 0x6e, 0x74, 0x20, 0x5f, 0x35, 0x34, 0x38, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, // uint _548;.    f
	0x6f, 0x72, 0x20, 0x28, 0x3b, 0x20, 0x5f, 0x35, 0x34, 0x32, 0x20, 0x3c, 0x20, 0x36, 0x2e, 0x32, // or (; _542 < 6.2
	0x38, 0x33, 0x31, 0x38, 0x35, 0x34, 0x38, 0x32, 0x30, 0x32, 0x35, 0x31, 0x34, 0x36, 0x34, 0x38, // 8318548202514648
	0x34, 0x33, 0x37, 0x35, 0x3b, 0x20, 0x5f, 0x35, 0x34, 0x34, 0x20, 0x3d, 0x20, 0x5f, 0x35, 0x34, // 4375; _544 = _54
	0x38, 0x2c, 0x20, 0x5f, 0x35, 0x34, 0x33, 0x20, 0x3d, 0x20, 0x5f, 0x35, 0x34, 0x37, 0x2c, 0x20, // 8, _543 = _547, 
	0x5f, 0x35, 0x34, 0x32, 0x20, 0x2b, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x31, 0x37, 0x34, 0x35, 0x33, // _542 += 0.017453
	0x32, 0x39, 0x32, 0x33, 0x38, 0x34, 0x37, 0x34, 0x33, 0x36, 0x39, 0x30, 0x34, 0x39, 0x30, 0x37, // 2923847436904907
	0x32, 0x32, 0x36, 0x35, 0x36, 0x32, 0x35, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, // 2265625).    {. 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x38, 0x20, 0x3d, 0x20, 0x5f, 0x35, //        _548 = _5
	0x34, 0x34, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x37, // 44;.        _547
	0x20, 0x3d, 0x20, 0x5f, 0x35, 0x34, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //  = _543;.       
	0x20, 0x66, 0x6f, 0x72, 0x20, 0x28, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x5f, 0x35, 0x34, 0x35, //  for (float _545
	0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x20, 0x5f, 0x35, 0x34, 0x35, 0x20, 0x3c, 0x20, 0x31, //  = 0.0; _545 < 1
	0x2e, 0x35, 0x37, 0x30, 0x37, 0x39, 0x36, 0x33, 0x37, 0x30, 0x35, 0x30, 0x36, 0x32, 0x38, 0x36, // .570796370506286
	0x36, 0x32, 0x31, 0x30, 0x39, 0x33, 0x37, 0x35, 0x3b, 0x20, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, // 62109375; ).    
	0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //     {.          
	0x20, 0x20, 0x5f, 0x35, 0x34, 0x38, 0x2b, 0x2b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //   _548++;.      
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x37, 0x20, 0x2b, 0x3d, 0x20, 0x28, 0x28, //       _547 += ((
	0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, // s_cubemap.sample
	0x28, 0x73, 0x5f, 0x63, 0x75, 0x62, 0x65, 0x6d, 0x61, 0x70, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, // (s_cubemapSample
	0x72, 0x2c, 0x20, 0x28, 0x28, 0x5f, 0x33, 0x34, 0x33, 0x20, 0x2a, 0x20, 0x63, 0x6f, 0x73, 0x28, // r, ((_343 * cos(
	0x5f, 0x35, 0x34, 0x35, 0x29, 0x29, 0x20, 0x2b, 0x20, 0x28, 0x28, 0x28, 0x5f, 0x33, 0x35, 0x33, // _545)) + (((_353
	0x20, 0x2a, 0x20, 0x63, 0x6f, 0x73, 0x28, 0x5f, 0x35, 0x34, 0x32, 0x29, 0x29, 0x20, 0x2b, 0x20, //  * cos(_542)) + 
	0x28, 0x5f, 0x33, 0x35, 0x36, 0x20, 0x2a, 0x20, 0x73, 0x69, 0x6e, 0x28, 0x5f, 0x35, 0x34, 0x32, // (_356 * sin(_542
	0x29, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x73, 0x69, 0x6e, 0x28, 0x5f, 0x35, 0x34, 0x35, 0x29, 0x29, // ))) * sin(_545))
	0x29, 0x2c, 0x20, 0x6c, 0x65, 0x76, 0x65, 0x6c, 0x28, 0x30, 0x2e, 0x30, 0x29, 0x29, 0x2e, 0x78, // ), level(0.0)).x
	0x79, 0x7a, 0x20, 0x2a, 0x20, 0x63, 0x6f, 0x73, 0x28, 0x5f, 0x35, 0x34, 0x35, 0x29, 0x29, 0x20, // yz * cos(_545)) 
	0x2a, 0x20, 0x73, 0x69, 0x6e, 0x28, 0x5f, 0x35, 0x34, 0x35, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, // * sin(_545));.  
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x35, 0x34, 0x35, 0x20, 0x2b, //           _545 +
	0x3d, 0x20, 0x30, 0x2e, 0x30, 0x31, 0x37, 0x34, 0x35, 0x33, 0x32, 0x39, 0x32, 0x33, 0x38, 0x34, // = 0.017453292384
	0x37, 0x34, 0x33, 0x36, 0x39, 0x30, 0x34, 0x39, 0x30, 0x37, 0x32, 0x32, 0x36, 0x35, 0x36, 0x32, // 7436904907226562
	0x35, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, // 5;.            c
	0x6f, 0x6e, 0x74, 0x69, 0x6e, 0x75, 0x65, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // ontinue;.       
	0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x5f, 0x74, //  }.    }.    s_t
	0x61, 0x72, 0x67, 0x65, 0x74, 0x2e, 0x77, 0x72, 0x69, 0x74, 0x65, 0x28, 0x66, 0x6c, 0x6f, 0x61, // arget.write(floa
	0x74, 0x34, 0x28, 0x28, 0x5f, 0x35, 0x34, 0x33, 0x20, 0x2a, 0x20, 0x33, 0x2e, 0x31, 0x34, 0x31, // t4((_543 * 3.141
	0x35, 0x39, 0x32, 0x37, 0x34, 0x31, 0x30, 0x31, 0x32, 0x35, 0x37, 0x33, 0x32, 0x34, 0x32, 0x31, // 5927410125732421
	0x38, 0x37, 0x35, 0x29, 0x20, 0x2f, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x33, 0x28, 0x66, 0x6c, // 875) / float3(fl
	0x6f, 0x61, 0x74, 0x28, 0x5f, 0x35, 0x34, 0x34, 0x29, 0x29, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, // oat(_544)), 1.0)
	0x2c, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x32, 0x28, 0x69, 0x6e, 0x74, 0x33, 0x28, 0x67, 0x6c, 0x5f, // , uint2(int3(gl_
	0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, 0x76, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, // GlobalInvocation
	0x49, 0x44, 0x29, 0x2e, 0x78, 0x79, 0x29, 0x2c, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x28, 0x69, 0x6e, // ID).xy), uint(in
	0x74, 0x33, 0x28, 0x67, 0x6c, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x49, 0x6e, 0x76, 0x6f, // t3(gl_GlobalInvo
	0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x49, 0x44, 0x29, 0x2e, 0x7a, 0x29, 0x29, 0x3b, 0x0a, 0x7d, // cationID).z));.}
	0x0a, 0x0a, 0x00, 0x00, 0x30, 0x00,                                                             // ....0.
};
