
#include <GLFW/glfw3.h>
#include <engine/Billboard.h>
#include <engine/CollisionBox.h>
#include <engine/Objectives.h>
#include <engine/Particles.h>
#include <engine/Plane.h>
#include <engine/QuadTexture.h>
#include <engine/RigidModel.h>
#include <engine/Terrain.h>
#include <engine/functions.h>
#include <engine/shader_m.h>
#include <engine/skybox.h>
#include <engine/textrenderer.h>
#include <glad/glad.h>
#include <iostream>

int main()
{
    //:::: INICIALIZAMOS GLFW CON LA VERSIÓN 3.3 :::://
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //:::: CREAMOS LA VENTANA:::://
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RECOLECTA", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //:::: OBTENEMOS INFORMACIÓN DE TODOS LOS EVENTOS DE LA VENTANA:::://
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetJoystickCallback(joystick_callback);

    //:::: DESHABILITAMOS EL CURSOR VISUALMENTE EN LA PANTALLA :::://
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //:::: INICIALIZAMOS GLAD CON LAS CARACTERÍSTICAS DE OPENGL ESCENCIALES :::://
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //INICIALIZAMOS LA ESCENA
    Shader ourShader("shaders/multiple_lighting.vs", "shaders/multiple_lighting.fs");
    Shader selectShader("shaders/selectedShader.vs", "shaders/lighting_maps.fs");
    initScene(ourShader);
    Terrain terrain("textures/hm3.jpg", texturePaths);
    SkyBox sky(1.0f, "10");
    cb = isCollBoxModel ? &models[indexCollBox].collbox : &collboxes.at(indexCollBox).second;

    //:::: RENDER:::://
    while (!glfwWindowShouldClose(window))
    {

        //::::TIMING:::://Ayuda a crear animaciones fluidas
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame);
        lastFrame = currentFrame;
        respawnCount += 0.1;

        //::::ENTRADA CONTROL:::://
        processInput(window);
        //:::: LIMPIAMOS BUFFERS:::://
        glClearColor(0.933f, 0.811f, 0.647f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //:::: PASAMOS INFORMACIÓN AL SHADER:::://
        ourShader.use();

        //:::: DEFINICIÓN DE MATRICES::::// La multiplicaciónd e model*view*projection crea nuestro entorno 3D
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //:::: FISICAS:::://
        physicsUpdate(&ourShader);
        //:::: RENDER DE MODELOS:::://
        drawModels(&ourShader, view, projection);
        //:::: SKYBOX Y TERRENO:::://
        loadEnviroment(&terrain, &sky, view, projection);
        //:::: COLISIONES:::://
        collisions();
        //:::: COLISIONES:::://
        pickObjects(&ourShader, &selectShader, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //:::: LIBERACIÓN DE MEMORIA:::://
   
    delete[] texturePaths;
    sky.Release();
    terrain.Release();
    for (int i = 0; i < lightcubes.size(); i++)
        lightcubes[i].second.Release();
    for (int i = 0; i < collboxes.size(); i++)
        collboxes[i].second.Release();
    for (int i = 0; i < models.size(); i++)
        models[i].Release();
    for (int i = 0; i < rigidModels.size(); i++)
    {
        physicsEnviroment.Unregister(rigidModels[i].getRigidbox());
    }
    physicsEnviroment.Unregister(&piso);
    physicsEnviroment.Unregister(&pared);
    glfwTerminate();

    return 0;
}

void initScene(Shader ourShader)
{

    //AGUA
    //:::: DEFINIMOS LAS TEXTURAS DE LA MULTITEXTURA DEL TERRENO :::://
    texturePaths = new const char *[4];
    texturePaths[0] = "textures/multi1.jpg";
    texturePaths[1] = "textures/test3.png";
    texturePaths[2] = "textures/test1.png";
    texturePaths[3] = "textures/test2.png";
    //:::: POSICIONES DE TODAS LAS LUCES :::://
    pointLightPositions.push_back(glm::vec3(2.3f, 5.2f, 2.0f));
    pointLightPositions.push_back(glm::vec3(2.3f, 10.3f, -4.0f));
    pointLightPositions.push_back(glm::vec3(-4.0f, 10.0f, -12.0f));
    pointLightPositions.push_back(glm::vec3(0.0f, 10.0f, -3.0f));

    //:::: POSICIONES DE TODOS LOS OBJETOS CON FISICAS :::://
    physicsObjectsPositions.push_back(glm::vec3(0.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(2.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(1.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(25.0, 10.0, -2.0));

    //:::: OBJETIVOS :::://
    objectives.AddObjective("recogerLampa", false);
    objectives.AddObjective("dejarLampa", false);
    objectives.AddObjective("dejarCofre", false);
    objectives.AddObjective("dejarJarron", false);

   


    //:::: ESTADO GLOBAL DE OPENGL :::://
    glEnable(GL_DEPTH_TEST);

    //Definimos los collbox de la camara
    camera.setCollBox();
    //:::: CARGAMOS LOS SHADERS :::://

    //:::: IMPLEMENTAMOS EL SHADER PARA SU CONFIGURACIÓN :::://
    ourShader.use();

    //:::: INICIALIZAMOS NUESTROS OBJETOS :::://
    //:::: PARTICULAS:::://

    //:::: BILLBOARDS :::://

    //:::: GUI:::://
    secondMessage = new QuadTexture("textures/containers.png", 240.0f, 240.0f, 4, 0);
    Text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
    Text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
    Text->Load("fonts/OCRAEXT.TTF", 60);
   
    mainMessage = QuadTexture("textures/containers.png", 240.0f, 240.0f, 0, 0);
    p1 = QuadTexture("textures/items.png", 98.0f, 98.0f, 0, 0);
    p2 = QuadTexture("textures/items.png", 98.0f, 98.0f, 0, 0);
    p3 = QuadTexture("textures/items.png", 98.0f, 98.0f, 0, 0);

    //:::: INICIALIZAMOS NUESTROS MODELOS :::://
    
    models.push_back(Model("Malo", "models/Malo/Malo.obj", glm::vec3(5.69, 0.191, 31.84), glm::vec3(0, 90, 0), 0.0f, initScale));
    
    //AQUÍ

    pickModels.push_back(Model("cat", "models/Cat/CatProta.obj", glm::vec3(11.26, 0.2, 6.9), glm::vec3(0, 0, 0), 0.0f, initScale));
    pickModels.push_back(Model("cat2", "models/Cat/CatProta.obj", glm::vec3(0.7f, -1.8f, -0.4f), glm::vec3(0, 45, 0), 0.0f, 1.0));
    pickModels.push_back(Model("lampara", "models/Lampara/Lampara.obj", glm::vec3(15.9401, 0.8, 6.52), glm::vec3(0, 91, 0), 0.0f, 0.6));
    pickModels.push_back(Model("lampara2", "models/Lampara/Lampara.obj", glm::vec3(0.55f, -1.5f, -0.5f), glm::vec3(0, 0, 0), 0.0f, 2.0));
    pickModels.push_back(Model("jarron", "models/Jarrones/JA.obj", glm::vec3(20.9401, 0.5, 6.52), glm::vec3(0, 91, 0), 0.0f, 0.4));
    pickModels.push_back(Model("jarron2", "models/Jarrones/JA.obj", glm::vec3(0.60f, -1.2f, -0.5f), glm::vec3(0, 0, 0), 0.0f, 0.8));

    //CREAMOS TODAS  LAS CAJAS DE COLISION INDIVIDUALES
    CollisionBox collbox;
    glm::vec4 colorCollbox(0.41f, 0.2f, 0.737f, 0.06f);
    
    //CREAMOS LOS LIGHTCUBES QUE ENREALIDAD SON COLLISION BOXES QUE NOS AYUDARAN A IDENTIFICAR LA POSICIÓN DE DONDE ESTAN
    glm::vec3 lScale(0.5);
    colorCollbox = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f);
    collbox = CollisionBox(pointLightPositions[0], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(0, pair<string, CollisionBox>("LUZ1", collbox)));
    collbox = CollisionBox(pointLightPositions[1], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(1, pair<string, CollisionBox>("LUZ2", collbox)));
    collbox = CollisionBox(pointLightPositions[2], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(2, pair<string, CollisionBox>("LUZ3", collbox)));
    collbox = CollisionBox(pointLightPositions[3], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(3, pair<string, CollisionBox>("LUZ4", collbox)));

    //:::: FISICAS :::://
    //AÑADIMOS TODOS LOS MODELOS DEL TIPO RIGID MODEL
    rigidModels.push_back(RigidModel("carrito", "models/carrito.obj", physicsObjectsPositions[0], 50.0, 0.0, 0.5, glm::vec3(0.08), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("sprite", "models/sprite.obj", physicsObjectsPositions[1], 10.0, 0.0, 0.5, glm::vec3(0.12), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("coca", "models/coca.obj", physicsObjectsPositions[2], 10.0, 0.0, 0.5, glm::vec3(0.12), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("mundet", "models/mundet.obj", physicsObjectsPositions[3], 10.0, 0.0, 0.5, glm::vec3(0.12), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("pepsi", "models/pepsi.obj", physicsObjectsPositions[4], 10.0, 0.0, 0.5, glm::vec3(0.12), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("basura", "models/basura.obj", glm::vec3(4.22502, 0.1, 3.80488), 10.0, 0.0, 0.5, glm::vec3(0.2), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("bolsabasura", "models/trashbag.obj", glm::vec3(4.54844, 0.1, 4.37938), 10.0, 0.0, 0.5, glm::vec3(0.22), rbRigidBody::Attribute_AutoSleep));
    rigidModels.push_back(RigidModel("bolaboliche", "models/bowlingBall.obj", physicsObjectsPositions[7], 10.0, 0.0, 0.5, glm::vec3(0.2), rbRigidBody::Attribute_AutoSleep));

    //QUITAMOS DEL ARREGLO PRINCIPAL EL BOTE DE BASURA PARA QUE NO COLISIONE CONSIGO MISMO
    rbmodels = rigidModels;
    rbmodels.erase(rbmodels.begin() + 4, rbmodels.end());

    //DEFINIMOS LOS RIGID BODY (SON CUERPOS QUE LES AFECTA LA FISICA Y COLISIONAN ENTRE SI)
    rbEnvironment::Config config;
    config.RigidBodyCapacity = 100;              //CANTIDAD DE RIGID BODIES
    config.ContactCapacty = 100;                 //CANTIDAD DE CONTACTOS ENTRE LSO RIGID BODIES
    piso.SetShapeParameter(10000.0f,             //MASA
                           100.0f, 0.5f, 100.0f, //TAMAÑO
                           0.1f, 0.3f);          //COEF RESTITUCIÓN, COEF FRICCIÓN
    piso.SetPosition(rbVec3(0.0, -0.35, 0.0));
    piso.EnableAttribute(rbRigidBody::Attribute_AutoSleep); //SERÁ FIJO NO LE AFECTARÁ LA FISICA SOLO  SERVIRÁ PARA COLISIONAR
    piso.name = "piso";
    pared.SetShapeParameter(10.0f,
                            0.3, 10, 6.4,
                            0.0f, 0.5f);
    pared.SetPosition(rbVec3(5, -0.3, 0.0));
    pared.EnableAttribute(rbRigidBody::Attribute_Fixed);
    pared.name = "pared";
    //AÑADIMOS AL AMBIENTE DE FISICAS LOS RIGIDBODIES QUE CREAMOS PARA QUE EL SISTEMA HAGA QUE COLISIONEN ENTRE SI
    physicsEnviroment = rbEnvironment(config);
    physicsEnviroment.Register(&piso);
    physicsEnviroment.Register(&pared);
    for (int i = 0; i < rigidModels.size(); i++)
        physicsEnviroment.Register(rigidModels[i].getRigidbox());

    //:::: AMBIENTE:::://

   
}
//:::: CONFIGURACIONES :::://
void pickObjects(Shader *ourShader, Shader *selectShader, glm::mat4 projection)
{
    //SI COLISIONO DIBUJAMOS EL EXTINTOR QUE USA EL SHADER SELECTSHADER.VS
    glm::vec3 m = camera.Position + camera.Front * 1.2f; //UBICAMOS AL FRENTE DE LA CAMARA AL EXTINTOR
    if (pickObject && CofreColi)
    {
        selectShader->use();
        setSimpleLight(selectShader);
        pickModels[1]
            .Draw(*selectShader);
        pickModels[0]
            .setPosition(glm::vec3(m.x, 0.2, m.z));
        selectShader->notUse();
    }
    else
    {
        ourShader->use();
        pickModels[0].Draw(*ourShader);
        ourShader->notUse();
    }

    if (pickObject && LamparaColi)
    {
        selectShader->use();
        setSimpleLight(selectShader);
        pickModels[3]
            .Draw(*selectShader);
        pickModels[2].setPosition(glm::vec3(m.x, 0.2, m.z));
        selectShader->notUse();
    }
    else
    {
        ourShader->use();
        pickModels[2].Draw(*ourShader);
        ourShader->notUse();
    }

    if (pickObject && JarronColi)
    {
        selectShader->use();
        setSimpleLight(selectShader);
        pickModels[5]
            .Draw(*selectShader);
        pickModels[4]
            .setPosition(glm::vec3(m.x, 0.2, m.z));
        selectShader->notUse();
    }
    else
    {
        ourShader->use();
        pickModels[4].Draw(*ourShader);
        ourShader->notUse();
    }
}
void loadEnviroment(Terrain *terrain, SkyBox *sky, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, -2.5f, 0.0f));   // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(100.5f, 100.0f, 100.5f)); // it's a bit too big for our scene, so scale it down

    terrain->draw(model, view, projection);
    terrain->getShader()->setFloat("shininess", 100.0f);
    setMultipleLight(terrain->getShader(), pointLightPositions);

    glm::mat4 skyModel = mat4(1.0f);
    skyModel = glm::translate(skyModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    skyModel = glm::scale(skyModel, glm::vec3(40.0f, 40.0f, 40.0f));  // it's a bit too big for our scene, so scale it down
    sky->draw(skyModel, view, projection, skyPos);
    sky->getShader()->setFloat("shininess", 10.0f);
    setMultipleLight(sky->getShader(), pointLightPositions);

  
    //RENDER DE LIGHT CUBES
    if (renderLightingCubes)
        for (pair<int, pair<string, CollisionBox>> lights : lightcubes)
            lights.second.second.draw(view, projection);

    
}
void collisions()
{

    detectColls(collboxes, &camera, renderCollBox, collidedObject_callback);
    //NUEVO
    if (!CofreColi || !pickObject)
        detectColls(&pickModels[0].collbox, pickModels[0].name, &camera, renderCollBox, collidedObject_callback);
    if (!LamparaColi || !pickObject)
        detectColls(&pickModels[2].collbox, pickModels[2].name, &camera, renderCollBox, collidedObject_callback);
    if (!JarronColi || !pickObject)
        detectColls(&pickModels[4].collbox, pickModels[4].name, &camera, renderCollBox, collidedObject_callback);

    detectColls(&physicsEnviroment, collidedPhysicsObject_callback);

    //NUEVO
    if (renderCountPhysicsCollided > 0)
    {
        renderCountPhysicsCollided -= 0.1;
        showSecondMessage = true;
        Text->RenderText(textRendererPhysicsObjectsCollided, -0.45f, 0.7f, 0.001f, glm::vec3(1.0f, 1.0f, 0.0f));
        secondMessage->Draw(glm::vec2(0.0f, -0.75f), 1.0f);
    }
}
void physicsUpdate(Shader *shader)
{
    int div = 3;
    const rbVec3 G(0.0f, rbReal(-9.8), 0.0f); //FUERZA DE GRAVEDAD 9.8
    for (int i = 0; i < rigidModels.size(); i++)
    {
        //DEFINIMOS LA FUERZA Y DIBUJAMOS EL OBJETO
        rigidModels[i].getRigidbox()->SetForce(G);
        rigidModels[i].Draw(*shader);
    }
    physicsEnviroment.Update(deltaTime, div);
}
void drawModels(Shader *shader, glm::mat4 view, glm::mat4 projection)
{
    //DEFINIMOS EL BRILLO  DEL MATERIAL
    shader->setFloat("material.shininess", 60.0f);
    setMultipleLight(shader, pointLightPositions);
    for (int i = 0; i < models.size(); i++)
    {
        //SI SE RECOGIO EL OBJETO
        shader->use();
        models[i].Draw(*shader);
        shader->notUse();
        detectColls(&models[i].collbox, models[i].name, &camera, renderCollBox, collidedObject_callback);
    }
}
void setSimpleLight(Shader *shader)
{
    shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader->setInt("lightType", (int)lightType);
    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.direction", lightDir);
    shader->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader->setVec3("viewPos", camera.Position);
    shader->setFloat("light.constant", 0.2f);
    shader->setFloat("light.linear", 0.02f);
    shader->setFloat("light.quadratic", 0.032f);
    shader->setFloat("material.shininess", 60.0f);
}
void setMultipleLight(Shader *shader, vector<glm::vec3> pointLightPositions)
{
    shader->setVec3("viewPos", camera.Position);

    shader->setVec3("dirLights[0].direction", pointLightPositions[0]);
    shader->setVec3("dirLights[0].ambient", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].diffuse", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].specular", mainLight.x, mainLight.y, mainLight.z);

    shader->setVec3("dirLights[1].direction", pointLightPositions[1]);
    shader->setVec3("dirLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[1].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[1].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("dirLights[2].direction", pointLightPositions[2]);
    shader->setVec3("dirLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[2].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[2].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("dirLights[3].direction", pointLightPositions[3]);
    shader->setVec3("dirLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[3].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[3].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("pointLights[0].position", pointLightPositions[0]);
    shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[0].constant", 1.0f);
    shader->setFloat("pointLights[0].linear", 0.09);
    shader->setFloat("pointLights[0].quadratic", 0.032);

    shader->setVec3("pointLights[1].position", pointLightPositions[1]);
    shader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[1].constant", 1.0f);
    shader->setFloat("pointLights[1].linear", 0.09);
    shader->setFloat("pointLights[1].quadratic", 0.032);

    shader->setVec3("pointLights[2].position", pointLightPositions[2]);
    shader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[2].constant", 1.0f);
    shader->setFloat("pointLights[2].linear", 0.09);
    shader->setFloat("pointLights[2].quadratic", 0.032);

    shader->setVec3("pointLights[3].position", pointLightPositions[3]);
    shader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[3].constant", 1.0f);
    shader->setFloat("pointLights[3].linear", 0.09);
    shader->setFloat("pointLights[3].quadratic", 0.032);

    shader->setVec3("spotLights[0].position", pointLightPositions[0]);
    shader->setVec3("spotLights[0].direction", glm::vec3(0.2, 0.8, 0.2));
    shader->setVec3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[0].constant", 1.0f);
    shader->setFloat("spotLights[0].linear", 0.09);
    shader->setFloat("spotLights[0].quadratic", 0.032);
    shader->setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));

    // spotLight
    shader->setVec3("spotLights[1].position", pointLightPositions[1]);
    shader->setVec3("spotLights[1].direction", camera.Front);
    shader->setVec3("spotLights[1].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[1].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[1].constant", 1.0f);
    shader->setFloat("spotLights[1].linear", 0.09);
    shader->setFloat("spotLights[1].quadratic", 0.032);
    shader->setFloat("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(15.0f)));

    shader->setVec3("spotLights[2].position", pointLightPositions[2]);
    shader->setVec3("spotLights[2].direction", camera.Front);
    shader->setVec3("spotLights[2].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[2].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[2].constant", 1.0f);
    shader->setFloat("spotLights[2].linear", 0.09);
    shader->setFloat("spotLights[2].quadratic", 0.032);
    shader->setFloat("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[2].outerCutOff", glm::cos(glm::radians(15.0f)));

    shader->setVec3("spotLights[3].position", pointLightPositions[3]);
    shader->setVec3("spotLights[3].direction", camera.Front);
    shader->setVec3("spotLights[3].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[3].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[3].constant", 1.0f);
    shader->setFloat("spotLights[3].linear", 0.09);
    shader->setFloat("spotLights[3].quadratic", 0.032);
    shader->setFloat("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[3].outerCutOff", glm::cos(glm::radians(15.0f)));

    shader->setInt("lightType", (int)lightType);
    shader->setInt("maxRenderLights", 1);
}
bool fullFillObjectives()
{

    bool fullFill = false;
    if (objectives.getValue("recogerLampa") &&
        objectives.getValue("dejarLampa") &&
        objectives.getValue("dejarLampa") &&
        objectives.getValue("dejarJarron"))
    {
        fullFill = true;
        std::cout << "¡¡¡HAAAAAAS GANADOOOOOOOO!!" << std::endl;
    }
    else
    {
        system("cls");
        if (objectives.getValue("recogerLampa"))
        {
            std::cout << "Se ha recogido la lampara" << std::endl;
            //HACER COSAS AQUÍ
        }
        if (objectives.getValue("dejarLampa"))
        {
            std::cout << "Se ha dejado la lampara" << std::endl;
            //HACER COSAS AQUÍ
        }
     
        if (objectives.getValue("dejarCofre"))
        {
            std::cout << "Se ha dejado el michi" << std::endl;
            //HACER COSAS AQUÍ
        }

        if (objectives.getValue("dejarJarron"))
        {
            std::cout << "Se ha dejado el jarron" << std::endl;
            //HACER COSAS AQUÍ
        }
    }
    return fullFill;
}
