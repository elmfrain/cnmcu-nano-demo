-- Config Portion

local meshFiles = {
  "code-node-nano.obj"
}

local materials = {
  default = {
    diffuse = {1.0, 1.0, 1.0},
    specular = {0.0, 0.0, 0.0},
	ambient = {1.0, 1.0, 1.0},
    roughness = 0.35
  }
}

local objects = {
  pcb = {
    mesh = "pcb",
    material = materials.default
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

-- Setup, Logic, and Custom Behaviors Portion

-- Setup compositor and camera
local function initCompositorAndCamera()
  scene.compositor:setGamma(1.6)
  scene.compositor:setExposure(1.5)

  scene.camera.transform:setRotationEuler({-1.57, 0.0, 0.0})
end

local function createObjects()

end

local function createLights()

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