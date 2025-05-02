
uniform sampler2D diffuse;
uniform vec4 color;
uniform vec4 tintColor;

uniform vec2 lightPosition;
uniform float lightIntensity;
uniform float lightRadius;

varying vec2 texCoordVar;
varying vec2 varPosition;

float attenuate(float dist, float a, float b)
{
    return 1.0 / (1.0 + (a * dist) + (b * dist * dist));
}

void main()
{
   
    vec4 texColor = texture2D(diffuse, texCoordVar);
    vec4 tintedColor = texColor * color * vec4(tintColor.rgb, 1.0);
    tintedColor.a = texColor.a * color.a;

    float dist = distance(lightPosition, varPosition);
    
    float brightness = attenuate(dist, lightIntensity, 0.1);
    
    if (dist > lightRadius) {
        brightness = 0.2;
    }

    gl_FragColor = vec4(tintedColor.rgb * brightness, tintedColor.a);
}
