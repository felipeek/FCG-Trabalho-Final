#version 330 core

#define LT_POINTLIGHT 0
#define LT_SPOTLIGHT 1
#define LT_DIRECTIONALLIGHT 2

struct LightDescriptor
{
	/* Generic Lights Attributes */
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
	float constantTerm;				// Attenuation
	float linearTerm;				// Attenuation
	float quadraticTerm;			// Attenuation
	int type;

	/* Specific Light Attributes */
	vec4 position;					// PointLight and SpotLight
	vec4 direction;					// SpotLight and DirectionLight
	float innerCutOffAngleCos;		// SpotLight
	float outerCutOffAngleCos;		// SpotLight
	
	bool isOn;
};

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D normalMap;
	bool useNormalMap;
	float shineness;
};

struct FogDescriptor
{
	float density;
	float gradient;
	vec4 skyColor;
	bool on;
};

in vec4 fragmentPosition;
in vec4 fragmentNormal;
in vec2 fragmentTextureCoords;
in mat4 tangentMatrix;
in float fragmentVisibility;

out vec4 finalColor;

uniform LightDescriptor lights[32];
uniform int lightQuantity;
uniform Material material;
uniform vec4 cameraPosition;
uniform FogDescriptor fogDescriptor;

vec4 getCorrectNormal();
vec3 getPointLightContribution(LightDescriptor pointLight, vec4 normal);
vec3 getSpotLightContribution(LightDescriptor pointLight, vec4 normal);
vec3 getDirectionalLightContribution(LightDescriptor pointLight, vec4 normal);

void main()
{
	vec3 resultColor = vec3(0.0, 0.0, 0.0);
	vec4 normal = getCorrectNormal();
	int i;

	for (i=0; i<lightQuantity; ++i)
		if (lights[i].isOn)
			switch(lights[i].type)
			{
				case LT_POINTLIGHT:
					resultColor += getPointLightContribution(lights[i], normal);
					break;
				case LT_SPOTLIGHT:
					resultColor += getSpotLightContribution(lights[i], normal);
					break;
				case LT_DIRECTIONALLIGHT:
					resultColor += getDirectionalLightContribution(lights[i], normal);
					break;
			}

	finalColor = vec4(resultColor, 1.0);
	
	if (fogDescriptor.on)
		finalColor = mix(fogDescriptor.skyColor, finalColor, fragmentVisibility);
}

vec4 getCorrectNormal()
{
	vec4 normal;

	// Check if normal map is being used. In case positive, the normal must be obtained from the normal map.
	// If normal map is not being used, we use the fragment normal.
	if (material.useNormalMap)
	{
		// Sample normal map (range [0, 1])
		normal = texture(material.normalMap, fragmentTextureCoords);
		// Transform normal vector to range [-1, 1]
		normal = normal * 2.0 - 1.0;
		// W coordinate must be 0
		normal.w = 0;
		// Normalize normal
		normal = normalize(normal);
		// Transform normal from tangent space to world space.
		normal = normalize(tangentMatrix * normal);
	}
	else
		normal = fragmentNormal;

	return normal;
}

vec3 getPointLightContribution(LightDescriptor pointLight, vec4 normal)
{
	// Ambient Color
	vec4 pointAmbientColor = pointLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(pointLight.position - fragmentPosition);
	float pointDiffuseContribution = max(0, dot(fragmentToPointLightVec, normal));
	vec4 pointDiffuseColor = pointDiffuseContribution * pointLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float pointSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToPointLightVec, normal)), 0.0), material.shineness);
	vec4 pointSpecularColor = pointSpecularContribution * pointLight.specularColor * texture(material.specularMap, fragmentTextureCoords);

	// Attenuation
	float pointLightDistance = length(pointLight.position - fragmentPosition);
	float pointAttenuation = 1.0 / (pointLight.constantTerm + pointLight.linearTerm * pointLightDistance +
		pointLight.quadraticTerm * pointLightDistance * pointLightDistance);

	pointAmbientColor *= pointAttenuation;
	pointDiffuseColor *= pointAttenuation;
	pointSpecularColor *= pointAttenuation;

	vec4 pointColor = pointAmbientColor + pointDiffuseColor + pointSpecularColor;
	return pointColor.xyz;
}

vec3 getSpotLightContribution(LightDescriptor spotLight, vec4 normal)
{
	vec4 fragmentToSpotLightVec = normalize(spotLight.position - fragmentPosition);
	float spotAngleCos = dot(-fragmentToSpotLightVec, normalize(spotLight.direction));
	float spotIntensity = 10.0 * clamp((spotAngleCos - spotLight.outerCutOffAngleCos) /
		(spotLight.innerCutOffAngleCos - spotLight.outerCutOffAngleCos), 0.0, 1.0);

	// Ambient Color
	vec4 spotAmbientColor = spotLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(spotLight.position - fragmentPosition);
	float spotDiffuseContribution = max(0, dot(fragmentToSpotLightVec, normal));
	vec4 spotDiffuseColor = spotDiffuseContribution * spotLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float spotSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToSpotLightVec, normal)), 0.0), material.shineness);
	vec4 spotSpecularColor = spotSpecularContribution * spotLight.specularColor * texture(material.specularMap, fragmentTextureCoords);

	// Attenuation
	float spotLightDistance = length(spotLight.position - fragmentPosition);
	float spotAttenuation = 1.0 / (spotLight.constantTerm + spotLight.linearTerm * spotLightDistance +
		spotLight.quadraticTerm * spotLightDistance * spotLightDistance);

	spotAmbientColor *= spotAttenuation * spotIntensity;
	spotDiffuseColor *= spotAttenuation * spotIntensity;
	spotSpecularColor *= spotAttenuation * spotIntensity;

	vec4 spotColor = spotAmbientColor + spotDiffuseColor + spotSpecularColor;
	return spotColor.xyz;
}

vec3 getDirectionalLightContribution(LightDescriptor directionalLight, vec4 normal)
{
	vec4 normalizedDirection = normalize(directionalLight.direction);

	// Ambient Color
	vec4 directionalAmbientColor = directionalLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	float directionalDiffuseContribution = max(0, dot(-normalizedDirection, normal));
	vec4 directionalDiffuseColor = directionalDiffuseContribution * directionalLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float directionalSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(normalizedDirection, normal)), 0.0), material.shineness);
	vec4 directionalSpecularColor = directionalSpecularContribution * directionalLight.specularColor * texture(material.specularMap, fragmentTextureCoords);

	vec4 directionalColor = directionalAmbientColor + directionalDiffuseColor + directionalSpecularColor;
	return directionalColor.xyz;
}