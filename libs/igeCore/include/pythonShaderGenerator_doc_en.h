
//shaderGeneragtor
PyDoc_STRVAR(shaderGeneragtor_doc,
	"shaderGenerator is abstracted to be compatible\n"\
	"with multiple platforms by replacing \n"\
	"shader code with simple procedure\n"\
	"You can see the current shader code with print (shaderGeneragtor)");

//setBoneCondition
PyDoc_STRVAR(setBoneCondition_doc,
	"Specifys the number of  bone palette and number of bone influencen\n"\
	"\n"\
	"shaderGenerater.setBoneCondition(numBoneInfluence, numBonePalette)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    numBoneInfluence : int\n"\
	"        Number of bones that one vertex refers to simultaneously\n"\
	"    numBonePalette : int\n"\
	"        Number of bones to put in the bones palette\n");


//setSpecular
PyDoc_STRVAR(setSpecular_doc,
	"Enable or disable specular reflection\n"\
	"\n"\
	"shaderGenerater.setSpecular(enableSpecular, textureMapChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableSpecular : bool\n"\
	"        Enable or disable specular reflection\n"\
	"    textureMapChannel : int\n"\
	"        Specify Specular strength map\n"\
	"        The following values can be selected\n"\
	"        igeCore.MAPCHANNEL_NONE\n"\
	"        igeCore.MAPCHANNEL_COLOR_RED\n"\
	"        igeCore.MAPCHANNEL_COLOR_ALPHA\n"\
	"        igeCore.MAPCHANNEL_NORMAL_RED\n"\
	"        igeCore.MAPCHANNEL_NORMAL_ALPHA\n"\
	"        igeCore.MAPCHANNEL_VERTEX_COLOR_RED\n"\
	"        igeCore.MAPCHANNEL_VERTEX_COLOR_ALPHA");


//setAmbientOcclusion
PyDoc_STRVAR(setAmbientOcclusion_doc,
	"Enable or disable ambient occlusion mapping.\n"\
	"\n"\
	"shaderGenerater.setAmbientOcclusion(textureMapChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    textureMapChannel : int\n");


//setColorTextureUVSet
PyDoc_STRVAR(setColorTextureUVSet_doc,
	"Specifies the UV channel of the color map texture\n"\
	"RGB and A can be specified separately\n"\
	"Supports 3 channels of UV0, UV1and UV2\n"\
	"\n"\
	"shaderGenerater.setColorTextureUVSet(rgbChannel, aChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    rgbChannel : int\n"\
	"        uv channel no (0, 1, 2)\n"\
	"    aChannel : int\n"\
	"        uv channel no (0, 1, 2)\n");



//setNormalTextureUVSet
PyDoc_STRVAR(setNormalTextureUVSet_doc,
	"Specifies the UV channel of the normal map texture\n"\
	"RGBand A can be specified separately\n"\
	"Supports 3 channels of UV0, UV1and UV2\n"\
	"\n"\
	"shaderGenerater.setNormalTextureUVSet(rgbChannel, aChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    rgbChannel : int\n"\
	"        uv channel no (0, 1, 2)\n"\
	"    aChannel : int\n"\
	"        uv channel no (0, 1, 2)\n");

//setLightTextureUVSet
PyDoc_STRVAR(setLightTextureUVSet_doc,

	"Specifies the UV channel of the light map texture\n"\
	"RGBand A can be specified separately\n"\
	"Supports 3 channels of UV0, UV1and UV2\n"\
	"\n"\
	"shaderGenerater.setLightTextureUVSet(rgbChannel, aChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    rgbChannel : int\n"\
	"        uv channel no (0, 1, 2)\n"\
	"    aChannel : int\n"\
	"        uv channel no (0, 1, 2)\n");


//setOverlayColorTextureUVSet
PyDoc_STRVAR(setOverlayColorTextureUVSet_doc,

	"Specifies the UV channel of the overlay color map texture\n"\
	"RGBand A can be specified separately\n"\
	"Supports 3 channels of UV0, UV1and UV2\n"\
	"\n"\
	"shaderGenerater.setOverlayColorTextureUVSet(rgbChannel, aChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    rgbChannel : int\n"\
	"        uv channel no (0, 1, 2)\n"\
	"    aChannel : int\n"\
	"        uv channel no (0, 1, 2)\n");

