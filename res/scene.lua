local meshes = {
  sare = "res/sare.obj",
  planet = "res/planet.obj",
  ring = "res/ring.obj",
  backdrop = "res/backdrop.obj"
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
  }
}

local lights = {
  cyan = {
    position = {-3.0, 5.0, 4.0},
    color = {0.0, 0.86, 1.0},
    intensity = 5.0
  },
  magenta = {
    position = {3.0, 5.0, -4.0},
    color = {1.0, 0.0, 0.77},
    intensity = 5.0
  },
  fore = {
    position = {0.0, 4.0, 0.0},
    color = {1.0, 1.0, 1.0},
    intensity = 1.3
  }
}

local function initCompositorAndCamera()
  scene.compositor:setGamma(1.6)
  scene.compositor:setExposure(1.5)
  scene.camera.transform:setPosition({0.0, 1.0, 0.0})
  scene.camera.transform:setRotationEuler({-1.57, 0.0, 0.0})
end

local function loadObjects()
  for name, value in pairs(objects) do
    local object = scene.createObject(name)
    local mesh = scene.loadMeshes(value.mesh)[1]
    mesh:makeRenderable()
    object:setMesh(mesh.ptr)
    
    object.material:setDiffuse(value.material.diffuse)
    object.material:setSpecular(value.material.specular)
    if value.material.roughness then
      object.material:setRoughness(value.material.roughness)
    end

    if name == "sare" then
      SareObject = object
    end
  end
end

local function loadLights()
  for name, value in pairs(lights) do
    local light = scene.createLight(name)
    light.transform:setPosition(value.position)
    light:setColor(value.color)
    light:setIntensity(value.intensity)
  end
end

function Start()
  initCompositorAndCamera()

  loadObjects()
  loadLights()

  SareObject:setDynamic(true)
  SareObject.dynamics.time = 0.0
  SareObject.dynamics:getTimeline("tml"):addTrack("xpos", -2.0, 1.0)
  SareObject.dynamics:getTimeline("tml"):setLooping(true)
  SareObject.dynamics:getTimeline("tml"):setDuration(2.0)
  SareObject.dynamics:getTimeline("tml"):play()
  SareObject.dynamics.onUpdate = function (dt, obj)
    obj.dynamics.time = obj.dynamics.time + dt
    local smoother = obj.dynamics:getSmoother("smt")

    local animation = obj.dynamics:getTimeline("tml")
    local x = animation:getValue("xpos")

    obj.transform:setPosition({x, 0.0, 0.0})

    smoother:setSpeed(3.0)
    smoother:setDamping(4.0)
    smoother:setSpringing(true)

    if(obj.dynamics.time > 1.0) then
      obj.dynamics.time = 0
      obj.dynamics.dir = not obj.dynamics.dir
      if obj.dynamics.dir then smoother:grab(0)
      else smoother:grab(3.14) end
    end

    local y = smoother:getValue()
    obj.transform:setRotationEuler({0.0, y, 0.0})
  end
end

function Update(dt)
  
end