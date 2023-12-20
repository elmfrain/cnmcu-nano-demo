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

function Start()
  scene.compositor:setGamma(1.6)
  scene.compositor:setExposure(1.5)

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
  end

  for name, value in pairs(lights) do
    local light = scene.createLight(name)
    light.transform:setPosition(value.position)
    light:setColor(value.color)
    light:setIntensity(value.intensity)
  end
end

function Update(dt)
  
end