{"fragmentSource":"#version 100\r\n\r\nprecision mediump float;\r\n\r\nstruct Material\r\n{\r\n    sampler2D diffuseMap;\r\n    sampler2D specularMap;\r\n    float shininess;\r\n};\r\n\r\nstruct DirectionLight\r\n{\r\n    vec3 direction;\r\n\r\n    vec3 ambient;\r\n    vec3 diffuse;\r\n    vec3 specular;\r\n};\r\n\r\nstruct PointLight\r\n{\r\n    vec3 position;\r\n    \r\n    vec3 ambient;\r\n    vec3 diffuse;\r\n    vec3 specular;\r\n\r\n    float constant;\r\n    float linear;\r\n    float quadratic;\r\n};\r\n\r\nstruct Spotlight\r\n{\r\n    vec3 position;\r\n    vec3 direction;\r\n    float cutOff;\r\n    float outterCutOff;\r\n\r\n    vec3 ambient;\r\n    vec3 diffuse;\r\n    vec3 specular;\r\n};\r\n\r\n#define MAX_DIRECTIONAL_LIGHTS 16\r\n#define MAX_POINT_LIGHTS 32\r\n#define MAX_SPOTLIGHTS 32\r\n\r\nvarying vec3 fragmentPosition;\r\nvarying vec3 normal;\r\nvarying vec2 textureCoordinate;\r\n\r\nuniform float directionalLightsCount;\r\nuniform float pointLightsCount;\r\nuniform float spotlightsCount;\r\n\r\nuniform Material material;\r\nuniform DirectionLight directionalLights[MAX_DIRECTIONAL_LIGHTS];\r\nuniform PointLight pointLights[MAX_POINT_LIGHTS];\r\nuniform Spotlight spotlights[MAX_SPOTLIGHTS];\r\n\r\nuniform vec3 viewPosition;\r\n\r\nvec3 processDirectionLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);\r\nvec3 processPointLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);\r\nvec3 processSpotlight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);\r\nfloat mininmum(float a, float b);\r\n\r\nvoid main()\r\n{\r\n    vec3 normalizedNormal = normalize(normal);\r\n    vec3 diffuseColor = texture2D(material.diffuseMap, textureCoordinate).rgb;\r\n    vec3 specularColor = texture2D(material.specularMap, textureCoordinate).rgb;\r\n    vec3 viewDirection = normalize(viewPosition - fragmentPosition);\r\n\r\n    vec3 outputColor = vec3(0.0);\r\n\r\n    // directional lights\r\n    const float maxDirectionalLightsCount = mininmum(directionalLightsCount, MAX_DIRECTIONAL_LIGHTS);\r\n    for (float i = 0.; i < maxDirectionalLightsCount; i++)\r\n    {\r\n        outputColor += processDirectionLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);\r\n    }\r\n\r\n    // point lights\r\n    const float maxPointLightsCount = mininmum(pointLightsCount, MAX_POINT_LIGHTS);\r\n    for (float i = 0.; i < maxPointLightsCount; i++)\r\n    {\r\n        outputColor += processPointLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);\r\n    }\r\n\r\n    // spotlights\r\n    const float maxSpotlightsCount = mininmum(spotlightsCount, MAX_POINT_LIGHTS);\r\n    for (float i = 0.; i < maxSpotlightsCount; i++)\r\n    {\r\n        outputColor += processSpotlight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);\r\n    }\r\n    \r\n    gl_FragColor = vec4(outputColor, 0.5);\r\n}\r\n\r\nvec3 processDirectionLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)\r\n{\r\n    vec3 lightDirection = normalize(-directionalLights[int(index)].direction);\r\n\r\n    // ambient\r\n    vec3 ambient = directionalLights[int(index)].ambient * diffuseColor;\r\n\r\n    // diffuse\r\n    float diffuseScalar = max(dot(normal, lightDirection), 0.0);\r\n    vec3 diffuse = directionalLights[int(index)].diffuse * diffuseScalar * diffuseColor;\r\n\r\n    // specular\r\n    vec3 reflectDirection = reflect(-lightDirection, normal);\r\n    float specularScalar = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\r\n    vec3 specular = directionalLights[int(index)].specular * specularScalar * specularColor;\r\n\r\n    return ambient + diffuse + specular;\r\n}\r\n\r\nvec3 processPointLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)\r\n{\r\n    PointLight light = pointLights[int(index)];\r\n\r\n\r\n    vec3 lightDirection = normalize(light.position - fragmentPosition);\r\n\r\n    // point light attenuation factor\r\n    float distance = length(light.position - fragmentPosition);\r\n    float attenuation = 1.0 / (light.constant + light.linear * distance +\r\n                                light.quadratic * (distance * distance));\r\n\r\n    // ambient\r\n    vec3 ambient = light.ambient * diffuseColor * attenuation;\r\n\r\n    // diffuse\r\n    float diffuseScalar = max(dot(normal, lightDirection), 0.0);\r\n    vec3 diffuse = light.diffuse * diffuseScalar * diffuseColor * attenuation;\r\n\r\n    // specular\r\n    vec3 reflectDirection = reflect(-lightDirection, normal);\r\n    float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\r\n    vec3 specular = light.specular * specularValue * specularColor * attenuation;\r\n\r\n    return ambient + diffuse + specular;\r\n}\r\n\r\nvec3 processSpotlight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)\r\n{\r\n\r\n    Spotlight light = spotlights[int(index)];\r\n\r\n    vec3 lightDirection = normalize(light.position - fragmentPosition);\r\n    float theta = dot(lightDirection, normalize(-light.direction));\r\n    float outterDelta = max(light.cutOff - light.outterCutOff, 0.0001);\r\n    float intensity = clamp((theta - light.outterCutOff) / outterDelta, 0.0, 1.0);\r\n\r\n    // ambient\r\n    vec3 ambient = light.ambient * diffuseColor;\r\n\r\n    if (theta > light.outterCutOff)\r\n    {\r\n        // diffuse\r\n        float diffuseScalar = max(dot(normal, lightDirection), 0.0);\r\n        vec3 diffuse = light.diffuse * diffuseScalar * diffuseColor * intensity;\r\n\r\n        // specular\r\n        vec3 reflectDirection = reflect(-lightDirection, normal);\r\n        float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\r\n        vec3 specular = light.specular * specularValue * specularColor * intensity;\r\n\r\n        return ambient + diffuse + specular;\r\n    }\r\n    else\r\n    {\r\n        return ambient;\r\n    }\r\n}\r\n\r\nfloat mininmum(float a, float b)\r\n{\r\n    return a < b ? a : b;\r\n}","name":"webgl_baseLit","vertexSource":"#version 100\r\n\r\nprecision mediump float;\r\n\r\nattribute vec3 aPosition;\r\nattribute vec3 aNormal;\r\nattribute vec2 aTextureCoordinate;\r\n\r\nuniform mat4 model;\r\nuniform mat3 normalMatrix;\r\nuniform mat4 view;\r\nuniform mat4 projection;\r\n\r\nvarying vec3 fragmentPosition;\r\nvarying vec3 normal;\r\nvarying vec2 textureCoordinate;\r\n\r\nvoid main()\r\n{\r\n    gl_Position = projection * view * model * vec4(aPosition, 1.0);\r\n    fragmentPosition = vec3(model * vec4(aPosition, 1.0));\r\n    normal = normalMatrix * aNormal;\r\n    textureCoordinate = aTextureCoordinate;\r\n}\r\n\r\n"}