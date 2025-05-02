

uniform sampler2D diffuse;
uniform vec4 color;
uniform vec4 tintColor;

varying vec2 texCoordVar;

void main()
{
    
    vec4 texColor = texture2D(diffuse, texCoordVar);
    
    vec4 tintedColor = texColor * color * vec4(tintColor.rgb, 1.0);

    tintedColor.a = texColor.a * color.a;
    
    gl_FragColor = tintedColor;
}