//setOverlayNormalColorTextureUVSet
PyDoc_STRVAR(setOverlayNormalColorTextureUVSet_doc,

	"Specifies the UV channel of the overlay normal map texture\n"\
	"RGBand A can be specified separately\n"\
	"Supports 3 channels of UV0, UV1and UV2\n"\
	"\n"\
	"shaderGenerater.setOverlayNormalColorTextureUVSet(rgbChannel, aChannel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    rgbChannel : int\n"\
	"        uv channel no (0, 1, 2)\n"\
	"    aChannel : int\n"\
	"        uv channel no (0, 1, 2)\n");

//setCalcBinormalInShader
PyDoc_STRVAR(setCalcBinormalInShader_doc,
	"Whether to calculate Binormal values used in \n"\
	"normal mapping calculations in the shader\n"\
	"\n"\
	"shaderGenerater.setCalcBinormalInShader(calcBinormal)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    calcBinormal : bool\n");



//setNumDirLamp
PyDoc_STRVAR(setNumDirLamp_doc,
	"Specifies the number of directional light sources affected.\n"\
	"You can specify up to 3\n"\
	"\n"\
	"shaderGenerater.setNumDirLamp(numLamp)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    numLamp : int\n"\
	"        Number of directional light (0, 1, 2, 3)\n");

	
//setNumPointLamp
PyDoc_STRVAR(setNumPointLamp_doc,
	"Specifies the number of point light sources affected.\n"\
	"You can specify up to 7\n"\
	"\n"\
	"shaderGenerater.setNumPointLamp(numLamp)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    numLamp : int\n"\
	"        Number of point light (0 - 7)\n");


//setAmbientType
PyDoc_STRVAR(setAmbientType_doc,
	"Specifies the ambient light type\n"\
	"\n"\
	"shaderGenerater.setAmbientType(type)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    type : int \n"\
	"        Ambient light type\n"\
	"        igeCore.AMBIENT_TYPE_NONE\n"\
	"        igeCore.AMBIENT_TYPE_AMBIENT\n"\
	"        igeCore.AMBIENT_TYPE_HEMISPHERE\n");



//setClutLamp
PyDoc_STRVAR(setClutLamp_doc,
	"Enable or disable color look up table lighting\n"\
	"Useful for expressing cartoon lighting\n"\
	"\n"\
	"shaderGenerater.setClutLamp(enableCLUT)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableCLUT : bool\n");

//setVertexColor
PyDoc_STRVAR(setVertexColor_doc,
	"Enable or disable vertex color\n"\
	"\n"\
	"shaderGenerater.setVertexColor(enableVertexColor)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableVertexColor : bool\n");


//setVertexAlpha
PyDoc_STRVAR(setVertexAlpha_doc,
	"Whether to references the vertex color alpha channel as a transparent source\n"\
	"\n"\
	"shaderGenerater.setVertexAlpha(enableVertexAlpha)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableVertexAlpha : bool\n");


//setColorTexture
PyDoc_STRVAR(setColorTexture_doc,
	"Whether to enable color texture\n"\
	"\n"\
	"shaderGenerater.setColorTexture(enableTexture)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableTexture : bool\n");


//setNormalTexture
PyDoc_STRVAR(setNormalTexture_doc,
	"Whether to enable normal texture\n"\
	"\n"\
	"shaderGenerater.setNormalTexture(enableTexture)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableTexture : bool\n");



//setLightTexture
PyDoc_STRVAR(setLightTexture_doc,
	"Whether to enable light texture\n"\
	"\n"\
	"shaderGenerater.setLightTexture(enableTexture)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableTexture : bool\n");


//setOverlayColorTexture
PyDoc_STRVAR(setOverlayColorTexture_doc,
	"Whether to enable overlay color texture\n"\
	"\n"\
	"shaderGenerater.setOverlayColorTexture(enableTexture)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableTexture : bool\n");

//setOverlayNormalTexture
PyDoc_STRVAR(setOverlayNormalTexture_doc,
	"Whether to enable overlay normal texture\n"\
	"\n"\
	"shaderGenerater.setOverlayNormalTexture(enableTexture)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableTexture : bool\n");

	
//setOverlaySpecularTexture
PyDoc_STRVAR(setOverlaySpecularTexture_doc,
	"Whether to enable overlay specular texture\n"\
	"\n"\
	"shaderGenerater.setOverlaySpecularTexture(enableSpecular)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableSpecular : bool\n");

//setOverlayVertexAlpha
PyDoc_STRVAR(setOverlayVertexAlpha_doc,
	"Whether to refer to the alpha channel of the vertex color \n"\
	"when calculating overlay map transparency\n"\
	"\n"\
	"shaderGenerater.setOverlayVertexAlpha(enableVertexAlpha)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enableVertexAlpha : bool");


