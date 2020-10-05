//init
PyDoc_STRVAR(soundInit_doc,
	"init the sound system \n"\
	"\n"\
	"sound.init()");

//release
PyDoc_STRVAR(soundRelease_doc,
	"release all sound source + buffer\n"\
	"\n"\
	"sound.release()");

//update
PyDoc_STRVAR(soundUpdate_doc,
	"update the 3d sound paramaters\n"\
	"\n"\
	"sound.update()");

//play
PyDoc_STRVAR(soundPlay_doc,
	"play the sound \n"\
	"\n"\
    "sound.play(name, stream, loop, is_3d, position, volume)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    name : string or int\n"\
	"        The sound name or sfxr preset id to play\n"\
	"    stream : bool (optional)\n"\
	"        Is streaming support? \n"\
	"    loop : bool (optional)\n"\
	"        when it is True, the sound will play and allow it to loop infinitely\n"\
	"    is_3d : bool (optional)\n"\
	"        to play 3D\n"\
	"    position : tuple (optional)\n"\
	"        (x, y, z) value\n"\
	"    volume : float (optional)\n"\
	"        0.0 : 1.0 (default -1.0 that will get from data source = 1.0)\n"\
	"Returns\n"\
	"-------\n"\
	"    sound handle : int");

//stop
PyDoc_STRVAR(soundStop_doc,
	"stop the sound \n"\
	"\n"\
	"sound.stop(filename)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name");

//stopAll
PyDoc_STRVAR(soundStopAll_doc,
	"stop all sound\n"\
	"\n"\
	"sound.stopAll()");

//load
PyDoc_STRVAR(soundLoad_doc,
	"load the sound to cache system\n"\
	"\n"\
	"sound.load(filename)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name\n"\
	"    stream : bool (optional)\n"\
	"        Is streaming support?");


//unload
PyDoc_STRVAR(soundUnload_doc,
	"unload the sound from cache system\n"\
	"\n"\
	"sound.unload(filename)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name");
    
//unloadUnused
PyDoc_STRVAR(soundUnloadUnused_doc,
	"unload the unused sound from cache system\n"\
	"\n"\
	"sound.unloadUnused()");

//setPositon
PyDoc_STRVAR(soundSetPositon_doc,
	"play the sound \n"\
	"\n"\
    "sound.setPositon(filename, x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name\n"\
	"    x : float\n"\
	"    y : float\n"\
	"    z : float");
    
//setPitch
PyDoc_STRVAR(soundSetPitch_doc,
	"play the sound \n"\
	"\n"\
    "sound.setPitch(filename, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name\n"\
	"    value : float\n"\
	"        The pitch value");
    
//setGain
PyDoc_STRVAR(soundSetGain_doc,
	"play the sound \n"\
	"\n"\
    "sound.setGain(filename, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name\n"\
	"    value : float\n"\
	"        The Gain value");
    
//setRolloff
PyDoc_STRVAR(soundSetRolloff_doc,
	"play the sound \n"\
	"\n"\
    "sound.setRolloff(filename, value)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    filename : string\n"\
	"        The sound name\n"\
	"    value : float\n"\
	"        The Rolloff value");
    
//setListenerPosition
PyDoc_STRVAR(soundSetListenerPosition_doc,
	"play the sound \n"\
	"\n"\
    "sound.setListenerPosition(x, y, z)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    x : float\n"\
	"    y : float\n"\
	"    z : float");
    
//setListenerOrientation
PyDoc_STRVAR(soundSetListenerOrientation_doc,
	"play the sound \n"\
	"\n"\
    "sound.setListenerOrientation(xAt, yAt, zAt, xUp, yUp, zUp)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    xAt : float\n"\
	"    yAt : float\n"\
	"    zAt : float\n"\
	"    xUp : float\n"\
	"    yUp : float\n"\
	"    zUp : float");

//fadeVolume
PyDoc_STRVAR(soundFadeVolume_doc,
	"Set up volume fader \n"\
	"\n"\
	"sound.fadeVolume(handle, aTo, aTime)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"    aTo : float\n"\
	"        The sound volume\n"\
	"    aTime : float\n"\
	"        After the second");

//scheduleStop
PyDoc_STRVAR(soundScheduleStop_doc,
	"Schedule a sound to stop \n"\
	"\n"\
	"sound.scheduleStop(handle, aTime)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"    aTime : float\n"\
	"        After the second");

//setGlobalVolume
PyDoc_STRVAR(soundSetGlobalVolume_doc,
	"Set up global sound volume\n"\
	"\n"\
	"sound.setGlobalVolume(volume)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    volume : float\n"\
	"        The global sound volume");

//getGlobalVolume_doc
PyDoc_STRVAR(soundGetGlobalVolume_doc,
	"Get the global sound volume\n"\
	"\n"\
	"sound.getGlobalVolume()\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    global sound volume : float");

//getStreamTime
PyDoc_STRVAR(soundGetStreamTime_doc,
	"Get the current play position, in seconds\n"\
	"\n"\
	"sound.getStreamTime(handle)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"\n"\
	"Returns\n"\
	"-------\n"\
	"    current play position, in seconds : float");

//set3dSourcePosition
PyDoc_STRVAR(soundSet3dSourcePosition_doc,
	"set the position parameters of a live 3d audio source. Only evaluated when the update() function is called.\n"\
	"\n"\
	"sound.set3dSourcePosition(handle, position)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"    position : tuple (x, y, z)\n"\
	"        the source position");

//set3dMinMaxDistance
PyDoc_STRVAR(soundSet3dMinMaxDistance_doc,
	"set the minimum and maximum distances for the audio source \n"\
	"\n"\
	"sound.set3dMinMaxDistance(handle, minmax)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"    minmax : tuple (min, max)\n"\
	"        the min max value");

//set3dAttenuation
PyDoc_STRVAR(soundSet3dAttenuation_doc,
	"set the rolloff factor\n"\
	"\n"\
	"sound.set3dAttenuation(handle, rolloffFactor)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    handle : int\n"\
	"        The sound handle\n"\
	"    rolloffFactor : float)\n"\
	"        the rolloff factor value");