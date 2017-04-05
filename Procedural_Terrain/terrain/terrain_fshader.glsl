#version 410

in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;

//For lightning
in vec4 gPointMV;
in vec4 gLightDir;
in vec2 gUV;
in float gHeight;
in float gDist;
in vec4 pos3D;
in vec4 gNormal;

out vec3 color;

uniform float PI;
uniform mat4 MV, MVP, inv_MV, light_MVP;
uniform vec3 l_a, l_d, l_s;
uniform vec3 light_pos;
uniform sampler2D tex_grass, tex_sand, tex_snow, tex_rock, tex_noise;
uniform sampler2DShadow tex_shadows;
uniform float grid_size;
uniform int clip_coef;
uniform vec2 noise_displacement;
uniform int tessellation_demo;
uniform int normals_in_fragment;
uniform int raymarching;

vec3 reflection;
vec3 shading;
vec3 ka = vec3(0.0, 0.3, 0.0);
vec3 kd = vec3(1.0);
vec3 ks = vec3(1.0);
float alpha = 30.0;
float snow_height = 0.7;
float grass_height = 0.02;
float mix_zone = 0.05;


vec2 getUV(vec2 pos){
    return (pos + vec2(grid_size/2.0)) / grid_size;
}

vec4 get3Dcoord(vec2 pos_2D){
    vec2 uv = getUV(pos_2D);
    float h = texture(tex_noise, uv).r;
    return vec4(pos_2D, h, 1.0);
}

vec4 compute_normal(vec4 current_pos) {
    float offset = 1.0/100.0;
    vec2 posx    = current_pos.xy + vec2(offset, 0.0);
    vec2 posxneg = current_pos.xy - vec2(offset, 0.0);
    vec2 posy    = current_pos.xy + vec2(0.0, offset);
    vec2 posyneg = current_pos.xy - vec2(0.0, offset);
    vec4 dx = get3Dcoord(posx) - get3Dcoord(posxneg);
    vec4 dy = get3Dcoord(posy) - get3Dcoord(posyneg);
    return vec4(normalize(cross(dx.xyz, dy.xyz)), 1.0);
}

float amplify(float d, float scale, float offset) {
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}
vec2 poissonDisk[16] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
vec2( 0.94558609, -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2( 0.34495938, 0.29387760 ),
vec2( -0.91588581, 0.45771432 ),
vec2( -0.81544232, -0.87912464 ),
vec2( -0.38277543, 0.27676845 ),
vec2( 0.97484398, 0.75648379 ),
vec2( 0.44323325, -0.97511554 ),
vec2( 0.53742981, -0.47373420 ),
vec2( -0.26496911, -0.41893023 ),
vec2( 0.79197514, 0.19090188 ),
vec2( -0.24188840, 0.99706507 ),
vec2( -0.81409955, 0.91437590 ),
vec2( 0.19984126, 0.78641367 ),
vec2( 0.14383161, -0.14100790 )
);
// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}


// from http://www.iquilezles.org/www/articles/fog/fog.htm
vec3 applyFog(vec3  rgb, float gDistance) {
    float b = 0.1;
    float fogAmount = max(0.0, 1.0 - exp(gDistance*b));
    vec3  fogColor  = vec3(0.5, 0.6, 0.7);
    return mix(rgb, fogColor, fogAmount );
}

vec3 applyFog(vec3 rgb, float gDistance, vec3 rayDir, vec3 sunDir ){
    float b = 0.1;
    float fogAmount = 1.0 - exp( gDistance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,0.7), vec3(1.0,0.9,0.7), pow(sunAmount,8.0));
    return mix( rgb, fogColor, fogAmount );
}


float inShadows(){

    mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0,
                            0.5, 0.5, 0.5, 1.0 );
    vec4 shadowCoord = biasMatrix * light_MVP * pos3D;

    float visibility=1.0;

    float bias = 0.005;

    for (int i=0;i<4;i++){
        int index = int(16.0*random(floor(pos3D.xyz*1000.0), i))%16;
        visibility -= 0.2*(1.0-texture( tex_shadows, vec3(shadowCoord.xy + poissonDisk[index]/700.0,  (shadowCoord.z-bias)/shadowCoord.w) ));
    }
    visibility = (visibility - 0.2)*0.8;
    //if(pos3D.z > light_pos.z){
       // visibility = 0.0; }
    return visibility;


}


vec3 computeDayColor(){
    vec4 currentNormal = (normals_in_fragment > 0) ? compute_normal(pos3D) : gNormal;
    vec3 result;
    vec3 v = normalize(-gPointMV.xyz);
    vec3 n = normalize(vec3(inv_MV * currentNormal));
    vec3 l = normalize(gLightDir.xyz);
    vec3 r = normalize(reflect(-l, n));

    vec2 current_uv = gUV + noise_displacement;
    vec3 snow = texture(tex_snow, current_uv*100).rgb;
    vec3 grass = texture(tex_grass, current_uv*30).rgb;
    vec3 sand = texture(tex_sand, current_uv*50).rgb;
    vec3 rock = texture(tex_rock, current_uv*100).rgb;
    vec3 vertical = vec3(0, 0, 1);

    float angleDiff = abs(dot(currentNormal.xyz, vertical));
    float pureRock = 0.6;
    float lerpRock = 0.7;
    float coef = 1.0 - smoothstep(pureRock, lerpRock, angleDiff);
    grass = mix(grass, rock, coef);
    snow = mix(snow, rock, coef);
    coef = smoothstep(0.90, 0.98, angleDiff);
    grass = mix(grass, snow, coef);


    if (gHeight > snow_height + mix_zone){
        kd = snow;
    } else if (gHeight > snow_height - mix_zone) {
        float coef = (gHeight-(snow_height - mix_zone))/(2.0 * mix_zone);
        kd = mix(grass, snow, coef);
    } else if (gHeight > grass_height + mix_zone){
        kd = grass;
    } else if (gHeight > grass_height - mix_zone){
        float coef = (gHeight-(grass_height - mix_zone))/(2.0 * mix_zone);
        kd = mix(sand, grass, coef);
    } else {
        kd = sand;
    }

    vec3 ambiant = kd*l_a;

    float nl = max(0.0, dot(n,l));
    float shadowCoef = 1.0;
    vec3 diffuse = kd * nl * l_d;
    if(clip_coef == 0){
        diffuse *= inShadows();
    }

    if (gHeight < 0.0 && raymarching == 0){
        vec3 depthColor = vec3(0.0, 0.1, 0.2);
        float coef = clamp(1.0 + gDist/2.0, 0.0, 1.0);
        result = mix(depthColor, ambiant + diffuse, coef);
    } else {
        result = vec3(applyFog(ambiant + diffuse, gDist, -v, l)) ;
    }

    return result;
}


vec3 computeNightColor(){
    vec3 v = normalize(-gPointMV.xyz);
    vec3 c = vec3(0.0, 0.2, 0.3);
    vec3 l = normalize(-gLightDir.xyz);
    vec3 nightColor = pow(-gDist/20.0, 0.6) * c;
    return nightColor;
}

void main() {
    float sunHeight = light_pos.z/100.0;
    vec3 day = computeDayColor();
    vec3 night = computeNightColor();
    float coef = smoothstep(-0.3, 0.0, sunHeight);
    color = mix(night, day, coef);

    if(tessellation_demo > 0){

        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);

        float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);

        color *= amplify(d1, 40, -0.5);
        //color *= amplify(d2, 60, -0.5);
    }

}

