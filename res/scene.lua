-- Config Portion

local meshFiles = {
  "res/code-node-nano.obj"
}

local materials = {
  default = {
    diffuse = {1.0, 1.0, 1.0},
    specular = {0.0, 0.0, 0.0},
	  ambient = {0.02, 0.02, 0.02},
    roughness = 0.5
  },
  dust = {
    diffuse = {1.0, 0.0, 0.0},
    specular = {0.0, 0.0, 0.0},
    ambient = {0.02, 0.00, 0.00},
    roughness = 0.5
  }
}

local objects = {
  pcb = {
    mesh = "pcb",
    material = materials.default
  },
  smd = {
    mesh = "smd",
    material = materials.default
  },
  northDust = {
    mesh = "north-dust",
    material = materials.dust,
  },
  southDust = {
    mesh = "south-dust",
    material = materials.dust,
  },
  westDust = {
    mesh = "west-dust",
    material = materials.dust
  },
  eastDust = {
    mesh = "east-dust",
    material = materials.dust
  },
}

local lights = {
  -- ambient = {
  --   position = {100.0, 100.0, 100.0},
  --   color = {1.0, 1.0, 1.0},
  --   intensity = 0.5
  -- },
  left = {
    position = {0.5, 5.0, -4.5},
    color = {1.0, 1.0, 1.0},
    intensity = 4.5
  },
  right = {
    position = {0.5, 5.0, 4.5},
    color = {1.0, 1.0, 1.0},
    intensity = 4.5
  }
}

-- Setup, Logic, and Custom Behaviors Portion

-- Setup compositor and camera
local function initCompositorAndCamera()
  scene.compositor:setGamma(1.6)
  scene.compositor:setExposure(1.2)

  scene.camera.transform:setPosition({0.5, 5.0, -0.5})
  scene.camera.transform:setRotationEuler({-1.57, 0.0, 0.0})
end

local function createObjects()
  local createdObjects = {}
  local loadedMeshes = {}

  for _, file in ipairs(meshFiles) do
    local meshes = scene.loadMeshes(file)

    for _, mesh in ipairs(meshes) do
      loadedMeshes[mesh:getName()] = mesh
    end
  end

  for name, value in pairs(objects) do
    local object = scene.createObject(name)
    local mesh = loadedMeshes[value.mesh]

    if mesh then
      mesh:makeRenderable()
      object:setMesh(mesh.ptr)
    end

    if value.material.diffuse then
      object.material:setDiffuse(value.material.diffuse)
    end

    if value.material.specular then
      object.material:setSpecular(value.material.specular)
    end

    if value.material.ambient then
      object.material:setAmbient(value.material.ambient)
    end

    if value.material.roughness then
      object.material:setRoughness(value.material.roughness)
    end

    if value.position then
      object.transform:setPosition(value.position)
    end

    if value.rotation then
      object.transform:setRotationEuler(value.rotation)
    end

    if value.scale then
      object.transform:setScale(value.scale)
    end

    if value.color then
      object.material:setDiffuse(value.color)
    end

    createdObjects[name] = object
  end

  for name, value in pairs(objects) do
    if value.parent then
      createdObjects[value.parent]:addChild(createdObjects[name])
    end
  end
end

local function createLights()
  for name, value in pairs(lights) do
    local light = scene.createLight(name)

    light.transform:setPosition(value.position)
    light:setColor(value.color)
    light:setIntensity(value.intensity)
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