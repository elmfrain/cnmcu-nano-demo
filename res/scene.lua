-- Config Portion

local meshes = {
  sare = "res/sare.obj",
  planet = "res/planet.obj",
  ring = "res/ring.obj",
  backdrop = "res/backdrop.obj",
  ringPlanet6 = "res/ring-planet-6.obj",
  ringPlanet3 = "res/ring-planet-3.obj"
}

local materials = {
  metallic = {
    diffuse = {0.0, 0.0, 0.0},
    specular = {0.12, 0.12, 0.12},
    roughness = 0.35
  },
  backdrop = {
    diffuse = {0.5, 0.5, 0.5},
    specular = {0.0, 0.0, 0.0}
  }
}

local objects = {
  sare = {
    mesh = meshes["sare"],
    material = materials["metallic"]
  },
  planet = {
    mesh = meshes["planet"],
    material = materials["metallic"]
  },
  ring = {
    mesh = meshes["ring"],
    material = materials["metallic"]
  },
  backdrop = {
    mesh = meshes["backdrop"],
    material = materials["backdrop"]
  },
  ringPlanet31 = {
    position = {-1.414, 0.0, -0.7},
    mesh = meshes.ringPlanet3,
    material = materials.metallic,
    parent = "ring"
  },
  ringPlanet32 = {
    position = {-1.21, 0.0, 0.088},
    scale = {0.5, 0.5, 0.5},
    mesh = meshes.ringPlanet3,
    material = materials.metallic,
    parent = "ring"
  },
  ringPlanet33 = {
    position = {-0.904, 0.0, -1.05},
    scale = {0.22, 0.22, 0.22},
    mesh = meshes.ringPlanet3,
    material = materials.metallic,
    parent = "ring"
  },
  ringPlanet34 = {
    position = {1.24, 0.0, -0.033},
    scale = {0.38, 0.38, 0.38},
    mesh = meshes.ringPlanet3,
    material = materials.metallic,
    parent = "ring"
  },
  ringPlanet35 = {
    position = {1.338, 0.0, 0.929},
    scale = {0.32, 0.32, 0.32},
    mesh = meshes.ringPlanet3,
    material = materials.metallic,
    parent = "ring"
  },
  ringPlanet6 = {
    position = {0.685, 0.0, 1.09},
    mesh = meshes.ringPlanet6,
    material = materials.metallic,
    parent = "ring"
  }
}

local lights = {
  cyan = {
    position = {-5.1, 0.0, -1.2},
    color = {0.0, 0.86, 1.0},
    intensity = 2.0
  },
  magenta = {
    position = {4.9, 0.0, 1.7},
    color = {1.0, 0.0, 0.77},
    intensity = 2.5
  },
  fore = {
    position = {0.0, 4.0, 0.0},
    color = {1.0, 1.0, 1.0},
    intensity = 1.3
  },
  cross = {
    position = {0.0, 3.2, 0.0},
    color = {1.0, 1.0, 1.0},
    intensity = 0.3
  },
  back = {
    position = {0.0, -1.1, 0.0},
    color = {1.0, 0.95, 0.74},
    intensity = 0.7
  }
}

Duration = 7.5

-- Setup, Logic, and Custom Behaviors Portion

-- Setup compositor and camera
local function initCompositorAndCamera()
  scene.compositor:setGamma(1.6)
  scene.compositor:setExposure(1.5)

  scene.camera.transform:setRotationEuler({-1.57, 0.0, 0.0})

  -- Create camera animation
  scene.camera:setDynamic(true)
  local camYSm = scene.camera.dynamics:getSmoother("ypos")
  camYSm:setSpeed(0.6)
  local camFovSm = scene.camera.dynamics:getSmoother("fov")
  camFovSm:setSpeed(0.5)
  scene.camera.camYSm = camYSm
  scene.camera.camFovSm = camFovSm
  local camAni = scene.camera.dynamics:getTimeline("ani")
  camAni:setDuration(Duration)
  camAni:addTrack("ypos", 12.6, 3.4, "SINE_IN")
  camYSm:setValueAndGrab(12.6)
  camAni:addTrack("fov", 10.8, 99.5, "SINE_IN")
  camFovSm:setValueAndGrab(10.8)
  camAni:play()
  scene.camera.camAni = camAni
  scene.camera.dynamics.onUpdate = function (dt, obj)
    local y = obj.camYSm:getValue()
    local fov = obj.camFovSm:getValue()
    obj:setFieldOfView(fov)
    obj.transform:setPosition({0.0, y, 0.0})

    if obj.camAni:isStopped() then return end
    obj.camYSm:grab(obj.camAni:getValue("ypos"))
    obj.camFovSm:grab(obj.camAni:getValue("fov"))
  end
  -- / Create camera animation
end

