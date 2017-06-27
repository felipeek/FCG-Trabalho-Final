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

struct Color
{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
};

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shineness;
};

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoords;

out vec4 ambientColor;
out vec4 diffuseColor;
out vec4 specularColor;

out vec2 fragmentTextureCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform LightDescriptor lights[32];
uniform Material material;
uniform int lightQuantity;
uniform vec4 cameraPosition;

Color getFragmentColor(vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition);
Color getPointLightContribution(LightDescriptor pointLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition);
Color getSpotLightContribution(LightDescriptor pointLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition);
Color getDirectionalLightContribution(LightDescriptor pointLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition);

void main()
{
	vec3 normal3D = mat3(inverse(transpose(modelMatrix))) * vertexNormal.xyz;
	vec4 fragmentNormal = normalize(vec4(normal3D, 0.0));
	vec4 fragmentPosition = modelMatrix * vertexPosition;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
	
	fragmentTextureCoords = vertexTextureCoords;
	Color fragmentColor = getFragmentColor(fragmentNormal, fragmentTextureCoords, fragmentPosition);
	ambientColor = fragmentColor.ambientColor;
	diffuseColor = fragmentColor.diffuseColor;
	specularColor = fragmentColor.specularColor;
}

Color getFragmentColor(vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition)
{
	Color c;
	Color resultColor = {vec4(0), vec4(0), vec4(0)};
	int i;

	for (i=0; i<lightQuantity; ++i)
		switch(lights[i].type)
		{
			case LT_POINTLIGHT:
				c = getPointLightContribution(lights[i], fragmentNormal, fragmentTextureCoords, fragmentPosition);
				resultColor.ambientColor += c.ambientColor;
				resultColor.diffuseColor += c.diffuseColor;
				resultColor.specularColor += c.specularColor;
				break;
			case LT_SPOTLIGHT:
				c = getSpotLightContribution(lights[i], fragmentNormal, fragmentTextureCoords, fragmentPosition);
				resultColor.ambientColor += c.ambientColor;
				resultColor.diffuseColor += c.diffuseColor;
				resultColor.specularColor += c.specularColor;
				break;
			case LT_DIRECTIONALLIGHT:
				c = getDirectionalLightContribution(lights[i], fragmentNormal, fragmentTextureCoords, fragmentPosition);
				resultColor.ambientColor += c.ambientColor;
				resultColor.diffuseColor += c.diffuseColor;
				resultColor.specularColor += c.specularColor;
				break;
		}

	fragmentTextureCoords = vertexTextureCoords;
		
	resultColor.ambientColor.w = 1.0;
	resultColor.diffuseColor.w = 1.0;
	resultColor.specularColor.w = 1.0;
	
	return resultColor;
}

Color getPointLightContribution(LightDescriptor pointLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition)
{
	// Ambient Color
	vec4 pointAmbientColor = pointLight.ambientColor;

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(pointLight.position - fragmentPosition);
	float pointDiffuseContribution = max(0, dot(fragmentToPointLightVec, fragmentNormal));
	vec4 pointDiffuseColor = pointDiffuseContribution * pointLight.diffuseColor;
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float pointSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToPointLightVec, fragmentNormal)), 0.0), material.shineness);
	vec4 pointSpecularColor = pointSpecularContribution * pointLight.specularColor;

	// Attenuation
	float pointLightDistance = length(pointLight.position - fragmentPosition);
	float pointAttenuation = 1.0 / (pointLight.constantTerm + pointLight.linearTerm * pointLightDistance +
		pointLight.quadraticTerm * pointLightDistance * pointLightDistance);

	pointAmbientColor *= pointAttenuation;
	pointDiffuseColor *= pointAttenuation;
	pointSpecularColor *= pointAttenuation;

	Color pointColor;
	pointColor.ambientColor = pointAmbientColor;
	pointColor.diffuseColor = pointDiffuseColor;
	pointColor.specularColor = pointSpecularColor;
	
	return pointColor;
}

Color getSpotLightContribution(LightDescriptor spotLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition)
{
	vec4 fragmentToSpotLightVec = normalize(spotLight.position - fragmentPosition);
	float spotAngleCos = dot(-fragmentToSpotLightVec, normalize(spotLight.direction));
	float spotIntensity = clamp((spotAngleCos - spotLight.outerCutOffAngleCos) /
		(spotLight.innerCutOffAngleCos - spotLight.outerCutOffAngleCos), 0.0, 1.0);

	// Ambient Color
	vec4 spotAmbientColor = spotLight.ambientColor;

	// Diffuse Color
	vec4 fragmentToPointLightVec = normalize(spotLight.position - fragmentPosition);
	float spotDiffuseContribution = max(0, dot(fragmentToSpotLightVec, fragmentNormal));
	vec4 spotDiffuseColor = spotDiffuseContribution * spotLight.diffuseColor;
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float spotSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(-fragmentToSpotLightVec, fragmentNormal)), 0.0), material.shineness);
	vec4 spotSpecularColor = spotSpecularContribution * spotLight.specularColor;

	// Attenuation
	float spotLightDistance = length(spotLight.position - fragmentPosition);
	float spotAttenuation = 1.0 / (spotLight.constantTerm + spotLight.linearTerm * spotLightDistance +
		spotLight.quadraticTerm * spotLightDistance * spotLightDistance);

	spotAmbientColor *= spotAttenuation * spotIntensity;
	spotDiffuseColor *= spotAttenuation * spotIntensity;
	spotSpecularColor *= spotAttenuation * spotIntensity;

	Color spotColor;
	spotColor.ambientColor = spotAmbientColor;
	spotColor.diffuseColor = spotDiffuseColor;
	spotColor.specularColor = spotSpecularColor;
	
	return spotColor;
}

Color getDirectionalLightContribution(LightDescriptor directionalLight, vec4 fragmentNormal, vec2 fragmentTextureCoords, vec4 fragmentPosition)
{
	vec4 normalizedDirection = normalize(directionalLight.direction);

	// Ambient Color
	vec4 directionalAmbientColor = directionalLight.ambientColor;

	// Diffuse Color
	float directionalDiffuseContribution = max(0, dot(-normalizedDirection, fragmentNormal));
	vec4 directionalDiffuseColor = directionalDiffuseContribution * directionalLight.diffuseColor;
	
	// Specular Color
	vec4 fragmentToCameraVec = normalize(cameraPosition - fragmentPosition);
	float directionalSpecularContribution = pow(max(dot(fragmentToCameraVec, reflect(normalizedDirection, fragmentNormal)), 0.0), material.shineness);
	vec4 directionalSpecularColor = directionalSpecularContribution * directionalLight.specularColor;

	Color directionalColor;
	directionalColor.ambientColor = directionalAmbientColor;
	directionalColor.diffuseColor = directionalDiffuseColor;
	directionalColor.specularColor = directionalSpecularColor;
	
	return directionalColor;
}