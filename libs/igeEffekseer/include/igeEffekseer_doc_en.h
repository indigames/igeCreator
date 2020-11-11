//effekseer TextureLoader
PyDoc_STRVAR(effekseerTextureLoader_doc,
	"set the texture loader callback\n"\
	"\n"\
	"effekseer.texture_loader(callback)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    callback : function(name, type)\n"\
	"        name : string\n"\
	"			the texture name\n"\
	"        type : int\n"\
	"			load or unload");

//effekseer update
PyDoc_STRVAR(effekseerShoot_doc,
	"update then render the effekseer system\n"\
	"\n"\
	"effekseer.shoot(projection_mat, view_mat, dt, clear_screen, update_enabled, render_enabled, culling_enabled)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    projection_mat : mat_obj\n"\
	"        projection matrix\n"\
	"    view_mat : mat_obj\n"\
	"        view matrix\n"\
	"    dt : float\n"\
	"        optional delta time (0.166666666667f as default)\n"\
	"    clear_sceen : bool\n"\
	"        optional clear the screen (false as default)\n"\
	"    update_enabled : bool\n"\
	"        optional need to update or not (true as default)\n"\
	"    render_enabled : bool\n"\
	"        optional need to update or not (true as default)\n"\
	"    culling_enabled : bool\n"\
	"        optional culling when rendering (culling_enabled && culling paramater from init function)");

//effekseer play
PyDoc_STRVAR(effekseerRender_doc,
	"render the effekseer system\n"\
	"\n"\
	"effekseer.render()");

//effekseer add
PyDoc_STRVAR(effekseerAdd_doc,
	"add the effect to the effekseer system\n"\
	"\n"\
	"handle = effekseer.add(name, loop, position, rotation, scale, target_position, cache_only)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    name : string\n"\
	"        the effect name\n"\
	"    loop : bool\n"\
	"        [Option] Is looping or not\n"\
	"    position : tuple or list\n"\
	"        [Option] The position\n"\
	"    rotation : tuple or list\n"\
	"        [Option] The position\n"\
	"    scale : tuple or list\n"\
	"        [Option] The position\n"\
	"    target_position : tuple or list\n"\
	"        [Option] The target position\n"\
	"    cache_only : bool\n"\
	"        [Option] add to cache list only\n"\
	"\n"\
	"Returns\n"\
	"------ - \n"\
	"    handle : int\n"\
	"        the effect handle id");

//effekseer remove
PyDoc_STRVAR(effekseerRemove_doc,
	"remove the effect from effekseer system\n"\
	"\n"\
	"effekseer.remove(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle id");

//effekseer play
PyDoc_STRVAR(effekseerPlay_doc,
	"play the effect handle\n"\
	"\n"\
	"effekseer.play(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle id");

//effekseer render
PyDoc_STRVAR(effekseerStopAllEffects_doc,
	"remove|stop all the effect from effekseer system\n"\
	"\n"\
	"effekseer.stop_all_effects()");

//effekseer DrawCallCount
PyDoc_STRVAR(effekseerDrawCallCount_doc,
	"return the drawcall count\n"\
	"\n"\
	"count = effekseer.drawcall_count()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    count : int");

//effekseer DrawVertexCount
PyDoc_STRVAR(effekseerDrawVertexCount_doc,
	"return the draw vertex count\n"\
	"\n"\
	"count = effekseer.vertex_count()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    count : int");

//effekseer UpdateTime
PyDoc_STRVAR(effekseerUpdateTime_doc,
	"return the update time take\n"\
	"\n"\
	"time = effekseer.update_time()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    time : int (micro second)");

//effekseer DrawTime
PyDoc_STRVAR(effekseerDrawTime_doc,
	"return the draw time take\n"\
	"\n"\
	"time = effekseer.draw_time()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    time : int (micro second)");

//effekseer set_target_location
PyDoc_STRVAR(effekseerSetTargetLocation_doc,
	"set target location for the effect handle\n"\
	"\n"\
	"effekseer.set_target_location(handle, x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    x : float\n"\
	"        x value\n"\
	"    y : float\n"\
	"        y value\n"\
	"    z : float\n"\
	"        z value");

//effekseer get_location
PyDoc_STRVAR(effekseerGetLocation_doc,
	"get location from the effect handle\n"\
	"\n"\
	"effekseer.get_location(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    location : tuple");

//effekseer set_location
PyDoc_STRVAR(effekseerSetLocation_doc,
	"set location for the effect handle\n"\
	"\n"\
	"effekseer.set_location(handle, x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    x : float\n"\
	"        x value\n"\
	"    y : float\n"\
	"        y value\n"\
	"    z : float\n"\
	"        z value");

