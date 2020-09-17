//texture
PyDoc_STRVAR(texture_doc,
	"Textures are RGBA bitmap image objects \n"\
	"\n"\
	"Formula\n"\
	"----------\n"\
	"    tex = igeCore.texture(name)\n"\
	"        Create texture from image file.\n"\
	"    tex = igeCore.texture(name,width,height,format,depth,stencil,pixel)\n"\
	"        Create texture from parameters.\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    name : string\n"\
	"        If there are no parameters other than name, \n"\
	"        name is assumed to be the image file name.\n"\
	"        When combined with other parameters such as width and height, \n"\
	"        name is considered a unique name for the texture.\n"\
	"    width : int\n"\
	"        width of texture pixels.\n"\
	"    height : int\n"\
	"        height of texture pixels.\n"\
	"    format : int\n"\
	"        enable texture format\n"\
	"        GL_RED : 1 channel format (n,0,0,0)\n"\
	"        GL_RGB : 3 channel format (n,m,l,0)\n"\
	"        GL_RGBA : 4 channel format (n,m,l,o)\n"\
	"    depth : bool\n"\
	"        At the same time, a depth buffer is generated.\n"\
	"        This option is for use as a render target.\n"\
	"    stencil : bool\n"\
	"        At the same time, a stencil buffer is generated.\n"\
	"        This option is for use as a render target.\n"\
	"    pixel : numpy.ndarray or byte object\n"\
	"        array for bitmap image.\n");

//setImage
PyDoc_STRVAR(setImage_doc,
	"Set byte image to texture object\n"\
	"\n"\
	"texture.setImage(image, x, y, width,height)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    image : Bytes or numpy.ndarray\n"\
	"        array of rgb or rgba image\n"\
	"    x : int (optional)\n"\
	"        x offset of destination\n"\
	"    y : int (optional)\n"\
	"        y offset of destination\n"\
	"    width : int (optional)\n"\
	"        copy image width\n"\
	"    height : int (optional)\n"\
	"        copy image height\n");


//setCheckeredImage
PyDoc_STRVAR(setCheckeredImage_doc,
	"Set checkerd image to texture object\n"\
	"\n"\
	"texture.setCheckeredImage(r, g, b, a)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    r,g,b,a : int (optional)\n"\
	"        checker color (0 - 1)\n"\
	"        default color is red");


//setText
PyDoc_STRVAR(setText_doc,
	"Render text to texture object\n"\
	"\n"\
	"texture.setText(word, fontpath, size)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"	word : string\n"\
	"		word to render.\n"\
	"	fontpath : string\n"\
	"		font file path.\n"\
	"	size : int\n"\
	"		size of render text"\
	"	startX : int\n"\
	"		draw text X offset\n"\
	"	startY : int\n"\
	"		draw text Y offset\n"\
	"	clear : bool\n"\
	"		clear texture befor draw text\n");

//clear
PyDoc_STRVAR(clear_doc,
	"Clear the texture with the specified color \n"\
	"\n"\
	"texture.clear(r, g, b, a)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    r, g, b, a : float\n"\
	"        clear color(0-1).\n"\
	"        size of render text");

//capture
PyDoc_STRVAR(captureScreenshot_doc,
	"capture the screen \n"\
	"\n"\
	"texture.captureScreenshot(name, x, y, w, h, callback)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    name : string (optional)\n"\
	"        capture name\n"\
	"	 x : int\n"\
	"		x offset\n"\
	"	 y : int\n"\
	"		y offset\n"\
	"	 w : int\n"\
	"		width\n"\
	"	 h : int\n"\
	"		height\n"\
	"	 callback : function\n"\
	"		call the callback when the capturing is finished");

//capture
PyDoc_STRVAR(saveToGallery_doc,
    "save image captured to gallery \n"\
    "\n"\
    "texture.saveToGallery()");

//get render target data
PyDoc_STRVAR(getData_doc,
	"Get the render target data\n"\
	"\n"\
	"texture.getData(x,y,w,h)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    x,y,w,h : int (optional)\n"\
	"        default color is (0, 0, -1, -1)\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : list(unsigned char)");

//get resource name
PyDoc_STRVAR(getResourceName_doc,
    "Get the resource name\n"\
    "\n"\
    "texture.getResourceName()\n"\
    "\n"\
    "Returns\n"\
    "-------\n"\
    "    result : string");


//width
PyDoc_STRVAR(width_doc,
	"Texture width \n"\
	"\n"\
	"    type : int	\n"\
	"    read only");

//height
PyDoc_STRVAR(height_doc,
	"Texture height \n"\
	"\n"\
	"    type : int	\n"\
	"    read only");

//bitSize
PyDoc_STRVAR(bitSize_doc,
	"Texture bit size \n"\
	"\n"\
	"    type : int	\n"\
	"    read only");

//format
PyDoc_STRVAR(format_doc,
	"Texture format\n"\
	"\n"\
	"    type : int	\n"\
	"    read only");

//numMips
PyDoc_STRVAR(numMips_doc,
	"Number of texture mipmaps\n"\
	"\n"\
	"    type : int	\n"\
	"    read only");

//id
PyDoc_STRVAR(id_doc,
	"The texture handle id\n"\
	"\n"\
	"    type : int	\n"\
	"    read only");
