vec4 v_position_world  : TEXCOORD0 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 v_position_screen : TEXCOORD1 = vec4(0.0, 0.0, 0.0, 0.0);
vec3 v_normal          : NORMAL    = vec3(0.0, 1.0, 0.0);
vec3 v_tangent         : TANGENT   = vec3(1.0, 0.0, 0.0);
vec3 v_bitangent       : BINORMAL  = vec3(0.0, 0.0, 1.0);
vec4 v_shadow          : TEXCOORD2 = vec4(0.0, 0.0, 0.0, 0.0);

vec3 a_position        : POSITION;