//effekseer set_rotation
PyDoc_STRVAR(effekseerSetRotation_doc,
	"set rotation for the effect handle\n"\
	"\n"\
	"effekseer.set_rotation(handle, x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    x : float\n"\
	"        x value\n"\
	"    y : float\n"\
	"        y value\n"\
	"    z : float\n"\
	"        z value");

//effekseer set_scale
PyDoc_STRVAR(effekseerSetScale_doc,
	"set location for the effect handle\n"\
	"\n"\
	"effekseer.set_scale(handle, x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    x : float\n"\
	"        x value\n"\
	"    y : float\n"\
	"        y value\n"\
	"    z : float\n"\
	"        z value");

//effekseer set_color
PyDoc_STRVAR(effekseerSetColor_doc,
	"set color for the effect handle\n"\
	"\n"\
	"effekseer.set_color(handle, red, green, blue, alpha)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    red : int\n"\
	"        red value\n"\
	"    green : int\n"\
	"        green value\n"\
	"    blue : int\n"\
	"        blue value\n"\
	"    alpha : int\n"\
	"        alpha value");

//effekseer get_speed
PyDoc_STRVAR(effekseerGetSpeed_doc,
	"get speed from the effect handle\n"\
	"\n"\
	"speed = effekseer.get_speed(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    speed : float");

//effekseer set_speed
PyDoc_STRVAR(effekseerSetSpeed_doc,
	"set speed for the effect handle\n"\
	"\n"\
	"effekseer.set_speed(handle, speed)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    speed : float\n"\
	"        the speed value");

//effekseer is_playing
PyDoc_STRVAR(effekseerIsPlaying_doc,
	"checking the effect is playing\n"\
	"\n"\
	"result = effekseer.is_playing(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : bool");

//effekseer set_pause
PyDoc_STRVAR(effekseerSetPause_doc,
	"Pause the effect handle\n"\
	"\n"\
	"effekseer.set_pause(handle, paused)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    paused : bool");

//effekseer get_pause
PyDoc_STRVAR(effekseerGetPause_doc,
	"get the pause status from the effect handle\n"\
	"\n"\
	"result = effekseer.get_pause(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : bool");


//effekseer set_shown
PyDoc_STRVAR(effekseerSetShown_doc,
	"Switch between show or hide of the effect handle\n"\
	"\n"\
	"effekseer.set_shown(handle, shown, reset)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    shown : bool\n"\
	"        the effect handle\n"\
	"    reset : bool optional\n"\
	"        reset to first frame ");


//effekseer get_shown
PyDoc_STRVAR(effekseerGetShown_doc,
	"get the show status from the effect handle\n"\
	"\n"\
	"result = effekseer.get_shown(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : bool");


//effekseer set_loop
PyDoc_STRVAR(effekseerSetLoop_doc,
	"loop (true / false)\n"\
	"\n"\
	"effekseer.set_loop(handle, loop)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    loop : bool\n"\
	"        true / false");


//effekseer get_loop
PyDoc_STRVAR(effekseerGetLoop_doc,
	"get the loop status from the effect handle\n"\
	"\n"\
	"result = effekseer.get_loop(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : bool");


//desired framerate
PyDoc_STRVAR(framerate_doc,
	"the desired framerate\n"\
	"\n"\
	"    type : float (60.0 as default)	\n"\
	"    read / write");

//effekseer instance_count
PyDoc_STRVAR(effekseerInstanceCount_doc,
	"get the number of instances which is used in playing effects from the effect handle\n"\
	"\n"\
	"result = effekseer.instance_count(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : int");

//effekseer total_instance_count
PyDoc_STRVAR(effekseerGetTotalInstanceCount_doc,
	"get the number of instances which is used in playing effects\n"\
	"\n"\
	"result = effekseer.total_instance_count()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : int");

//effekseer set_dynamic_input
PyDoc_STRVAR(effekseerSetDynamicInput_doc,
	"specfiy a dynamic parameter, which changes effect parameters dynamically while playing\n"\
	"\n"\
	"effekseer.set_dynamic_input(handle, dynamic_input)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"    dynamic_input : tuple\n"\
	"        (input_1, input_2, input_3, input_4)");

//effekseer get_dynamic_input
PyDoc_STRVAR(effekseerGetDynamicInput_doc,
	"get the effect parameters from the effect handle\n"\
	"\n"\
	"result = effekseer.get_dynamic_input(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        the effect handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    result : tuple");