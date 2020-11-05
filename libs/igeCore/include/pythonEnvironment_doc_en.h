//environment
PyDoc_STRVAR(environment_doc,
	"Rendering environment such as light source and fog"
);

PyDoc_STRVAR(getDirectionalLampIntensity_doc,
	"Get intensity value of directional lamp\n"\
	"\n"\
	"intensity = environment.getDirectionalLampIntensity(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)"
	"Returns\n"\
	"-------\n"\
	"    intensity : float\n"\
	"        intensity value of directional lamp"
);

PyDoc_STRVAR(setDirectionalLampIntensity_doc,
	"Set intensity value of directional lamp\n"\
	"\n"\
	"environment.getDirectionalLampIntensity(index, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)"
	"    value : float\n"\
	"        intensity value of directional lamp\n"\
);

PyDoc_STRVAR(getDirectionalLampColor_doc,
	"Get directional lamp color\n"\
	"\n"\
	"color = environment.getDirectionalLampColor(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"Returns\n"\
	"-------\n"\
	"    color : igeVmath.vec3\n"\
	"       color value of directional lamp\n"\
);

PyDoc_STRVAR(setDirectionalLampColor_doc,
	"Set directional lamp color\n"\
	"\n"\
	"environment.setDirectionalLampColor(index, color)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"    value : igeVmath.vec3\n"\
	"        lamp color"
);

PyDoc_STRVAR(getDirectionalLampDirection_doc,
	"Get directional lamp direction\n"\
	"\n"\
	"dir = environment.getDirectionalLampDirection(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"Returns\n"\
	"-------\n"\
	"    dir : igeVmath.vec3\n"\
	"       lamp direction vector\n"\
);

PyDoc_STRVAR(setDirectionalLampDirection_doc,
	"Set directional lamp direction\n"\
	"\n"\
	"environment.setDirectionalLampDirection(index, dir)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"    dir : igeVmath.vec3\n"\
	"       lamp direction vector\n"\
);

PyDoc_STRVAR(getPointLampRange_doc,
	"Get point lamp range value\n"\
	"\n"\
	"range = environment.getPointLampRange(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"Returns\n"\
	"-------\n"\
	"    range : float\n"\
	"        range distance value of point lamp"
);

PyDoc_STRVAR(setPointLampRange_doc,
	"Set point lamp range value\n"\
	"\n"\
	"environment.setPointLampRange(index, range)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"    range : float\n"\
	"        range distance value of point lamp"
);

PyDoc_STRVAR(getPointLampIntensity_doc,
	"Get intensity value of point lamp\n"\
	"\n"\
	"intensity = environment.getPointLampIntensity(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"Returns\n"\
	"-------\n"\
	"    intensity : float\n"\
	"        intensity value of point lamp"
);

PyDoc_STRVAR(setPointLampIntensity_doc,
	"Set intensity value of point lamp\n"\
	"\n"\
	"environment.getPointLampIntensity(index, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"    value : float\n"\
	"        intensity value of point lamp\n"\
);

PyDoc_STRVAR(getPointLampColor_doc,
	"Get point lamp color\n"\
	"\n"\
	"color = environment.getPointLampColor(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"Returns\n"\
	"-------\n"\
	"    color : igeVmath.vec3\n"\
	"       color value of point lamp\n"\
);

PyDoc_STRVAR(setPointLampColor_doc,
	"Set point lamp color\n"\
	"\n"\
	"environment.setPointLampColor(index, color)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"    value : igeVmath.vec3\n"\
	"        lamp color"
);

PyDoc_STRVAR(getPointLampPosition_doc,
	"Get point lamp position\n"\
	"\n"\
	"pos = environment.getPointLampPosition(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"Returns\n"\
	"-------\n"\
	"    pos : igeVmath.vec3\n"\
	"       lamp position\n"\
);

PyDoc_STRVAR(setPointLampPosition_doc,
	"Set point lamp position\n"\
	"\n"\
	"environment.getPointLampPosition(index, pos)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"    pos : igeVmath.vec3\n"\
	"       lamp position\n"\
);




PyDoc_STRVAR(getSpotLampRange_doc,
	"Get spot lamp range value\n"\
	"\n"\
	"range = environment.getSpotLampRange(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"Returns\n"\
	"-------\n"\
	"    range : float\n"\
	"        range distance value of spot lamp"
);

PyDoc_STRVAR(setSpotLampRange_doc,
	"Set spot lamp range value\n"\
	"\n"\
	"environment.setSpotLampRange(index, range)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"    range : float\n"\
	"        range distance value of spot lamp"
);

PyDoc_STRVAR(getSpotLampIntensity_doc,
	"Get intensity value of spot lamp\n"\
	"\n"\
	"intensity = environment.getSpotLampIntensity(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"Returns\n"\
	"-------\n"\
	"    intensity : float\n"\
	"        intensity value of spot lamp"
);

PyDoc_STRVAR(setSpotLampIntensity_doc,
	"Set intensity value of spot lamp\n"\
	"\n"\
	"environment.getSpotLampIntensity(index, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"    value : float\n"\
	"        intensity value of spot lamp\n"\
);

PyDoc_STRVAR(getSpotLampColor_doc,
	"Get spot lamp color\n"\
	"\n"\
	"color = environment.getSpotLampColor(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"Returns\n"\
	"-------\n"\
	"    color : igeVmath.vec3\n"\
	"       color value of spot lamp\n"\
);

PyDoc_STRVAR(setSpotLampColor_doc,
	"Set spot lamp color\n"\
	"\n"\
	"environment.setSpotLampColor(index, color)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 2)\n"\
	"    value : igeVmath.vec3\n"\
	"        lamp color"
);

PyDoc_STRVAR(getSpotLampPosition_doc,
	"Get spot lamp position\n"\
	"\n"\
	"pos = environment.getSpotLampPosition(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"Returns\n"\
	"-------\n"\
	"    pos : igeVmath.vec3\n"\
	"       lamp position\n"\
);

PyDoc_STRVAR(setSpotLampPosition_doc,
	"Set spot lamp position\n"\
	"\n"\
	"environment.getSpotLampPosition(index, pos)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"    pos : igeVmath.vec3\n"\
	"       lamp position\n"\
);

PyDoc_STRVAR(getSpotLampAngle_doc,
	"Get angle value of spot lamp\n"\
	"\n"\
	"angle = environment.getSpotLampAngle(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"Returns\n"\
	"-------\n"\
	"    angle : float\n"\
	"        angle value of spot lamp"
);

PyDoc_STRVAR(setSpotLampAngle_doc,
	"Set angle value of spot lamp\n"\
	"\n"\
	"environment.getSpotLampAngle(index, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)"
	"    value : float\n"\
	"        angle value of spot lamp\n"\
);

PyDoc_STRVAR(getSpotLampDirection_doc,
	"Get spot lamp direction\n"\
	"\n"\
	"dir = environment.getSpotLampPosition(index)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"Returns\n"\
	"-------\n"\
	"    dir : igeVmath.vec3\n"\
	"       lamp direction\n"\
);

PyDoc_STRVAR(setSpotLampDirection_doc,
	"Set spot lamp direction\n"\
	"\n"\
	"environment.getSpotLampDirection(index, dir)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    index : int\n"\
	"        light index no (0 - 6)\n"\
	"    dir : igeVmath.vec3\n"\
	"       lamp direction\n"\
);


PyDoc_STRVAR(ambientColor_doc,
	"Ambient color	\n"\
	"igeVmath.vec3\n"\
);

PyDoc_STRVAR(groundColor_doc,
	"Ground color for hemisphere ambient	\n"\
	"igeVmath.vec3\n"\
);

PyDoc_STRVAR(ambientDirection_doc,
	"Ambient direction for hemisphere ambient	\n"\
	"igeVmath.vec3\n"\
);

PyDoc_STRVAR(distanceFogNear_doc,
	"near value of fog distance	\n"\
	"float\n"\
);

PyDoc_STRVAR(distanceFogFar_doc,
	"far value of fog distance	\n"\
	"float\n"\
);

PyDoc_STRVAR(distanceFogAlpha_doc,
	"alpha value of fog distance	\n"\
	"float\n"\
);

PyDoc_STRVAR(distanceFogColor_doc,
	"distance value of fog distance	\n"\
	"float\n"\
);

PyDoc_STRVAR(shadowColor_doc,
	"shadow color	\n"\
	"igeVmath.vec3\n"\
);

PyDoc_STRVAR(shadowDensity_doc,
	"shadow density	\n"\
	"float\n"\
);

PyDoc_STRVAR(shadowWideness_doc,
	"shadow wideness	\n"\
	"float\n"\
);

