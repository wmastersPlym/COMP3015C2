#version 460

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform struct LightInfo {
    vec4 Position;  // Light position in eye coords.
    vec3 La;        // Ambient light intensity
    vec3 L;         // Diffuse and specular light intensity
} lights[3];

uniform struct MaterialInfo {
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
} Material;

uniform vec3 cameraPos;

layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D Tex2;


layout (location = 0) out vec4 FragColour;

vec3 blinnPhong(vec4 pos, vec3 n) {
    // Get Mix texture colours
    vec4 texColour1 = texture(Tex1, TexCoord);
    vec4 texColour2 = texture(Tex2, TexCoord);
    vec3 texCol = mix(texColour1.rgb, texColour2.rgb, texColour2.a);
    
    vec3 Pf = vec3(0.0f);
    for(int i=0; i< 3; i++) {
        vec3 s = normalize(vec3(lights[i].Position) - pos.xyz);
        float sDotN = max(dot(s,n), 0.0f);


        // Ambient
        vec3 ambient = lights[i].La * texCol;

        // Diffuse
        vec3 diffuse = Material.Kd * sDotN * texCol;
        //vec3 diffuse = vec3(0.0f);

        // Specular
        vec3 spec = vec3(0.0f);

        if(sDotN > 0.0f) {
            vec3 v = normalize(cameraPos.xyz - pos.xyz);
            //r
            vec3 h = normalize (v + s);
            spec = Material.Ks * pow(max(dot(h,n), 0.0f), Material.Shininess);
        }

        Pf += (ambient + lights[i].L * (diffuse + spec));
    }

    

    return Pf;
}


void main() {
    
    FragColour = vec4(blinnPhong(Position, normalize(Normal)), 1.0);
}
