#version 410 core

#define EPSILON 1e-5
#define PI 3.14159265358979323846264338327
#define GAMMA_POWER 0.4545454545454545
#define NUM_LIGHTS 10

out vec4 col;
in vec3 v_normal;
in vec2 v_uv;
in vec3 v_wPos;
// Asset ubo
uniform vec4 c_Base;
uniform sampler2D t_Albedo;
uniform sampler2D t_Normal;
uniform sampler2D t_Mr;
uniform sampler2D t_Ao;
uniform sampler2D t_Emissive;
uniform vec4 uv_transform;

// Light ubo
uniform vec3  l_Pos[10];
uniform vec3  l_Color[10];
uniform float l_Strength[10];
uniform vec3 l_CamPos;
uniform vec4 l_ambient;

// UI ubo (for showing specific maps)
uniform int texFlag;

// Convert vector from tangent space to world space, normals are stored in tangent space in the texture.
vec3 TStoWSvector(vec3 v){
    v = v * 2. -1.;

    // Build tangents from fragments (for now)
    vec3 p1 = dFdx(v_wPos);
    vec3 p2 = dFdy(v_wPos);
    vec2 st1 = dFdx(v_uv);
    vec2 st2 = dFdy(v_uv);

    vec3 N = normalize(v_normal);
    vec3 T = normalize(p1 * st2.t - p2*st1.t);
    vec3 B = -normalize(cross(N,T));
    mat3 TBN = mat3(T,B,N);

    return normalize(TBN * v);
}

float GGX(vec3 n, vec3 h, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float ndh = max(dot(n, h), 0.);
    float ndh2 = ndh * ndh;

    float den = (ndh2 * (a2 - 1.) + 1.);
    den *= PI * den;
    return a2 / den;
}

float GGX_G(float adotb, float roughness){
    float r = roughness + 1.;
    float k = (r * r) / 8.;
    float den = adotb * (1. - k) + k;

    return adotb/den;
}

// Smith+GGX Approximation of G from https://media.gdcvault.com/gdc2017/Presentations/Hammon_Earl_PBR_Diffuse_Lighting.pdf
float GGXSmithApproximated(vec3 n, vec3 v, vec3 l, float roughness){
    float ndv = max(dot(n,v), 0.);
    float ndl = max(dot(n,l), 0.);

    float ggx1 = GGX_G(ndl, roughness);
    float ggx2 = GGX_G(ndv, roughness);

    return ggx1 * ggx2;
}

vec3 SchlickFresnel(vec3 a, vec3 b, vec3 f0)
{
    float cosTheta = max(dot(a,b), 0.0);
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec4 BRDF(vec3 f0, vec3 n, vec3 v, float roughness, float metallic, vec3 albedo, float ao){
    vec3 lOut = vec3(0.);

    for(int i=0; i < NUM_LIGHTS; i++){
        if(l_Strength[i] < EPSILON){
            continue;
        }
        // Assume point light model
        vec3 l = normalize(l_Pos[i] - v_wPos);
        vec3 h = normalize( v + l);
        float dist = length(l_Pos[i] - v_wPos);
        float attenuation = 1. / (dist*dist);
        vec3 lRadiance = (l_Color[i] * l_Strength[i]) * attenuation;

        // Cook-Torrance model with GGX NDF, Smith Masking, Schlick Approximated Fresnel
        float D = GGX(n, h, roughness);
        float G = GGXSmithApproximated(n, v, l, roughness);
        vec3 F = SchlickFresnel(h, v, f0);

        float ndl = max(dot(n,l), 0.);

        float denominator = 4. * max(dot(n,v), 0.) * ndl + EPSILON; // epsilon avoid dividing by 0
        vec3 spec = (F * G * D) / denominator;

        vec3 kSpec = F;
        vec3 kDiff = vec3(1.) - kSpec; // Ensures energy conservation
        kDiff *= 1. - metallic; // Metals do not scatter light via internal reflection

        lOut += (kDiff * albedo / PI + spec) * lRadiance * ndl;
    }
    vec3 ambient = l_ambient.xyz * l_ambient.w * albedo;
    if(ao > EPSILON){
        ambient *= ao;
    }
    vec3 color = ambient + lOut;
    return vec4(color, 1.);
}

void main(){
    
    vec2 uv = (v_uv + uv_transform.xy)  * uv_transform.zw;
    vec4 texAlbedo =   texture(t_Albedo, uv);
    if (length(texAlbedo.xyz) < EPSILON){
        texAlbedo = c_Base;
    }
    vec4 texNormal =   texture(t_Normal, uv);
    vec4 texMr =       texture(t_Mr, uv);
    float ao =         texture(t_Ao, uv).r ;
    vec4 texEmissive = texture(t_Emissive, uv);

    texNormal.xyz =  TStoWSvector(texNormal.xyz) ;

    if(texAlbedo.a < 0.75){
        discard;
    }
    vec3 f0 = mix(vec3(0.04),texAlbedo.xyz, texMr.b);
    vec3 v = normalize(l_CamPos - v_wPos);

    vec4 brdfCol = BRDF(f0, texNormal.xyz, v, texMr.g, texMr.b, texAlbedo.xyz, ao);
    brdfCol += texEmissive;

    // HDR + gamma
    brdfCol.xyz /= (brdfCol.xyz + vec3(1.));
    brdfCol.xyz = pow(brdfCol.xyz, vec3(GAMMA_POWER));
    col = brdfCol;

}