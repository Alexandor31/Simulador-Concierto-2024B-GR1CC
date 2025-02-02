#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;  

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Material material;
uniform Light lights[5];
// Agregar uniformes para el tiempo (esto será pasado desde el código principal)
uniform float time;

void main()
{    
    vec3 result = texture(texture_diffuse1, TexCoords).rgb;

    // Loop sobre las luces
    for (int i = 0; i < 5; ++i) {
        // Crear un color de luz basado en una función seno que cambie con el tiempo para cada luz
        float red = 0.5 * sin(time + float(i)) + 0.5;    // Oscilar entre 0 y 1
        float green = 0.5 * sin(time + float(i) + 2.0) + 0.5;  // Desfase de 2.0 para obtener un color diferente
        float blue = 0.5 * sin(time + float(i) + 4.0) + 0.5;   // Desfase de 4.0 para obtener otro color
        vec3 rainbowColor = vec3(red, green, blue);

        // Luz ambiental para la luz que sigue la cámara
        vec3 ambientCam = lights[i].ambient * 2.0 * rainbowColor * texture(material.diffuse, TexCoords).rgb;

        // Calculando la luz difusa para la luz que sigue la cámara
        vec3 norm = normalize(Normal);
        vec3 lightDirCam = normalize(lights[i].position - FragPos);
        float diffCam = max(dot(norm, lightDirCam), 0.0);
        vec3 diffuseCam = lights[i].diffuse * diffCam * 2.0 * rainbowColor * texture(material.diffuse, TexCoords).rgb;  

        // Luz especular para la luz que sigue la cámara
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDirCam = reflect(-lightDirCam, norm);  
        float specCam = pow(max(dot(viewDir, reflectDirCam), 0.0), material.shininess* 2.0);
        vec3 specularCam = lights[i].specular * specCam * 2.0 * rainbowColor * texture(material.specular, TexCoords).rgb;  

        // Spotlight (con bordes suaves) para la luz que sigue la cámara
        float theta = dot(lightDirCam, normalize(-lights[i].direction)); 
        float epsilon = (lights[i].cutOff - lights[i].outerCutOff);
        float intensityCam = clamp((theta - lights[i].outerCutOff) / epsilon, 0.0, 1.0);
        diffuseCam *= intensityCam;
        specularCam *= intensityCam;

        // Atenuación de la luz que sigue a la cámara
        float distanceCam = length(lights[i].position - FragPos);
        float attenuationCam = 1.0 / (lights[i].constant + lights[i].linear * distanceCam + lights[i].quadratic * (distanceCam * distanceCam));    
        attenuationCam *= 2.0;   
        ambientCam *= attenuationCam; 
        diffuseCam *= attenuationCam;
        specularCam *= attenuationCam;   

        // Sumar los resultados de todas las luces para la luz dinámica
        result += ambientCam + diffuseCam + specularCam;
    }
    FragColor = mix(texture(texture_diffuse1, TexCoords)* 1.5, vec4(result, 1.0),4.0);
}
