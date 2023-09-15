#version 410 core

#define EPSILON 1e-5
#define PI 3.14159265358979323846264338327
#define GAMMA_POWER 0.4545454545454545
#define NUM_LIGHTS 10

out vec4 col;
in vec3 v_normal;
in vec2 v_uv;
in vec3 v_wPos;
in vec3 v_tan;
in vec4 v_vCol;
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

uniform int texFlag;


// Frostbite dedicated values

/* 
    Common dielectrics have 4% reflectance. 
    For gemstones, f0 goes approximately 
    from 8% for ruby 
    to 17% for diamond
    Water is 2%
*/
uniform vec3 c_reflectance; // reflectance coefficient 

float smoothDistanceAtt(float squaredDistance , float invSqrAttRadius) {
    float factor = squaredDistance * invSqrAttRadius;
    float smoothFactor = clamp(1.0 - factor * factor,0.,1.);
    return smoothFactor * smoothFactor;
}
float getDistanceAtt(vec3 unormalizedLightVector , float invSqrAttRadius) {
    float sqrDist = dot(unormalizedLightVector , unormalizedLightVector);
    float attenuation = 1.0 / (max(sqrDist, 0.01*0.01));
    attenuation *= smoothDistanceAtt(sqrDist , invSqrAttRadius);
    return attenuation;
}

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

// Schlick Approximation of Fresnel Effect
vec3 FresnelSchlick(vec3 f0, vec3 f90, float u){
    return f0 + (f90 - f0) * pow(1. - u, 5.);
}


// Disney's diffuse model with normalization of energy, since the original one broke the energy consevation
// constraint to guarantee color consistency across different roughness values. 
float normalizedDisneyDiffuse(float ndv, float ndl, float ldh, float linearRoughness){
    float energyBias = mix(0., 0.5, linearRoughness);
    float energyFactor = mix(1.0, 0.6622516556291391, linearRoughness); // 1.0 / 1.51
    float fd90 = energyBias + 2.0 * ldh * ldh * linearRoughness;
    vec3 f0 = vec3(1.);
    float Lscattering = FresnelSchlick(f0, vec3(fd90), ndl).r;
    float Vscattering = FresnelSchlick(f0, vec3(fd90), ndv).r;

    return Lscattering * Vscattering * energyFactor;
}

// Masking function using Height-correlated Smith and GGX, V model.
float HeightCorrelatedSmithGGX(float ndv, float ndl, float r){
    // Unoptimized formula
    // lambda_v = ( -1 + sqrt ( alphaG2 * (1 - NdotL2 ) / NdotL2 + 1) ) * 0.5 f ;
    // lambda_l = ( -1 + sqrt ( alphaG2 * (1 - NdotV2 ) / NdotV2 + 1) ) * 0.5 f ;
    // G_ SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l ) ;
    // V_ SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV ) ;

    // Optimized version
    float r2 = r * r;
    float GGXVLambda = ndl * sqrt((-ndv * r2 + ndv) * ndv + r2);
    float GGXLLambda = ndv * sqrt((-ndl * r2 + ndl) * ndl + r2);

    return 0.5 / max(GGXVLambda + GGXLLambda, EPSILON);
}

float GGX(float ndh, float m){
    float m2 = m*m;
    float f = (ndh * m2 - ndh) * ndh + 1;
    return m2 / (f*f);
}

vec4 BSDF(vec3 N, vec3 V, vec3 L, vec3 f0, vec3 f90, float roughness, vec3 lightRadiance, vec3 diffuseColor){
    float linearRoughness = roughness * roughness;
    float ndv = abs(dot(N, V)) + 1e-5f; // avoid artifact
    vec3 H = normalize(V + L);
    float ldh = clamp(dot(L, H), 0., 1.);
    float ndh = clamp(dot(N, H), 0., 1.);  
    float ndl = clamp(dot(N, L), 0., 1.);

    // Specular BRDF
    vec3 F = FresnelSchlick(f0, f90, ldh);
    float Vis = HeightCorrelatedSmithGGX(ndv, ndl, roughness);
    float D = GGX(ndh, roughness);
    vec3 Fr = D * F * Vis ;

    // Diffuse BRDF
    float Fd = normalizedDisneyDiffuse(ndv, ndl, ldh, linearRoughness) ;
    // return vec4(Fd);
    float Fd2 = normalizedDisneyDiffuse(ndv, ndl, ldh, roughness) ;
    vec3 bsdf = (Fd * diffuseColor + Fr) * lightRadiance * ndl/ PI;
    // return (isnan(Vis) || isinf(Vis))? vec4(1.0, 0., 1., 1.) : vec4(bsdf, 1.);
    return vec4(bsdf,1.);
}


void main(){
    vec2 uv = (v_uv + uv_transform.xy)  * uv_transform.zw;
    vec4 texAlbedo =   texture(t_Albedo, uv);
    if (length(texAlbedo.xyz) < EPSILON){
        texAlbedo = c_Base;
    }
    // texAlbedo *= vec4(v_vCol.xyz, 1.);
    vec4 texNormal =   texture(t_Normal, uv);
    vec4 texMr =       texture(t_Mr, uv);
    float ao =         texture(t_Ao, uv).r ;
    vec4 texEmissive = texture(t_Emissive, uv);
    texNormal.xyz =  TStoWSvector(texNormal.xyz) ;
    if(texAlbedo.a < 0.75){
        discard;
    }

    vec3 dielectricF0 = 0.16 * c_reflectance * c_reflectance;
    dielectricF0 = vec3(0.04);

    vec3 f0 = mix(dielectricF0, texAlbedo.xyz, texMr.b);
    vec3 diffCol = mix(texAlbedo.xyz, vec3(0.), texMr.b);

    vec3 v = normalize(l_CamPos - v_wPos);
    vec4 lOut = vec4(0.);

    for(int i =0 ; i < NUM_LIGHTS; i++){
        vec3 f90 = l_Color[i];
        vec3 unnormalized_l = l_Pos[i] - v_wPos;
        vec3 L = normalize(unnormalized_l);
        float dist = length(unnormalized_l);
        float attenuation = getDistanceAtt(unnormalized_l, 2.5e-5 );  // 1/pow(lightRadius,2)
        vec3 lRad = (l_Color[i] * l_Strength[i]) * attenuation;

        lOut += BSDF(texNormal.xyz, v, L, f0, f90, texMr.g , lRad,diffCol);
    }
    vec3 ambient = l_ambient.xyz * l_ambient.w * texAlbedo.xyz;
    if (ao > EPSILON){
        ambient *= ao;
    }
    vec4 brdfCol = vec4(ambient,0.) + lOut;
    brdfCol += texEmissive;

    // HDR + gamma
    brdfCol.xyz /= (brdfCol.xyz + vec3(1.));
    brdfCol.xyz = pow(brdfCol.xyz, vec3(GAMMA_POWER));
    col = brdfCol;
    // col = vec4(isnan(brdfCol));


}