//discardColorMapRGB
PyDoc_STRVAR(discardColorMapRGB_doc,
	"Discard the RGB channel of the color map. \n"\
	"Used in combination with GL_ALPHA format textures.\n"\
	"\n"\
	"shaderGenerater.discardColorMapRGB(dicardRGB)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    dicardRGB : bool");


//setUVOffset
PyDoc_STRVAR(setUVOffset_doc,
	"Enable / Disable UV offset shader. \n"\
	"\n"\
	"shaderGenerater.setUVOffset(channel, enable)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    channel : int\n"\
	"		UV channel no (0 - 2)\n"\
	"    enable : bool");

//setUVScroll
PyDoc_STRVAR(setUVScroll_doc,
	"Enable / Disable UV scroll shader. \n"\
	"\n"\
	"shaderGenerater.setUVScroll(channel, enable)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    channel : int\n"\
	"		UV channel no (0 - 2)\n"\
	"    enable : bool");

//setProjectionMapping
PyDoc_STRVAR(setProjectionMapping_doc,
	"Enable / Disable projection mapping shader. \n"\
	"\n"\
	"shaderGenerater.setProjectionMapping(channel, enable)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    channel : int\n"\
	"		UV channel no (0 - 2)\n"\
	"    enable : bool");

//setDistortion
PyDoc_STRVAR(setDistortion_doc,
	"Enable / Disable distortion shader. \n"\
	"\n"\
	"shaderGenerater.setDistortion(enable)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    enable : bool");


//setAlphaMap
PyDoc_STRVAR(setAlphaMap_doc,
	"set diffuce alpha map channel. \n"\
	"\n"\
	"shaderGenerater.setAlphaMap(channel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    channel : int\n"\
	"		igeCore.DIFFUSE_ALPHA\n"\
	"		igeCore.DIFFUSE_RED\n"\
	"		igeCore.NORMAL_ALPHA\n"\
	"		igeCore.NORMAL_RED\n"\
	"		igeCore.LIGHT_ALPHA\n"\
	"		igeCore.LIGHT_RED\n"\
	"		igeCore.OVERLAY_DIFFUSE_ALPHA\n"\
	"		igeCore.OVERLAY_DIFFUSE_RED\n"\
	"		igeCore.OVERLAY_NORMAL_ALPHA\n"\
	"		igeCore.OVERLAY_NORMAL_RED\n");

//setOverlayAlphaMap
PyDoc_STRVAR(setOverlayAlphaMap_doc,
	"set overay diffuce alpha map channel. \n"\
	"\n"\
	"shaderGenerater.setOverlayAlphaMap(channel)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    channel : int\n"\
	"		igeCore.DIFFUSE_ALPHA\n"\
	"		igeCore.DIFFUSE_RED\n"\
	"		igeCore.NORMAL_ALPHA\n"\
	"		igeCore.NORMAL_RED\n"\
	"		igeCore.LIGHT_ALPHA\n"\
	"		igeCore.LIGHT_RED\n"\
	"		igeCore.OVERLAY_DIFFUSE_ALPHA\n"\
	"		igeCore.OVERLAY_DIFFUSE_RED\n"\
	"		igeCore.OVERLAY_NORMAL_ALPHA\n"\
	"		igeCore.OVERLAY_NORMAL_RED\n");

//setShadow
PyDoc_STRVAR(setShadow_doc,
	"Enable / disable shadow rendering\n"\
	"\n"\
	"shaderGenerater.setReceiveShadow(make, receive, depth)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"	make : bool\n"\
	"		When enabled, it produces shadows.\n"\
	"	receive : bool\n"\
	"		When enabled, it receive shadows.\n"\
	"	depth : bool\n"\
	"		When enabled, the self - shadowing algorithm with depth buffer is selected.\n"\
	"		depth must be enabled for make and receive to be active at the same time.\n"\
	"		If depth is disabled, you get a simpler cast shadow instead of a Depth shadow, \n"\
	"		Then you cannot enable make and receive at the same time.\n"\
	"		Cast shadows have the advantages of light processing and clear shadow drawing.\n"\
	"		Depth shadows and cast shadows cannot be used at the same time in the same scene.\n"\
	"		You need to unify to either one.\n");



//globalMipBias
PyDoc_STRVAR(globalMipBias_doc,
	"Texture mip bias. default value is -1.5\n"\
	"\n"\
	"    type : float	\n"\
	"    read / write");

//globalShadowBias
PyDoc_STRVAR(globalShadowBias_doc,
	"Depth Shadow coefficient. default value is 0.005\n"\
	"\n"\
	"    type : float	\n"\
	"    read / write");
