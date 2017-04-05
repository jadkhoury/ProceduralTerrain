#version 410 core
in vec2 uv;
in vec4 viewDir;
in vec4 lightDir;
in vec4 vpointMV;
in vec4 pos3d;
in vec4 vNormal;
in vec4 pos3dOnSphere;

out vec3 color;

uniform sampler2D tex, tex_mirror, tex_refract, tex_noise, tex_dudv;
uniform sampler2D tex_shadows;
uniform mat4 inv_MV, MV, MVP, light_MVP;
uniform vec3 l_a, l_d, l_s, light_pos;
uniform float grid_size;
uniform vec3 cam_pos;
uniform float time;
uniform vec2 noise_displacement;
uniform int raymarching;

const float alpha = 40;
const float air = 1.0;
const float water = 1.330;
const float eta = air / water;
//cF http://www.wikiwand.com/en/Refractive_index#Reflectivity
const float R0 = ((air - water) * (air - water)) / ((air + water) * (air + water));

vec3 lightning, reflection, refraction;

vec4 readNormal(vec2 uv){
    vec3 tmp = texture(tex, uv).rgb;
    tmp = vec3(tmp.x*2.0-1.0, tmp.y*2.0-1.0, tmp.z);
    tmp.xy /= 3.0;
    return vec4(tmp, 1.0);
}
vec2 readDuDv(vec2 uv){
    return 2.0 * texture(tex_dudv, uv).rg - 1.0;
}

vec2 getUV(vec3 pos){
    return (pos.xy + vec2(grid_size/2.0)) / grid_size;
}

float getTerrainHeigh(vec3 point){
    vec2 uv = getUV(point);
    return texture(tex_noise, uv).r;
}

//Reusable intersect function (ray-marching)
vec3 intersect(vec4 originPoint, vec4 direction, float targetPrecision, int maxIteration){
    //First test to see if the water is visible, 0.05 offset to avoid aliasing
    if (originPoint.z < getTerrainHeigh(originPoint.xyz) - 0.05)
        return vec3(0.0);

    vec3 step = normalize(direction.xyz)*0.02;
    int found = 0;
    vec3 p = originPoint.xyz;
    vec3 lastP;
    for (int i = 0; i < maxIteration; ++i){
        lastP = p;
        if (p.z >getTerrainHeigh(p)){
            if(found>0)
                step = step/2.0;
            p = p + step;
        } else {
            if (found == 0)
                found = 1;
            step = step/2.0;
            p = p - step;
        }
        if (distance(p, lastP) < targetPrecision)
            return p;
    }
    return p;
}

float distanceToTerrain(vec4 point){
    vec4 direction =  pos3d - vec4(cam_pos, 1.0);
    vec3 pointOnTerrain = intersect(pos3d, direction, 0.001, 100);
    return distance(pointOnTerrain, point.xyz);
}

bool inShadows(){
    mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0,
                            0.5, 0.5, 0.5, 1.0 );
    vec4 shadowCoord = biasMatrix * light_MVP * pos3d;
    float bias = 0.0001;
    return (texture(tex_shadows, shadowCoord.xy).r < shadowCoord.z-bias);
}


void main() {
    //Computing reflection and refraction UVs with offsets from dudvMap
    vec2 current_uv = uv + noise_displacement;
    vec2 texcoord2 = vec2(-current_uv.x*20.0+time/20.0, current_uv.y*20.0);
    vec2 texcoord1 = vec2(current_uv.x*20.0-time/15.0, current_uv.y*20.0-time/15.0);
    vec2 dudv1 = readDuDv(texcoord1);
    vec2 dudv2 = readDuDv(texcoord2);
    vec2 offset =  (dudv1 + dudv2) / 200;
    float coef = 1.0 - smoothstep(-0.1,  0.0, getTerrainHeigh(pos3d.xyz));
    offset *= coef;
    ivec2 size = textureSize(tex_mirror, 0);
    float normalizedU = gl_FragCoord.x / size.x;
    float normalizedV = gl_FragCoord.y / size.y;
    vec2 _uv = vec2(normalizedU, normalizedV) + offset/4.0; // coord for refraction
    vec2 uv_mirror = vec2(normalizedU + offset.x , 1.0 - normalizedV - offset.y); // coord for reflection
    vec4 waterNormal = vec4(0.0, 0.0, 1.0, 1.0);


    //Reflection
    reflection = texture(tex_mirror, uv_mirror).rgb;

    // Lightning using a normalmap for the water
    vec4 normal1 = readNormal(texcoord1);
    vec4 normal2 = readNormal(texcoord2);
    vec3 l = normalize(lightDir.xyz);
    vec3 v = normalize(viewDir.xyz);
    vec4 normal = vec4(normal1 + normal2);
    vec3 n = normalize(vec3(inv_MV * normal));
    vec3 r = normalize(reflect(-l, n));
    float rv = max(0.0, dot(r,v));
    vec3 specular = vec3(1.0) * pow(rv, alpha) * l_s;
    lightning = specular;

    //Computing Fresnel Term
    vec3 waternormal_mv = normalize(vec3(inv_MV*waterNormal));
    float fresnel = R0 + (1.0 - R0) * pow((1.0 - dot(v, waternormal_mv)), 5.0) + cam_pos.z/2.0;
    bool pureReflection = (fresnel > 1.0);
    fresnel = clamp(fresnel, 0.0, 1.0);

    vec3 transparency = vec3(0.0);
    if(raymarching > 0){
        //Transparency (depth effect + refraction texture)
        float depth;
        if(!pureReflection){
            vec3 refraction = texture(tex_refract, _uv).rgb;
            depth = distanceToTerrain(pos3d);
            depth = clamp(depth+0.1, 0.0, 1.0);
            vec3 depthColor = vec3(0.0, 0.1, 0.2);
            transparency = mix(refraction, depthColor, depth);
        }
    } else {
        transparency = texture(tex_refract, _uv).rgb;
    }

    color = mix(transparency, reflection, fresnel);
    if(!inShadows()) color += specular*fresnel;
}

