#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//FragColor = texture(texture1, TexCoord);
	//FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);//混合纹理颜色和顶点颜色
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);//多个纹理结合, 第三个参数位第二个输入颜色所占的比例
}