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
};

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shineness;
};

in vec4 fragmentPosition;
in vec4 fragmentNormal;
in vec2 fragmentTextureCoords;

out vec4 finalColor;

uniform LightDescriptor lights[32];
uniform int lightQuantity;
uniform Material material;
uniform vec4 cameraPosition;

vec3 getPointLightContribution(LightDescriptor pointLight);
vec3 getSpotLightContribution(LightDescriptor pointLight);
vec3 getDirectionalLightContribution(LightDescriptor pointLight);

void main()
{
	vec3 resultColor = vec3(0.0, 0.0, 0.0);
	int i;

	for (i=0; i<lightQuantity; ++i)
		switch(lights[i].type)
		{
			case LT_POINTLIGHT:
				resultColor += getPointLightContribution(lights[i]);
				break;
			case LT_SPOTLIGHT:
				resultColor += getSpotLightContribution(lights[i]);
				break;
			case LT_DIRECTIONALLIGHT:
				resultColor += getDirectionalLightContribution(lights[i]);
				break;
		}

	finalColor = vec4(resultColor, 1.0);
}

vec3 getPointLightContribution(LightDescriptor pointLight)
{
	// Ambient Color
	vec4 pointAmbientColor = pointLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(pointLight.position - fragmentPosition);
	float pointDiffuseContribution = max(0, dot(fragmentToPointLightVec, fragmentNormal));
	vec4 pointDiffuseColor = pointDiffuseContribution * pointLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float pointSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToPointLightVec, fragmentNormal)), 0.0), material.shineness);
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

vec3 getSpotLightContribution(LightDescriptor spotLight)
{
	vec4 fragmentToSpotLightVec = normalize(spotLight.position - fragmentPosition);
	float spotAngleCos = dot(-fragmentToSpotLightVec, normalize(spotLight.direction));
	float spotIntensity = clamp((spotAngleCos - spotLight.outerCutOffAngleCos) /
		(spotLight.innerCutOffAngleCos - spotLight.outerCutOffAngleCos), 0.0, 1.0);

	// Ambient Color
	vec4 spotAmbientColor = spotLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(spotLight.position - fragmentPosition);
	float spotDiffuseContribution = max(0, dot(fragmentToSpotLightVec, fragmentNormal));
	vec4 spotDiffuseColor = spotDiffuseContribution * spotLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float spotSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToSpotLightVec, fragmentNormal)), 0.0), material.shineness);
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

vec3 getDirectionalLightContribution(LightDescriptor directionalLight)
{
	vec4 normalizedDirection = normalize(directionalLight.direction);

	// Ambient Color
	vec4 directionalAmbientColor = directionalLight.ambientColor * texture(material.diffuseMap, fragmentTextureCoords);

	// Diffuse Color
	float directionalDiffuseContribution = max(0, dot(-normalizedDirection, fragmentNormal));
	vec4 directionalDiffuseColor = directionalDiffuseContribution * directionalLight.diffuseColor * texture(material.diffuseMap, fragmentTextureCoords);
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float directionalSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(normalizedDirection, fragmentNormal)), 0.0), material.shineness);
	vec4 directionalSpecularColor = directionalSpecularContribution * directionalLight.specularColor * texture(material.specularMap, fragmentTextureCoords);

	vec4 directionalColor = directionalAmbientColor + directionalDiffuseColor + directionalSpecularColor;
	return directionalColor.xyz;
}