#version 330


uniform sampler2D tex;

out vec4 pixelColor; 

//Zmienne interpolowane
in float d;
in float sun;
in vec2 i_tc;

void main(void) {
	
    vec4 color=texture(tex,i_tc);
	
	if (d>=sun)
	{
	pixelColor=vec4(color.rgb*d,color.a);
	}
	else{
	pixelColor=vec4(color.rgb*sun,color.a);
	}
	
}