local function createObjects()
  local createdObjects = {}
  local loadedMeshes = {}
  for name, value in pairs(objects) do
    local object = scene.createObject(name)
    local mesh = loadedMeshes[value.mesh]
    if mesh == nil then
      mesh = scene.loadMeshes(value.mesh)[1]
      mesh:makeRenderable()
      loadedMeshes[value.mesh] = mesh
    end
    object:setMesh(mesh.ptr)
    
    object.material:setDiffuse(value.material.diffuse)
    object.material:setSpecular(value.material.specular)
    if value.material.roughness then
      object.material:setRoughness(value.material.roughness)
    end
    if value.position then
      object.transform:setPosition(value.position)
    end
    if value.scale then
      object.transform:setScale(value.scale)
    end

    createdObjects[name] = object

    if name == "ring" then Ring = object end
    if name == "ringPlanet6" then RingPlanet6 = object end
  end

  for name, value in pairs(objects) do
    if value.parent then
      createdObjects[value.parent]:addChild(createdObjects[name])
    end
  end

  -- Create ring animation
  Ring:setDynamic(true)
  local anim = Ring.dynamics:getTimeline("anim")
  anim:setDuration(Duration)
  anim:addTrack("yrot", 2.79, 0.0, "EXPO_OUT")
  anim:play()
  Ring.anim = anim
  Ring.dynamics.onUpdate = function (dt, obj)
    local yrot = obj.anim:getValue("yrot")
    if obj.anim:isStopped() then return end
    obj.transform:setRotationEuler({0.0, yrot, 0.0})
  end

  -- Create planet 6 animation
  RingPlanet6:setDynamic(true)
  RingPlanet6.dynamics.onUpdate = function (dt, obj)
    local yrot = Ring.transform:getRotationEuler()[2]
    obj.transform:setRotationEuler({0.0, yrot * 50.0, 0.0})
  end

end

local function createLights()
  -- Create cross light and its animation
  local cross = scene.createLight("cross-light")
  cross:setColor(lights.cross.color)
  cross:setDynamic(true)
  local anim = cross.dynamics:getTimeline("anim")
  anim:setDuration(Duration * 0.5)
  anim:addTrack("xpos", -2.44, 1.9)
  anim:addTrack("zpos", -2.62, 1.42)
  anim:addTrack("intensity", lights.cross.intensity, 0.0, "EXPO_IN")
  anim:play()
  cross.anim = anim
  cross.dynamics.onUpdate = function (dt, obj)
    local x = obj.anim:getValue("xpos")
    local z = obj.anim:getValue("zpos")
    local intensity = obj.anim:getValue("intensity")
    if obj.anim:isStopped() then return end
    obj:setIntensity(intensity)
    obj.transform:setPosition({x, 3.3, z})
  end

  -- Create chromatic lights and their animations
  local chromaticHighlights = scene.createObject("chromatic-highlights")
  local cyan = scene.createLight("cyan")
  cyan:setColor(lights.cyan.color)
  cyan.transform:setPosition(lights.cyan.position)
  local magenta = scene.createLight("magenta")
  magenta:setColor(lights.magenta.color)
  magenta.transform:setPosition(lights.magenta.position)
  chromaticHighlights:addChild(cyan)
  chromaticHighlights:addChild(magenta)
  chromaticHighlights:setDynamic(true)
  anim = chromaticHighlights.dynamics:getTimeline("anim")
  anim:setDuration(Duration)
  anim:addTrack("yrot", -0.73, -5.6, "QUAD_OUT")
  anim:addTrack("ypos", -0.2, 5.6)
  anim:addTrack("intensity", 0.0, 1.0)
  anim:getTrack("intensity"):addKeyframe(0.5, 1.0)
  anim:play()
  chromaticHighlights.anim = anim
  chromaticHighlights.cyan = cyan
  chromaticHighlights.magenta = magenta
  chromaticHighlights.dynamics.onUpdate = function (dt, obj)
    local yrot = obj.anim:getValue("yrot")
    local ypos = obj.anim:getValue("ypos")
    local intensity = obj.anim:getValue("intensity")
    if obj.anim:isStopped() then return end
    obj.transform:setPosition({0.0, ypos, 0.0})
    obj.transform:setRotationEuler({0.0, yrot, 0.0})
    obj.cyan:setIntensity(lights.cyan.intensity * intensity)
    obj.magenta:setIntensity(lights.magenta.intensity * intensity)
  end

  -- Create fore light and its animations
  local fore = scene.createLight("fore-light")
  fore.transform:setPosition(lights.fore.position)
  fore:setColor(lights.fore.color)
  fore:setDynamic(true)
  anim = fore.dynamics:getTimeline("anim")
  anim:setDuration(Duration)
  anim:addTrack("intensity", 0.0, 1.0)
  anim:getTrack("intensity"):addKeyframe(0.22, 0.0, "QUAD_IN")
  anim:play()
  fore.anim = anim
  fore.dynamics.onUpdate = function (dt, obj)
    local intensity = obj.anim:getValue("intensity")
    if obj.anim:isStopped() then return end
    obj:setIntensity(lights.fore.intensity * intensity)
  end

  -- Create back light and its animations
  local back = scene.createLight("back-light")
  back.transform:setPosition(lights.back.position)
  back:setColor(lights.back.color)
  back:setDynamic(true)
  anim = back.dynamics:getTimeline("anim")
  anim:setDuration(Duration)
  anim:addTrack("intensity", 0.0, 1.0)
  anim:getTrack("intensity"):addKeyframe(0.38, 0.0)
  anim:getTrack("intensity"):addKeyframe(0.78, 1.0)
  anim:play()
  back.anim = anim
  back.dynamics.onUpdate = function (dt, obj)
    local intensity = obj.anim:getValue("intensity")
    if obj.anim:isStopped() then return end
    obj:setIntensity(lights.back.intensity * intensity)
  end
end

-- Called upon scene starting up
function Start()
  initCompositorAndCamera()

  createObjects()
  createLights()
end

-- Called upon every frame
function Update(dt)
  
end