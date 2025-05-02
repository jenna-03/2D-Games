
uniform vec4 color;
uniform vec4 tintColor;  

out vec4 fragColor;

void main() {
    // tint
    vec4 tintedColor = color * vec4(tintColor.rgb, 1.0);
    
    tintedColor.a = color.a;
    
    fragColor = tintedColor;
}
